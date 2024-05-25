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


// ��ӡ�й���Ա�ļ�¼
static void print_cpc_member(const StudentRecord *record) {
    printf("�й���Ա: %s, Student_ID: %d, Class_Number: %s, ����ʱ��: %d-%d-%d\n",
           record->name, record->student_id, record->class_number, record->info.CCP_info.join_time.tm_year + 1900,
           record->info.CCP_info.join_time.tm_mon + 1, record->info.CCP_info.join_time.tm_mday);
}

// ��ӡ������Ա�ļ�¼
static void print_cylc_member(const StudentRecord *record) {
    printf("������Ա: %s, Student_ID: %d, Class_Number: %s, ����ʱ��: %d-%d-%d, ",
           record->name, record->student_id, record->class_number, record->info.CYLC_info.join_time.tm_year + 1900,
           record->info.CYLC_info.join_time.tm_mon + 1, record->info.CYLC_info.join_time.tm_mday,
           record->info.CYLC_info.date_of_application.tm_year + 1900,
           record->info.CYLC_info.date_of_application.tm_mon + 1,
           record->info.CYLC_info.is_application_submitted ? "��" : "��"
           );
    if(record->info.CYLC_info.is_application_submitted){
        printf("�ύ�뵳����ʱ��: %d-%d-%d\n", record->info.CYLC_info.date_of_application.tm_mday);
    }
}

// ��ӡԤ����Ա�ļ�¼
static void print_p_cpc_member(const StudentRecord *record) {
    printf("Ԥ����Ա: %s, Student_ID: %d, Class_Number: %s, �Ƿ��������: %s, �Ƿ����ת������: %s\n",
           record->name, record->student_id, record->class_number,
           record->info.P_CPC_info.is_sworn ? "��" : "��",
           record->info.P_CPC_info.is_procedure_finished ? "��" : "��");
}
// ��ӡȺ�ڵļ�¼
static void print_mass_member(const StudentRecord *record) {
    printf("Ⱥ��: %s, Student_ID: %d, Class_Number: %s\n", record->name, record->student_id, record->class_number);
}
// ��ӡ����������ò�ļ�¼
static void print_others(const StudentRecord *record) {
    printf("����: %s, Student_ID: %d, Class_Number: %s\n", record->name, record->student_id, record->class_number);
}

// ����������òѡ���ӡ����
PrintFunction select_print_function(Political political) {
    switch (political) {
        case CPC_M:
            return print_cpc_member;
        case CYLC_M:
            return print_cylc_member;
        case P_CPC_M:
            return print_p_cpc_member;
        case MASS:
            return print_mass_member;
        case OTHERS:
            return print_others;
        default:
            return print_others;
    }
}

// ����B+������ӡ���м�¼
void print_bplus_tree(const BPlusTree *tree) {
    Node *current = tree->root;
	if (current == NULL) {
		printf("Empty tree.\n");
		return;
	}
    while (current) {
        for (int i = 0; i < current->num_keys; i++) {
            print_record(&current->records[i]);
        }
        // �����Ҷ�ڵ㣬��������һ��Ҷ�ڵ�
        if (current->is_leaf) {
            current = current->next;
        } else {
            // ������ڲ��ڵ㣬��������±���
            current = (Node *)current->children[0];
        }
    }
}

void print_record(StudentRecord *record)
{
    PrintFunction print_function = select_print_function(record->political);
    print_function(record);
}