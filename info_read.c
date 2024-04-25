#include <btree.h>
#include <buffer.h>
#include <info_read.h>
#include <stdio.h>

// 定义读取函数指针
typedef void (*ReadFunction)(const char *buffer, Feature_info *info);

// 读取 CPC_INFO 类型的数据
void read_cpc_info(const char *buffer, Feature_info *info) {
    // 使用 sscanf 读取 join_time 时间戳
    sscanf(buffer, "%ld", &info->CCP_info.join_time);
}

// 读取 CYLC_INFO 类型的数据
void read_cylc_info(const char *buffer, Feature_info *info) {
    // 使用 sscanf 读取 join_time、date_of_application 时间戳
    sscanf(buffer, "%ld %ld %d %d %d", &info->CYLC_info.join_time, &info->CYLC_info.date_of_application,
           (int *)&info->CYLC_info.is_recommended, (int *)&info->CYLC_info.is_active,
           (int *)&info->CYLC_info.is_training_finished);
}

// 读取 P_CPC_INFO 类型的数据
void read_p_cpc_info(const char *buffer, Feature_info *info) {
    // 使用 sscanf 读取布尔值 is_sworn, is_procedure_finished, is_data_over
    sscanf(buffer, "%d %d %d", (int *)&info->P_CCP_M_info.is_sworn, (int *)&info->P_CCP_M_info.is_procedure_finished,
           (int *)&info->P_CCP_M_info.is_data_over);
}

// 根据政治面貌读取特征信息
void read_feature_info(const char *buffer, Feature_info *info, Political political) {
    // 定义函数指针数组
    ReadFunction read_functions[] = {
        read_cpc_info,   // CPC_M
        read_p_cpc_info, // P_CPC_M
        read_cylc_info   // CYLC_M
    };

    // 根据政治面貌选择相应的读取函数
    if (political >= CPC_M && political <= CYLC_M) {
        read_functions[political](buffer, info);
    }
}

// 读取学生记录
void read_student_record(Buffer *buff, StudentRecord *record) {
    // 从缓冲区读取基本信息
    sscanf(buffer->data, "%d %9s %49s %9s %d", &record->age, record->id, record->name, record->class_number, (int *)&record->political);
    
    // 如果缓冲区中有特征信息
    if (buffer->has_feature_info) {
        // 根据政治面貌读取特征信息
        read_feature_info(buffer->data + 70, &record->info, record->political);
    } else {
        // 如果没有特征信息，则可以根据需求处理
        // 在这个示例中，简单地打印一条消息
        printf("No feature info available for student: %s\n", record->name);
    }
}