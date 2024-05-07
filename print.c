/*************************************************************************
*	> File Name: print.c
*	> Author: 
*	> Mail: 
*	> Created Time: Fri Apr 26 17:08:49 2024
* ************************************************************************/

#include <btree.h>
#include <print.h>
#include <time.h>
#include <stdio.h>


// 打印中共党员的记录
static void print_cpc_member(const StudentRecord *record) {
    printf("中共党员: %s, Student_ID: %d, 加入时间: %d-%d-%d\n",
           record->name, record->student_id, record->info.CCP_info.join_time.tm_year + 1900,
           record->info.CCP_info.join_time.tm_mon + 1, record->info.CCP_info.join_time.tm_mday);
}

// 打印共青团员的记录
static void print_cylc_member(const StudentRecord *record) {
    printf("共青团员: %s, Student_ID: %d, 入团时间: %d-%d-%d, 提交入党申请时间: %d-%d-%d\n",
           record->name, record->student_id, record->info.CYLC_info.join_time.tm_year + 1900,
           record->info.CYLC_info.join_time.tm_mon + 1, record->info.CYLC_info.join_time.tm_mday,
           record->info.CYLC_info.date_of_application.tm_year + 1900,
           record->info.CYLC_info.date_of_application.tm_mon + 1,
           record->info.CYLC_info.date_of_application.tm_mday);
}

// 打印预备党员的记录
static void print_p_cpc_member(const StudentRecord *record) {
    printf("预备党员: %s, Student_ID: %d, 是否完成宣誓: %s, 是否完成转正手续: %s\n",
           record->name, record->student_id,
           record->info.P_CPC_info.is_sworn ? "是" : "否",
           record->info.P_CPC_info.is_procedure_finished ? "是" : "否");
}

// 打印其他政治面貌的记录
static void print_others(const StudentRecord *record) {
    printf("其他: %s, Student_ID: %d\n", record->name, record->student_id);
}

// 根据政治面貌选择打印函数
PrintFunction select_print_function(Political political) {
    switch (political) {
        case CPC_M:
            return print_cpc_member;
        case CYLC_M:
            return print_cylc_member;
        case P_CPC_M:
            return print_p_cpc_member;
        case MASS:
        case OTHERS:
            return print_others;
        default:
            return print_others;
    }
}

// 遍历B+树并打印所有记录
void print_bplus_tree(const BPlusTree *tree) {
    Node *current = tree->root;
    while (current) {
        for (int i = 0; i < current->num_keys; i++) {
            PrintFunction print_function = select_print_function(current->records[i].political);
            print_function(&current->records[i]);
        }
        // 如果是叶节点，则跳到下一个叶节点
        if (current->is_leaf) {
            current = current->next;
        } else {
            // 如果是内部节点，则继续向下遍历
            current = (Node *)current->children[0];
        }
    }
}