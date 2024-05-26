#include <btree.h>
#include <print.h>
#include <info_read.h>
#include <operating.h>
#include <buffer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystrptime.h>

//#define NDEBUG
#include <assert.h>


bool save_flag = true;//��¼���ݸı��Ƿ񱻱���
BPlusTree *current_tree = NULL;//��¼��ǰ�������ڴ��е�B+��root��ַ
char current_filename[MAX_FILENAME_LENGTH];//��¼��ǰ�򿪵����ݿ��ļ�����

typedef enum{
    
    EXIT,//�˳����ݿ�
    CREATE,//�����Я��һ��filename��������filenameΪ������һ�����ݿ�
    INSERT,
    DELETE,
    OPEN,
    SHOW,// չʾ�������ݿ�
    WRITE,
    SEARCH,// ��ѧ�Ų�ѯѧ����Ϣ
    CHANGE,// �ı�ĳһѧ����Ϣ
    ANALYZE,// ���༶���꼶ͳ��ѧ��������ò��Ϣ
    COM_NUM,
    INVALID_INPUT

} COMMANDS;
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

const char *Commands[COM_NUM] = {".exit", ".create", ".insert", ".delete", ".open", ".show", ".write", ".search", ".change", ".analyze"};

static void eat_line(void){
    while(getchar() != '\n')
        continue;
}

static char *f_fgets(char *buffer, int n, FILE *stream){
    char *ptr = fgets(buffer, n, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0'; // �Ƴ����з�
    }
    return ptr;
}

static void help_prompt(void) { puts("ʹ��'.exit'������˳�����"); };
static void print_prompt(void) { printf("db/%s:> ", current_filename); }
static void save_prompt(const char *filename) { printf("�����ѱ������� %s�ļ�\n", filename); }
static void wrong_format_prompt(void) { puts("����������ʽ!"); };

COMMANDS do_meta_command(char Buffer[]) {

    bool Valid_Input=0;
    for(int i = 0; i < COM_NUM; i++){
        if(strcmp(Buffer, Commands[i])==0){
            Valid_Input=true;
            return (COMMANDS)i;
        }
    }
    if (!Valid_Input){
        // �������ʶ�������򷵻�һ��������
        return INVALID_INPUT;
    }
}


int main(int argc, char* argv[]) {
    
    Buffer *input_buffer = new_buffer();
    // FILE *InputFile, *OutputFile;
    // InputFile=fopen("example.csv", "r");
    // if (InputFile == NULL) {
    //     perror("Error opening file");
    //     return -1;
    // }
    

    while (true) {
        print_prompt();


        f_fgets(input_buffer->buffer, input_buffer->size, stdin);

        // switch (do_meta_command(buffer)) {
        COMMANDS temp=do_meta_command(input_buffer->buffer);
        switch (temp){
            case (CREATE):
            {
                puts("�������ݿ�����:");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                BPlusTree *temp = create_database(input_buffer->buffer, MAX_ORDER);
                if(current_tree == NULL && temp != NULL){
                    strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                    current_tree = temp;
                }
                continue;
            }
            case (INSERT):
            {
                save_flag = false;
                if(current_tree == NULL){
                    puts("����Ҫ�ȴ����ݿ�");
                    break;
                }
                printf("���� <Student_ID> <Age> <Name> <Class Number> <Political> ��ʽ����ѧ����Ϣ\n");
                printf("( <Political>������ѡ�� <��Ա>��<Ԥ����Ա>��<��Ա>��<Ⱥ��> �� <����> �ֱ���: 0 , 1, 2, 3, 4 5 ���) \n");
                print_prompt();
                int temp_info = -1;
                fgets(input_buffer->buffer, input_buffer->size, stdin);

                StudentRecord record;

                sscanf(input_buffer->buffer, "%d %d %s %s %d",&record.student_id, &record.age,
                                                            record.name, record.class_number, 
                                                            &temp_info);

                if(record.student_id < 0 || record.age < 0|| temp_info < 0){
                    wrong_format_prompt();
                    continue;
                }

                record.political = temp_info;
                read_feature_info(&record);
                insert_student_record(current_tree, record.student_id, record);
                continue;
            }
            case (DELETE):
            {
                save_flag = false;
                puts("�����뱻ɾ��ѧ����ѧ��");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    wrong_format_prompt();
                    eat_line(); 
                }
                eat_line();
                delete_student_record(current_tree, key);
                continue;
            }
            case (OPEN):
            {
                puts("������Ҫ�򿪵����ݿ�����");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                BPlusTree *temp = load_bplus_tree(input_buffer->buffer);
                if(temp != NULL){
                    current_tree = temp;
                    strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                }
                continue;
            }
            case (SHOW):
            {
                if(current_tree != NULL){
                    print_bplus_tree(current_tree);
                }
                continue;
            }
            case (WRITE):
            {
                if(current_tree == NULL){
                    puts("д��ʧ��");
                    puts("��ǰδ���������ݿ⡣����ʹ��.open��һ�����ݿ�");
                    continue;
                }
                else{
                    save_bplus_tree(current_tree, current_filename); 
                    save_prompt(current_filename);
                }

                save_flag = true;
                continue;
            }
            case (SEARCH):
            {
                puts("������Ҫ��ѯ��ѧ��ѧ��");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    puts("��ǰ��ѯ��ѧ����ϢΪ");
                    eat_line();
                }
                eat_line();
                Node *cur=search(current_tree, key);
                int index=search_in_node(cur, key);
                puts("��ǰ��ѯ��ѧ����ϢΪ");
                print_record(&cur->records[index]);
                break;
            }
            case (CHANGE):
            {
                save_flag=false;
                puts("������Ҫ�޸ĵ�ѧ����ѧ��");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    wrong_format_prompt();
                    eat_line();
                }
                eat_line();

                Node *cur=search(current_tree, key);
                int index=search_in_node(cur, key);
                puts("��ǰҪ�޸ĵ�ѧ����ϢΪ");
                print_record(&cur->records[index]);

                printf("<1: ����> <2: �༶> <3: ����> <4: ������ò> <5: ������Ϣ> (ѧ�Ų����޸�)\n");
                puts("�������޸����ͱ��:");
                int order;
                while(scanf("%d", &order) != 1){
                    wrong_format_prompt();
                    eat_line();
                }
                switch (order){
                    case 1:
                    case 2:
                    case 3:
                        change_record_info(&cur->records[index], order);
                        break;
                    case 4:
                        change_record_politaical(&cur->records[index]);
                        break;
                    case 5:
                        change_record_poli_info(&cur->records[index]);
                    default:
                        wrong_format_prompt();
                        break;
                }
                puts("�޸ĺ��ѧ����ϢΪ");
                print_record(&cur->records[index]);
                eat_line();
                continue;
            }
            case (ANALYZE):
            {
                int op;
                
                if(current_tree != NULL){
                    puts("�������ѯ��Ϣ");
                    puts("<grade> <class> ʾ��:2023 04932301; 23 all(��ѯ�����꼶)");
                    fgets(input_buffer->buffer, input_buffer->size, stdin);

                    char grade[10];
                    char class_number[10];
                    char all_sign[10];
                    sscanf("all", "%s", all_sign);
                    int ret=sscanf(input_buffer->buffer, "%s %s", grade, class_number);
                    if(ret!=2){
                        wrong_format_prompt();
                        continue;
                    }
                    if(strlen(grade)==4){
                        sscanf(grade, "%*2s%2s", grade);
                        // printf("�꼶��%s", grade);
                    }
                    if(strcmp(class_number, all_sign)==0){
                        analyze_grade(current_tree, grade);
                    }
                    else{
                        analyze_class(current_tree, class_number);
                    }
                }
                continue;
                
            }
            case (INVALID_INPUT):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            case (EXIT):
                delete_buffer(input_buffer);
                if(current_tree != NULL && !save_flag){
                    puts("�����ѱ��޸ģ��Ƿ񱣴��������޸�(y/n)");
                    if(getchar() == 'y'){
                        save_bplus_tree(current_tree, current_filename);
                        save_prompt(current_filename);
                    }
                }
                free_bplus_tree(current_tree);
                printf("Exiting the program.\n");
                return 0;
        }
    }
}