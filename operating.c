#include <btree.h>
#include <stdio.h>
#include <buffer.h>
#include <stdbool.h>
#include <operating.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADD_FILENAME_LENGTH 6

static char format_filename[MAX_FILENAME_LENGTH] = "";//用于统一文件路径便于管理文件



static inline void trans_filename_format(const char *filename){
    sprintf(format_filename, "./repo/%s", filename);
}//将文件路径转变为./db/<filename>形式

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
    trans_filename_format(filename);
    FILE *file = fopen(format_filename, "wb");
    if (!file) {
        printf("无法打开文件进行写入操作\n");
        return;
    }
    fwrite(&tree->order, sizeof(int), 1, file);
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
    trans_filename_format(filename);
    FILE *file = fopen(format_filename, "rb+");
    if (!file) {
        printf("文件不存在或文件加载失败\n");
        return NULL;
    }
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    fread(&tree->order, sizeof(int), 1, file); 
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

void delete_student_record(BPlusTree *tree, int key) {
    delete(tree, key);
}

extern BPlusTree *create_database(const char *filename, int order) {
    trans_filename_format(filename);
    BPlusTree *tree = malloc(sizeof(BPlusTree));
    tree->root = new_node(true); 
    tree->file = fopen(format_filename, "wb+");
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

    printf("数据库'%s'已经创建\n", filename);
    fclose(tree->file);

    return tree;
}