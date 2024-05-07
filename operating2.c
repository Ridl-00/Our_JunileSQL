#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// INSERTION

/* Creates a new record to hold the value
 * to which a key refers.
 */
StudentRecord * make_StudentRecord(int value) {
	StudentRecord * new_StudentRecord = (StudentRecord *)malloc(sizeof(StudentRecord));
	if (new_StudentRecord == NULL) {
		perror("StudentRecord creation.");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Enter student's id: ");
		// 读取学号
    	if (scanf("%9s", new_StudentRecord->id) != 1) {
    		// 处理输入错误
    		printf("Error: Invalid input for student's id.\n");
    		// 可以在这里添加更多的错误处理逻辑
    		exit(EXIT_FAILURE); // 或者其他适当的错误处理
}
	}
	return new_StudentRecord;
}