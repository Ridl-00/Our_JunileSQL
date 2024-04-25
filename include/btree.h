#pragma once
#ifndef B_PLUS_H_
#define B_PLUS_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * 节点结构和树结构定义
 * 宏MAX_ORDER定义为B+树的阶数
 */

#define MAX_ORDER 3 

typedef enum {
    CPC_M,          /* 中共党员 */
    P_CPC_M,        /* 中共预备党员 */
    CYLC_M,         /* 共青团员 */
    MASS,           /* 群众 */
    OTHERS          /* 其他 */
} Political;

typedef struct {
    time_t join_time;
} CPC_INFO; // 党员特征信息

typedef struct {
    time_t join_time;
    time_t date_of_application;
    bool is_recommended;
    bool is_adult;
    bool is_training_finished;
} CYLC_INFO; // 共青团员特征信息

typedef struct {
    bool is_sworn;
    bool is_procedure_finished;
    bool is_date_over;
} P_CPC_INFO; // 预备党员特征信息

typedef union {
    CPC_INFO CPC_info;
    CYLC_INFO CYLC_info;
    P_CPC_INFO P_CPC_M_info;
} Feature_info;

typedef struct {
    Political political; // 政治面貌
    int age;
    char id[10];
    char name[50];
    char class_number[10];
    Feature_info info;
} StudentRecord;

// 节点结构体
typedef struct Node {
    bool is_leaf; // 是否是叶节点
    int num_keys; // 当前键的数量
    int keys[MAX_ORDER - 1]; // 存储键的数组（学号）
    void *children[MAX_ORDER]; // 指向子节点或叶节点的指针

    // 如果是非叶节点为NULL
    StudentRecord records[MAX_ORDER - 1]; // 学生记录数组
    struct Node *next; // 指向下一个叶节点的指针
} Node;

// B+树结构体
typedef struct BPlusTree {
    Node *root; // 根节点
    int order;  // 树的顺序（最大和最小度）
} BPlusTree;



/*
 * 以下为B+数的一些基本接口的函数原型
 * 具体实现位于b_plus.c文件中
 */


/*
 * new_node
 * 创建一个新节点
 * @param is_beaf:
 * 如果 is_leaf 为 true： 创建的节点是一个叶节点
 * 如果 is_leaf 为 false： 创建的节点是一个内部节点（非叶节点）
 */
Node *new_node(bool is_leaf);
/*
 * new_bplus_tree
 * 创建一个新B+树
 * @param order: 指出新创建的B+树的阶数
 */
BPlusTree *new_bplus_tree(int order);
/*
 * 搜索操作
 * search_in_node
 * 查找对应键值应该插入的位置
 *
 * @param node: 对应的被查找节点
 * @param key: 查找的键值
 *
 */
int search_in_node(Node *node, int key);

Node *search(BPlusTree *tree, int key);

/*
 * 插入操作
 */

void insert_into_leaf(Node *leaf, int key, StudentRecord record);

void insert(BPlusTree *tree, int key, StudentRecord record);

void insert_non_full(Node *node, int key, StudentRecord record, BPlusTree *tree);

void split_child(Node *parent, int index, Node *child, BPlusTree *tree);
/*
 * 删除操作
 * delete_from_lea
 */

void delete_from_leaf(Node *leaf, int key);

void delete(BPlusTree *tree, int key);

void delete_from_node(Node *node, int key, BPlusTree *tree);

void borrow_from_left(Node *parent, int index);

void borrow_from_right(Node *parent, int index);

void merge_children(Node *parent, int index);

#endif
