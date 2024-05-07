/*************************************************************************
*	> File Name: include/print.h
*	> Author: 
*	> Mail: 
*	> Created Time: Fri Apr 26 17:15:23 2024
* ************************************************************************/

#ifndef PRINT_H
#define PRINT_H

#include <btree.h>
typedef void (*PrintFunction)(StudentRecord *record);

static void print_cylc_member(StudentRecord *record);

static void print_cpc_member(StudentRecord *record);

static void print_p_cpc_member(StudentRecord *record);

static void print_mass_member(StudentRecord *record);

static void print_others(StudentRecord *record);

PrintFunction select_print_function(Political political);

void print_bplus_tree(BPlusTree *tree);

#endif
