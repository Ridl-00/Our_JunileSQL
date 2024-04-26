#include <btree.h>
#include <buffer.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>

typedef void (*InputFunction)(Feature_info *info);

void read_time(struct tm *time_info, Buffer *buff) {
    Buffer temp_buff
    printf("请输入时间 (格式: YYYY-MM-DD): ");
    fgets(buff->buffer, sizeof(buff->size), stdin);
    strptime(buff->buffer, "%Y-%m-%d", time_info);
}

bool read_bool(const char *prompt) {
    char response[4];
    printf("%s (yes/no): ", prompt);
    fgets(response, sizeof(response), stdin);
    return strcasecmp(response, "yes\n") == 0;
}

void read_cpc_info(Feature_info *info) {
    printf("输入学生相关信息：\n");
    read_time(&info->CCP_info.join_time);
}

void read_cylc_info(Feature_info *info) {
    printf("输入学生相关信息\n");
    read_time(&info->CYLC_info.join_time);
    printf("请输入提交入党申请的申请日期：\n");
    read_time(&info->CYLC_info.date_of_application);
    info->CYLC_info.is_recommended = read_bool("是否被推荐");
    info->CYLC_info.is_training_finished = read_bool("训练是否完成");
}

void read_p_cpc_info(Feature_info *info) {
    printf("输入学生相关信息：\n");
    info->P_CPC_info.is_sworn = read_bool("是否完成宣誓");
    info->P_CPC_info.is_date_over = read_bool("预备期是否已满");
    info->P_CPC_info.is_procedure_finished = read_bool("转正手续是否完成");
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
            printf("没有对应的特征信息输入函数。\n");
            return;
    }
    input_func(&record->info);
}