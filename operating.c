#include <btree.h>
#include <stdio.h>
#include <buffer.h>
#include <stdbool.h>
#include <operating.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NDEBUG
#include <assert.h>

#define ADD_FILENAME_LENGTH 6
#define POLITICAL_NUM 5
#define MAX_CLASS_NUMBER 50



static char format_filename[MAX_FILENAME_LENGTH] = "";//����ͳһ�ļ�·�����ڹ����ļ�

Node *prev_leaf = NULL;


static inline void trans_filename_format(const char *filename){
    sprintf(format_filename, "./repo/%s", filename);
}//���ļ�·��ת��Ϊ./db/<filename>��ʽ

// ����ڵ�
void save_node(const Node *node, FILE *file) {
    fwrite(&node->is_leaf, sizeof(bool), 1, file);
    fwrite(&node->num_keys, sizeof(int), 1, file);
    fwrite(node->keys, sizeof(int), node->num_keys, file);
    if (node->is_leaf) {
        fwrite(node->records, sizeof(StudentRecord), node->num_keys, file);
    } else {
        for (int i = 0; i < node->num_keys; i++) {
            save_node((Node *)node->children[i], file);
        }
    }
}

// ����B+��
void save_bplus_tree(const BPlusTree *tree, const char *filename) {
    trans_filename_format(filename);
    FILE *file = fopen(format_filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }
    save_node(tree->root, file);
    fclose(file);
}

// ���ؽڵ�
Node *load_node(FILE *file) {
    Node *node = malloc(sizeof(Node));
    fread(&node->is_leaf, sizeof(bool), 1, file);
    fread(&node->num_keys, sizeof(int), 1, file);
    fread(node->keys, sizeof(int), node->num_keys, file);
    if (node->is_leaf) {
        if(prev_leaf != NULL){
            prev_leaf->next = node;
        }
        prev_leaf = node;
        fread(node->records, sizeof(StudentRecord), node->num_keys, file);
    } else {
        for (int i = 0; i < node->num_keys; i++) {
            node->children[i] = load_node(file);
        }
    }
    return node;
}

// ����B+��
BPlusTree *load_bplus_tree(const char *filename) {
    trans_filename_format(filename);
    FILE *file = fopen(format_filename, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }
    BPlusTree *tree = new_bplus_tree(filename, MAX_ORDER);
    tree->root = load_node(file);
    fclose(file);
    return tree;
}

// ����ѧ����¼
void insert_student_record(BPlusTree *tree, int key, StudentRecord record) {
    insert(tree, key, record);
}

// ��ȡѧ����¼
StudentRecord *read_student_record(BPlusTree *tree, int key) {
    Node *leaf = search(tree, key);
    if (leaf != NULL) {
        int index = search_in_node(leaf, key);
        if (index < leaf->num_keys && leaf->keys[index] == key) {
            return &leaf->records[index];
        }
    }
    return NULL;
}

void delete_student_record(BPlusTree *tree, int key) {
    delete_(tree, key);
}

void analyze_grade(BPlusTree *tree, char *grade)
{
    char class_list[MAX_CLASS_NUMBER][10];
    int grade_info[MAX_CLASS_NUMBER][5]={{0}};
    Node *root = tree->root;

    count_political_by_grade(root, grade, grade_info, class_list);
    print_political_count_by_grade(grade, grade_info, class_list);
}
// ͳ���꼶��ÿ���༶��Ա������ò��Ϣ
void count_political_by_grade(Node * const root, const char *grade, int grade_info[][POLITICAL_NUM], char (*class_list)[10])
{
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}

    // ��ʼ��class_info
    for(int i=0;i<MAX_CLASS_NUMBER;i++){
        strcpy(class_list[i], "empty");
    }

	int i, j;
    char record_grade[4];
	Node * c = root;
	while (!c->is_leaf)
		c = c->children[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
            sscanf(c->records[i].class_number, "%*4s%2s", record_grade);
            int find_flag=0, j=0;
            if (strcmp(grade, record_grade)==0){
                while (j<MAX_CLASS_NUMBER && strcmp(class_list[j], "empty")!=0){
                    if (strcmp(class_list[j], c->records[i].class_number)==0){
                        grade_info[j][c->records[i].political] += 1;
                        find_flag=1;
                        break;
                    }
                    j++;
                }
                if(j<MAX_CLASS_NUMBER && (!find_flag)){
                    strcpy(class_list[j], c->records[i].class_number);
                    grade_info[j][c->records[i].political] += 1;
                }
            }
		}
        if (c->next!=NULL){
            c=c->next;
        }
		else
			break;
	}


}

void print_political_count_by_grade(const char *grade, int grade_info[][POLITICAL_NUM], char (*class_list)[10])
{
    printf("�꼶��%s\n", grade);
    int i=0;
    while(i<MAX_CLASS_NUMBER && strcmp(class_list[i], "empty")!=0){
        printf("�༶��%s    ", class_list[i]);
        printf("�й���Ա��%d�� �й�Ԥ����Ա��%d�� ������Ա��%d�� Ⱥ�ڣ�%d�� ������%d��\n", grade_info[i][0], grade_info[i][1], grade_info[i][2], grade_info[i][3], grade_info[i][4]);
        i++;
    }
    if(i==0){
        puts("���꼶û�а༶��Ϣ");
    }
    return;

}



// ͳ�ư༶��Ϣ
void analyze_class(BPlusTree *tree, char *class_number)
{
    Class_info *count = (Class_info *)malloc(sizeof(Class_info));
    Node *root = tree->root;
    
    count_political_by_class(root, class_number, count);
    print_political_count_by_class(count);
}
// ͳ�ư༶����������ò��Ա����
void count_political_by_class(Node * const root, const char *class_number, Class_info *count)
{
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
    // ��ʼ��Class_info�ṹ��
    strcpy(count->class_number, class_number);
    for (int i=0;i<5;i++){
        count->counts[i]=0;
    }

	int i;
	Node * c = root;
	while (!c->is_leaf)
		c = c->children[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
            if (strcmp(class_number, c->records[i].class_number)==0){
                count->counts[c->records[i].political]++;
            }
		}
		// if (c->children[MAX_ORDER - 1] != NULL) {
		// 	c = c->children[MAX_ORDER - 1];
		// }
        if (c->next!=NULL){
            c=c->next;
        }
		else
			break;
	}
}
// ��ӡͳ�ƽ��
void print_political_count_by_class(Class_info *count)
{
    if(count->counts[0]==0 && count->counts[1]==0 && count->counts[2]==0 && count->counts[3]==0 && count->counts[4]==0){
        puts("�˰༶�޳�Ա��Ϣ");
        return;
    }
    printf("�༶��%s    ", count->class_number);
    printf("�й���Ա��%d�� �й�Ԥ����Ա��%d�� ������Ա��%d�� Ⱥ�ڣ�%d�� ������%d��\n", count->counts[0], count->counts[1], count->counts[2], count->counts[3], count->counts[4]);
    return;
}
// �޸ļ�¼������Ϣ
void change_record_info(StudentRecord *record, int order)
{
    if(order==1){
        puts("�������µ�����");
        char newname[50];
        scanf("%s", newname);
        strcpy(record->name, newname);
    }
    else if(order==2){
        puts("�������µİ༶");
        char newclass[10];
        scanf("%s", newclass);
        strcpy(record->class_number, newclass);
    }
    else if(order==3){
        puts("�������µ�����");
        scanf("%d", &record->age);
    }

    return;
}
// �޸ļ�¼������ò
void change_record_politaical(StudentRecord *record)
{
    puts("�������µ�������ò");
    printf("( <Political>������ѡ�� <��Ա>��<Ԥ����Ա>��<��Ա>��<Ⱥ��> �� <����> �ֱ���: 0 , 1, 2, 3, 4 5 ���) \n");
    int newpoli;
    scanf("%d", &newpoli);
    record->political=newpoli;
    read_feature_info(record);
    return;
}
// �޸ļ�¼������Ϣ
void change_record_poli_info(StudentRecord *record)
{
    puts("�������µ�������ò��Ϣ");
    read_feature_info(record);
    return;
}


extern BPlusTree *create_database(const char *filename, int order) {
    trans_filename_format(filename);
    BPlusTree *tree = malloc(sizeof(BPlusTree));
    tree->root = new_node(true); 
    tree->file = fopen(format_filename, "wb+");
    if (tree->file == NULL) {
        perror("Failed to open database file");
        free(tree);
        return NULL;
    }

    // д��B+���ĸ��ڵ����������
    fwrite(&(tree->order), sizeof(int), 1, tree->file);
    fwrite(&(tree->root->is_leaf), sizeof(bool), 1, tree->file);
    fwrite(&(tree->root->num_keys), sizeof(int), 1, tree->file);
    fwrite(tree->root->keys, sizeof(int), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->records, sizeof(StudentRecord), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->children, sizeof(void *), MAX_ORDER, tree->file);

    printf("���ݿ�'%s'�Ѿ�����\n", filename);
    fclose(tree->file);

    return tree;
}