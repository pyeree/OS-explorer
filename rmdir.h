#pragma once
#include "header.h"
#include "tree_io.h"

int is_empty_directory(TreeNode* dir);
void remove_child(TreeNode* parent, TreeNode* target);
int remove_dir(DirectoryTree* dTree, const char* dir_name);
int remove_dir_path(DirectoryTree* dTree, const char* path);
int remove_dir_p_path(DirectoryTree* dTree, const char* path);
