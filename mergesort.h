#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "record.h"

#define RECORSDS_NUM 12345   // 定义记录总数
#define WAYS_NUM 18          // 定义归并排序的路数
#define MAX_VALUE 30000     // 定义记录中A属性的最大值
#define BUFFER_SIZE 700      // 定义总缓冲区大小
#define SUB_BUF_SIZE 20      // 有序子集合子缓冲区大小
#define OUTPUT_BUF_SIZE 200  // 输出缓冲区大小

const char* inputFile = "input.bin";  // 待排序文件名
const char* outputFile = "ouput.bin"; // 已排序文件名

FILE* subinput_fp[WAYS_NUM];  // 有序子文件指针
int recordNums[WAYS_NUM];     // 每个子集中存储的记录数


// 内存中生成数据并写入外存
void generate_records() {
    assert(RECORSDS_NUM <= BUFFER_SIZE * WAYS_NUM);
    assert(OUTPUT_BUF_SIZE + WAYS_NUM * (SUB_BUF_SIZE + 1) <= BUFFER_SIZE);
    FILE* fw;
    if ((fw = fopen(inputFile, "wb")) == NULL) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    Record* pRecords = initial_records(RECORSDS_NUM,MAX_VALUE);
    fwrite(pRecords, sizeof(Record), RECORSDS_NUM, fw);
    fclose(fw);
    free(pRecords);
    printf("generate data successfully!\n");
}


// 比较函数，用于qsort函数中对记录进行排序
int compare(const void* a, const void* b) {
    return ((Record*)a)->A - ((Record*)b)->A;
}


// 一趟排序
void firstSort() {
    printf("Subset size(%d): ", WAYS_NUM);
    for (int i = 0; i < WAYS_NUM; i++) {
        recordNums[i] = i < RECORSDS_NUM % WAYS_NUM ? RECORSDS_NUM / WAYS_NUM + 1 : RECORSDS_NUM / WAYS_NUM;
        printf("%d ", recordNums[i]);
    }
    printf("\n");

    // 从输入中读取数据
    FILE* fr;
    if ((fr = fopen(inputFile, "rb")) == NULL) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    // 子数据排序并保存文件
    for (int i = 0; i < WAYS_NUM; i++) {
        int n = recordNums[i];
        Record* pRecord = (Record*)malloc(n * sizeof(Record));
        if (pRecord == NULL) {
            printf("Error: pRecord is NULL\n");
            exit(1);
        }
        fread(pRecord, sizeof(Record), n, fr);
        // 使用快速排序库函数
        qsort(pRecord,n , sizeof(Record), compare);

        char subinputname[20];
        sprintf(subinputname, "subinput%d.bin", i);
        subinput_fp[i] = fopen(subinputname, "wb");
        if (subinput_fp[i] == NULL) {
            printf("Fail to open subinput%c!\n", i);
            exit(0);
        }
        fwrite(pRecord, sizeof(Record), n, subinput_fp[i]);
        free(pRecord);
        fclose(subinput_fp[i]);
    }
    fclose(fr);
}


// 二趟排序
void secondSort() {
    // 输入缓冲区
    Record** pRecord2 = createRecord2(WAYS_NUM); 
    // 向ways个文件读数据
    for (int i = 0; i < WAYS_NUM; i++) {
        char subinputname[20];
        sprintf(subinputname, "subinput%d.bin", i);
        subinput_fp[i] = fopen(subinputname, "rb");
        if (subinput_fp[i] == NULL) {
            printf("Fail to open subinput%c!\n", i);
            exit(0);
        }
        pRecord2[i] = (Record*)malloc(sizeof(Record) * SUB_BUF_SIZE);
        if (pRecord2[i] == NULL) {
            printf("Error: pRecord2[%d] is NULL\n",i);
            exit(1);
        }
        fread(pRecord2[i], sizeof(Record), SUB_BUF_SIZE, subinput_fp[i]);
    }

    // 初始化缓冲区
    int buffer_waitNum[WAYS_NUM];
    int waitNum[WAYS_NUM];        // 每个子集[下一个]正在等的元素下标
    bool isEnd[WAYS_NUM];         // 每个子集是否已经读完
    Record sorting[WAYS_NUM];     // 正在排序的元素
    Record output[OUTPUT_BUF_SIZE];  // 已排序等待被输出的元组(输出缓冲区)
    int outputNum = 0;            // 当前输出缓冲区的元素数量
    for (int i = 0; i < WAYS_NUM; i++) {
        buffer_waitNum[i] = SUB_BUF_SIZE;
        isEnd[i] = false;
        sorting[i] = pRecord2[i][0]; // 把第一个元组放入排序中
        waitNum[i] = 1;
    }
    int writeCount = 0; // 输出缓冲区输出的次数
    FILE* fw;
    if ((fw = fopen("output.bin", "wb")) == NULL) {
        printf("Fail to open file: output!\n");
        exit(0);
    }
    while (true) {
        int min = getMin(sorting, WAYS_NUM, MAX_VALUE); // 当前最小元素所在的子集
        // 所有排序结束
        if (min == -1) {
            printf("Output data to disk for %d times\n", writeCount);
            printf("All the records are sorted finally, End!\n");
            // 释放资源
            for (int i = 0; i < WAYS_NUM; i++) {
                fclose(subinput_fp[i]);
                free(pRecord2[i]);
            }
            fclose(fw);
            free(pRecord2);
            break;
        }
       
        output[outputNum] = sorting[min]; // 最小值放入输出缓冲区
        outputNum++;
        if (isEnd[min] == true) {
            sorting[min].A = MAX_VALUE + 1;
        }
        else {
            sorting[min] = pRecord2[min][waitNum[min]];  // 第min个等待区++
            waitNum[min]++;
        }

        // 如果子集缓冲区已经比较完
        if (waitNum[min] == buffer_waitNum[min]) {
            size_t read_num=fread(pRecord2[min], sizeof(Record), SUB_BUF_SIZE, subinput_fp[min]);
            // 子集已比较完(最后一个元素于sorting[]中还未输出外存)
            if (read_num == 0) {
                isEnd[min] = true;
                waitNum[min] = -1;
            }
            else if (read_num < SUB_BUF_SIZE) {
                buffer_waitNum[min] = read_num;
                waitNum[min] = 0; // 正在等待的元素置零
            }
            else {
                waitNum[min] = 0; // 正在等待的元素置零
            } 
        }

        // 判断是否输出数据到外存
        if (outputNum == OUTPUT_BUF_SIZE) {
            // 如果输出缓冲区已满
            fwrite(output, sizeof(Record), OUTPUT_BUF_SIZE, fw);
            //printf("writeCount:%d   writeNums:%d  \n", writeCount+1, outputNum);
            outputNum = 0; // 置零
            writeCount++;
        }
        else {
            // 输出缓冲区未满，但全部子集已排序比较完
            int flag = 0;
            for (int i = 0; i < WAYS_NUM; i++) {
                if (sorting[i].A <= MAX_VALUE) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                fwrite(output, sizeof(Record), outputNum, fw);
                //printf("writeCount:%d   writeNums:%d  \n", writeCount + 1, outputNum);
                outputNum = 0; // 置零
                writeCount++;
            }
        }
    }
}
