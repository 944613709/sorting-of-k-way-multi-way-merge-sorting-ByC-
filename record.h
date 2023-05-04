#pragma once
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>


// 定义记录结构体，包括整型属性A和字符串属性B
typedef struct record_{
    int A;
    char B[20];
} Record;


// (内存中)生成随机记录
Record* initial_records(int num_records,int max_value) {
    srand(time(0)); // 使用当前时间作为种子
    Record* pRecord = (Record*)malloc(num_records * sizeof(Record));
    if (pRecord == NULL) {
        printf("Error: pRecord is NULL\n");
        exit(1);
    }
    for (int i = 0; i < num_records; i++) {
        pRecord[i].A = rand() % max_value;
        sprintf(pRecord[i].B, "Record %d", pRecord[i].A);
    }
    return pRecord;
}

// 输出二进制文件数据
void print_records(const char* filename, int print_num) {
    Record* pRecords=(Record*)malloc(sizeof(Record)*print_num);
    if (pRecords == NULL) {
        printf("Fail to malloc memory\n");
        return;
    }
    FILE* fr = fopen(filename, "rb");
    if (fr == NULL) {
        printf("Error opening file");
        exit(1);
    }
    size_t read_num=fread(pRecords, sizeof(Record), print_num, fr);
    if (read_num < print_num) {
        printf("Error: read_num:%llu < print_num:%d\n", read_num, print_num);
    }
    for (int i = 0; i < read_num; i++) {
        printf("Seq:%-6d A:%-6d  B:%-10s\n", i + 1, pRecords[i].A, pRecords[i].B);
    }
    fclose(fr);
}


// 初始化一个二维记录数组
// 这个数组有m个指针, 每个指针分配n个空间
Record** createRecord2(int m) {
    Record** pRecord2 = (Record**)malloc(sizeof(Record*) * m);
    if (pRecord2 == NULL) {
        printf("no more memory");
        exit(1);
    }
    for (int i = 0; i < m; i++) {
        pRecord2[i] = NULL; // 给每个数组里每个指针分配NULL
    }
    return pRecord2;
}


// 求出一个数组中的最小值的索引
int getMin(Record* pRecord, int ways,int max_value) {
    int min = -1;
    for (int i = 0; i < ways; i++) {
        if (pRecord[i].A <= max_value) {
            min = i;
            break;
        }
    }
    // 已经读取完全部元组, 结束程序
    if (min == -1) {
        return min;
    }
    for (int i = 0; i < ways; i++) {
        if ((pRecord[i].A < pRecord[min].A) && pRecord[i].A<=max_value) {
            min = i;
        }
    }
    return min;
}

