#include "header.h"
#include "tree_io.h"
#include "rmdir.h"

#define MAX_PARTS 100
#define MAX_PART_LEN 256

int is_empty_directory(TreeNode* dir) {
    return dir->left == NULL;
}

void remove_child(TreeNode* parent, TreeNode* target) {
    TreeNode* prev = NULL;
    TreeNode* curr = parent->left;

    while (curr) {
        if (curr == target) {
            if (prev) {
                prev->right = curr->right;
            } else {
                parent->left = curr->right;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->right;
    }
}

TreeNode* find_child_dir(TreeNode* parent, const char* name) {
    TreeNode* curr = parent->left;
    while (curr) {
        if (curr->type == 'd' && strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->right;
    }
    return NULL;
}

int split_path(const char* path, char parts[MAX_PARTS][MAX_PART_LEN]) {
    int count = 0;
    int j = 0;

    while (*path) {
        if (*path == '/') {
            if (j > 0) {
                parts[count][j] = '\0';
                count++;
                j = 0;
            }
            path++;
            continue;
        }
        if (j < MAX_PART_LEN - 1) {
            parts[count][j++] = *path;
        }
        path++;
    }
    if (j > 0) {
        parts[count][j] = '\0';
        count++;
    }
    return count;
}

int remove_dir(DirectoryTree* dTree, const char* dir_name) {
    TreeNode* parent = dTree->current;
    TreeNode* curr = parent->left;

    while (curr) {
        if (strcmp(curr->name, dir_name) == 0 && curr->type == 'd') {
            if (!is_empty_directory(curr)) {
                printf("rmdir: failed to remove '%s': Directory is not empty\n", dir_name);
                return -1;
            }
            remove_child(parent, curr);
            printf("Directory '%s' removed successfully.\n", dir_name);
            return 0;
        }
        curr = curr->right;
    }

    printf("Failed to remove '%s': No such directory\n", dir_name);
    return -1;
}

int remove_dir_path(DirectoryTree* dTree, const char* path) {
    char parts[MAX_PARTS][MAX_PART_LEN];
    int count = split_path(path, parts);

    if (count == 0) {
        printf("rmdir: invalid path\n");
        return -1;
    }

    TreeNode* original_current = dTree->current;
    for (int i = 0; i < count - 1; i++) {
        TreeNode* next_dir = find_child_dir(dTree->current, parts[i]);
        if (!next_dir) {
            printf("rmdir: failed to remove '%s': No such directory\n", path);
            dTree->current = original_current; // 원래 위치 복원
            return -1;
        }
        dTree->current = next_dir;
    }

    // 마지막 디렉터리 이름만 삭제 시도
    int ret = remove_dir(dTree, parts[count - 1]);

    dTree->current = original_current; // 작업 후 원래 위치로 복원

    return ret;
}

int remove_dir_p_path(DirectoryTree* dTree, const char* path) {
    char parts[MAX_PARTS][MAX_PART_LEN];
    int count = split_path(path, parts);

    if (count == 0) {
        printf("rmdir: invalid path\n");
        return -1;
    }

    TreeNode* original_current = dTree->current;

    // 뒤에서부터 하나씩 삭제 시도
    for (int i = count - 1; i >= 0; i--) {
        dTree->current = original_current; // 매번 원래 위치부터 시작
        for (int j = 0; j < i; j++) {
            TreeNode* next_dir = find_child_dir(dTree->current, parts[j]);
            if (!next_dir) {
                printf("rmdir: failed to remove '%s': No such directory\n", path);
                dTree->current = original_current;
                return -1;
            }
            dTree->current = next_dir;
        }

        int ret = remove_dir(dTree, parts[i]);
        if (ret != 0) {
            printf("rmdir: Failed to remove '%s'. Isn't it empty?\n", parts[i]);
            dTree->current = original_current;
            return ret;
        }
    }

    dTree->current = original_current;
    return 0;
}


