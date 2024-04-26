#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <btree.h>
#include <stdio.h>
extern BPlusTree *create_database(const char *filename, int order);

extern BPlusTree *open_database(const char *filename);
#endif