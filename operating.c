#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void count_political_by_class(Node *n, const char *class_number, PoliticalCount *count) {
    if (n == NULL) return;
    if (n->is_leaf) {
        for (int i = 0; i < n->num_keys; i++) {
            if (strcmp(n->records[i].class_number, class_number) == 0) {
                switch (n->records[i].political) {
                    case CPC_M:
                        count->CPC_M_count++;
                        break;
                    case P_CPC_M:
                        count->P_CPC_M_count++;
                        break;
                    case CYLC_M:
                        count->CYLC_M_count++;
                        break;
                    case MASS:
                        count->MASS_count++;
                        break;
                    case OTHERS:
                        count->OTHERS_count++;
                        break;
                }
            }
        }
    }
    else {
        for (int i = 0; i <= n->num_keys; i++) {
            count_political_by_class((Node *)n->pointers[i], class_number, count);
        }
    }
}

void print_political_count_by_class(BPlusTree *tree, const char *class_number) {
    printf("Class %s political count:\n", class_number);
    printf("CPC_M: %d\n", count.CPC_M_count);
    printf("P_CPC_M: %d\n", count.P_CPC_M_count);
    printf("CYLC_M: %d\n", count.CYLC_M_count);
    printf("MASS: %d\n", count.MASS_count);
    printf("OTHERS: %d\n", count.OTHERS_count);
}

void analyze_class(char *class_number)
{
    PoliticalCount count = {0};
    count_political_by_class(tree->root, class_number, &count);
    print_political_count_by_class(tree, class_number);
}