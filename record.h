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


/**
 * 在内存中生成随机Records，最大值为max_value，返回指向Records的指针
 * @param num_records
 * @param max_value
 * @return
 */
Record* initial_records(int num_records,int max_value) {
    srand(time(0)); // 使用当前时间作为种子
    Record* p_records = (Record*)malloc(num_records * sizeof(Record));
    if (p_records == NULL) {
        printf("Error: p_record is NULL\n");
        exit(1);
    }
    for (int i = 0; i < num_records; i++) {
        p_records[i].A = rand() % max_value;
        sprintf(p_records[i].B, "Record %d", p_records[i].A);
    }
    return p_records;
}

/**
 *  输出records的二进制文件数据
 * @param filename
 * @param print_num 需要输出的记录数,如果要读取整个文件中的所有记录，则可以设置 print_num 为文件中总记录数目
 */
void print_binFile_for_records(const char* filename, int print_num) {
    Record* p_records=(Record*)malloc(sizeof(Record)*print_num);
    if (p_records == NULL) {
        printf("Fail to malloc memory\n");
        return;
    }
    FILE* fr = fopen(filename, "rb");
    if (fr == NULL) {
        printf("Error opening file");
        exit(1);
    }
    size_t read_num=fread(p_records, sizeof(Record), print_num, fr);
    if (read_num < print_num) {
        printf("Error: read_num:%llu < print_num:%d\n", read_num, print_num);
    }
    for (int i = 0; i < read_num; i++) {
        printf("{record_id:%-6d A:%-6d  B:%-10s}\n", i + 1, p_records[i].A, p_records[i].B);
    }
    fclose(fr);
}



/**
 * 是一个用于动态创建二维数组的函数。它的输入参数是一个整数 m，表示需要创建的二维数组的行数，返回值是一个指向指针的指针，即一个二维数组。
 * 初始化一个二维记录数组,这个数组有m个指针, 每个指针分配n个空间
 * pRecords[m][n]
 * @param m 需要创建的二维数组的行数
 * @return pRecords_twoDim 二维数组
 */
Record** createRecordsTwoDim(int m) {
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



/**
 * 求出一个数组中的最小值的索引
 * @param pRecord
 * @param ways
 * @param max_value
 * @return
 */
int getMin(Record* pRecord, int ways,int max_value) {
    int min = -1;
    //
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

