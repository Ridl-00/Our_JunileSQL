#pragma once
#ifndef OPERATING_H_
#define OPERATING_H_

#include <btree.h>
#include <stdio.h>

static void save_node(FILE *file, Node *node);

void save_bplus_tree(BPlusTree *tree, const char *filename);

BPlusTree *load_bplus_tree(const char *filename);

void insert_student_record(BPlusTree *tree, int key, StudentRecord record);

StudentRecord *read_student_record(BPlusTree *tree, int key);

// 删除学生记录
void delete_student_record(BPlusTree *tree, int key);

/*
 *统计操作
 */

void analyze_class(BPlusTree *tree, char *class_number);
// 统计个数
void count_political_by_class(Node * const root, const char *class_number, Class_info *count);
// 打印统计结果
void print_political_count_by_class(Class_info *count);

/*
 *修改操作
 */
// 修改记录基本信息
void change_record_info(StudentRecord *record, int order);
// 修改记录政治面貌
void change_record_politaical(StudentRecord *record);
// 修改记录政治信息
void change_record_poli_info(StudentRecord *record);

extern BPlusTree *create_database(const char *filename, int order);
#endif