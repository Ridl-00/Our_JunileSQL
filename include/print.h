#ifndef PRINT_H
#define PRINT_H

#include <btree.h>
typedef void (*PrintFunction)(const StudentRecord *record);

void print_record(StudentRecord *record);

static void print_cylc_member(const StudentRecord *record);

static void print_cpc_member(const StudentRecord *record);

static void print_p_cpc_member(const StudentRecord *record);

static void print_mass_member(const StudentRecord *record);

static void print_others(const StudentRecord *record);

PrintFunction select_print_function(const Political political);

void print_bplus_tree(const BPlusTree *tree);

void print_node(const Node *node);

#endif