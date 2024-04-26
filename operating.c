#include <btree.h>
#include <stdio.h>
#include <stdbool.h>
#include <operating.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ORDER 3

// 保存节点到文件
static void save_node(FILE *file, Node *node) {
    fwrite(node, sizeof(Node), 1, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            save_node(file, (Node *)node->children[i]);
        }
    }
}

// 将B+树保存到文件
void save_bplus_tree(BPlusTree *tree, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("无法打开文件进行写入操作\n");
        return;
    }
    fwrite(&tree->order, sizeof(int), 1, file); // 写入树的顺序
    save_node(file, tree->root); // 保存根节点和所有子节点
    fclose(file);
}

// 从文件读取节点
Node *load_node(FILE *file, int order) {
    Node *node = (Node *)malloc(sizeof(Node));
    fread(node, sizeof(Node), 1, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            node->children[i] = load_node(file, order);
        }
    }
    return node;
}

// 从文件加载B+树
BPlusTree *load_bplus_tree(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件进行读取操作\n");
        return NULL;
    }
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    fread(&tree->order, sizeof(int), 1, file); // 读取树的顺序
    tree->root = load_node(file, tree->order); // 加载根节点和所有子节点
    fclose(file);
    return tree;
}

// 插入学生记录
void insert_student_record(BPlusTree *tree, int key, StudentRecord record) {
    insert(tree, key, record);
}

// 读取学生记录
StudentRecord *read_student_record(BPlusTree *tree, int key) {
    Node *leaf = search(tree, key);
    if (leaf != NULL) {
        int index = search_in_node(leaf, key);
        if (index < leaf->num_keys && leaf->keys[index] == key) {
            return &leaf->records[index];
        }
    }
    return NULL;
}

// 删除学生记录
void delete_student_record(BPlusTree *tree, int key) {
    delete(tree, key);
}

extern BPlusTree *create_database(const char *filename, int order) {
    BPlusTree *tree = malloc(sizeof(BPlusTree));
    tree->root = new_node(true); // 创建根节点
    
    tree->file = fopen(filename, "wb");
    if (tree->file == NULL) {
        perror("Failed to open database file");
        free(tree);
        return NULL;
    }

    // 写入B+树的根节点和其他数据
    fwrite(&(tree->order), sizeof(int), 1, tree->file);
    fwrite(&(tree->root->is_leaf), sizeof(bool), 1, tree->file);
    fwrite(&(tree->root->num_keys), sizeof(int), 1, tree->file);
    fwrite(tree->root->keys, sizeof(int), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->records, sizeof(StudentRecord), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->children, sizeof(void *), MAX_ORDER, tree->file);

    printf("数据库'%s'已经创建并被打开\n", filename);
    // 关闭文件
    fclose(tree->file);

    return tree;
}