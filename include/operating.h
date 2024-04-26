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

void delete_student_record(BPlusTree *tree, int key);

extern BPlusTree *create_database(const char *filename, int order);
#endif