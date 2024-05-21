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
typedef struct tm tm;

typedef enum {
    CPC_M,          /* 0 中共党员 */
    P_CPC_M,        /* 1 中共预备党员 */
    CYLC_M,         /* 2 共青团员 */
    MASS,           /* 3 群众 */
    OTHERS,         /* 4 其他 */
    COUNT
} Political;

typedef struct {
    tm join_time;//入党时间
} CPC_INFO; // 党员特征信息

typedef struct {
    tm join_time;//加入共青团时间
    tm date_of_application;//申请日期
    bool is_recommended;//是否被推荐
    //有一个age数据记录年龄，无需判断是否成年
    bool is_training_finished;
} CYLC_INFO; // 共青团员特征信息

typedef struct {
    bool is_sworn;//是否完成宣誓
    bool is_date_over;//预备期是否已满
    bool is_procedure_finished;//转正手续是否完成
} P_CPC_INFO; // 预备党员特征信息

typedef union Feature_info{
    CPC_INFO CCP_info;
    CYLC_INFO CYLC_info;
    P_CPC_INFO P_CPC_info;
} Feature_info;

typedef struct {
    Political political; // 政治面貌
    int age;
    int student_id;
    char name[50];
    char class_number[10];
    Feature_info info;
} StudentRecord;

// 班级政治面貌统计
typedef struct {
    char class_number[10];
    int counts[5];  /* [0 中共党员, 1 中共预备党员, 2 共青团员 , 3 群众 , 4 其他 */
} Class_info;

// 节点结构体
typedef struct Node {
    bool is_leaf; // 是否是叶节点
    int num_keys; // 当前键的数量
    int keys[MAX_ORDER - 1]; // 存储键的数组（学号）
    void *children[MAX_ORDER]; // 指向子节点或叶节点的指针
    // void ** pointers;
    // 如果是非叶节点为NULL
    StudentRecord records[MAX_ORDER - 1]; // 学生记录数组
    struct Node *next; // 指向下一个叶节点的指针
} Node;

// B+树结构体
typedef struct BPlusTree {
    Node *root; // 根节点
    int order;  // 树的顺序（最大和最小度）
    FILE *file;
} BPlusTree;



/*
 * 以下为B+数的一些基本接口的函数原型
 * 具体实现位于btree.c文件中
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
BPlusTree *new_bplus_tree(const char *filename, int order);
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
void split_child(Node *parent, int index, Node *child, BPlusTree *tree);

void insert_into_leaf(Node *leaf, int key, StudentRecord record);

void insert(BPlusTree *tree, int key, StudentRecord record);

void insert_non_full(Node *node, int key, StudentRecord record, BPlusTree *tree);

/*
 * 删除操作
 */

void delete_from_leaf(Node *leaf, int key);

void delete_(BPlusTree *tree, int key);

void delete_from_node(Node *node, int key, BPlusTree *tree);

void borrow_from_left(Node *parent, int index);

void borrow_from_right(Node *parent, int index);

void merge_children(Node *parent, int index);


#endif
