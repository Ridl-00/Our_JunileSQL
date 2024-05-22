#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#include <btree.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <mystrptime.h>

#define TIME_BUFFER_SIZE 11
#define BOOL_BUFFER_SIZE 10

typedef void (*InputFunction)(Feature_info *info);


static void eat_line(void){
    while(getchar() != '\n')
        continue;
}

static char time_buffer[TIME_BUFFER_SIZE];
static char bool_buffer[BOOL_BUFFER_SIZE];

static void read_time(struct tm *time_info) {
    printf("������ʱ�� (��ʽ: YYYY-MM-DD): ");
    fgets(time_buffer, TIME_BUFFER_SIZE, stdin);
    mystrptime(time_buffer, "%Y-%m-%d", time_info);
    eat_line();
}

static bool read_bool(const char *prompt) {
    while(true){
        printf("%s (yes/no): ", prompt);
        fgets(bool_buffer, BOOL_BUFFER_SIZE, stdin);
        if(strcasecmp(bool_buffer, "yes\n") == 0){
            return true;
        }
        else if(strcasecmp(bool_buffer, "no\n") == 0){
            return false;
        }
        else{
            continue;
        }
    }
}

static void read_cpc_info(Feature_info *info) {
    
    printf("����ѧ���뵳ʱ�䣺\n");
    read_time(&info->CCP_info.join_time);
}

static void read_cylc_info(Feature_info *info) {
    printf("����ѧ�������Ϣ\n");
    puts("������ѧ��������ʱ��");
    read_time(&info->CYLC_info.join_time);
    printf("�������ύ�뵳������������ڣ�\n");
    read_time(&info->CYLC_info.date_of_application);
    info->CYLC_info.is_recommended = read_bool("�Ƿ��Ƽ�");
    info->CYLC_info.is_training_finished = read_bool("��ѵ�Ƿ����");
}

static void read_p_cpc_info(Feature_info *info) {
    printf("����ѧ�������Ϣ��\n");
    info->P_CPC_info.is_sworn = read_bool("�Ƿ��������");
    info->P_CPC_info.is_date_over = read_bool("Ԥ�����Ƿ�����");
    info->P_CPC_info.is_procedure_finished = read_bool("ת�������Ƿ����");
}

void read_feature_info(StudentRecord *record) {
    InputFunction input_func;
    switch (record->political) {
        case CPC_M:
            input_func = read_cpc_info;
            break;
        case CYLC_M:
            input_func = read_cylc_info;
            break;
        case P_CPC_M:
            input_func = read_p_cpc_info;
            break;
        default:
            return;
    }
    input_func(&record->info);
}