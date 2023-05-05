#ifndef EXTERNALSORTING_H
#define EXTERNALSORTING_H

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include "record.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <queue>
#include <vector>
#define WAYS_NUM 18          // 定义归并排序的路数
const char *path_input = "input_records.bin";  // 待排序文件名
const char *path_output = "ouput_records.bin"; // 已排序文件名
FILE *subinput_fp[WAYS_NUM];  // 有序子文件指针
int recordNums[WAYS_NUM];     // 每个子集中存储的记录数
#define RECORDS_TOTAL_NUM 12345   // 定义记录总数
#define A_MAX 30000     // 定义记录中A属性的最大值
#define BUFFER_TOTAL_SIZE 700      // 定义总缓冲区大小
#define SUB_BUF_SIZE 20      // 有序子集合子缓冲区大小
#define OUTPUT_BUF_SIZE 200  // 输出缓冲区大小
using namespace std;

/**
 * 生成随机Records，最大值为max_value,内存中生成数据并写入外存
 * 写入inputFile
 */
void generateRecords() {
    //输出当前系统时间
    printf("当前运行时间：%fs----开始-生成Records数据\n\n", ((double) clock()) / CLK_TCK);

    assert(RECORDS_TOTAL_NUM <= BUFFER_TOTAL_SIZE * WAYS_NUM);
    assert(OUTPUT_BUF_SIZE + WAYS_NUM * (SUB_BUF_SIZE + 1) <= BUFFER_TOTAL_SIZE);
    FILE *fw;
    if ((fw = fopen(path_input, "wb")) == nullptr) {
        printf("Fail to open file\n");
        exit(0);
    }
    Record *pRecords = initialRecords(RECORDS_TOTAL_NUM, A_MAX);
    fwrite(pRecords, sizeof(Record), RECORDS_TOTAL_NUM, fw);
    fclose(fw);
    free(pRecords);

    //输出当前系统时间
    printf("当前运行时间：%fs----完成-生成Records数据\n\n", ((double) clock()) / CLK_TCK);
}


int compareForRecord(const void *a, const void *b) {
    return ((Record *) a)->A - ((Record *) b)->A;
}

// 一趟排序
void firstRun() {
    //输出当前系统时间
    printf("当前运行时间：%fs----开始-第一趟排序\n\n", ((double) clock()) / CLK_TCK);

    printf("一共划分的子集个数: %d", WAYS_NUM);
    for (int i = 0; i < WAYS_NUM; i++) {
        // 计算每个子集中存储的记录数大小
        recordNums[i] =
                i < RECORDS_TOTAL_NUM % WAYS_NUM ? RECORDS_TOTAL_NUM / WAYS_NUM + 1 : RECORDS_TOTAL_NUM / WAYS_NUM;
    }
    printf("\n");

    // 从输入中读取数据
    FILE *fr;
    if ((fr = fopen(path_input, "rb")) == nullptr) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    // 子数据排序并保存文件
    for (int i = 0; i < WAYS_NUM; i++) {
        int n = recordNums[i];// 每个子集中存储的记录数
        auto *pRecord = (Record *) malloc(n * sizeof(Record));
        if (pRecord == nullptr) {
            printf("Error: pRecord is NULL\n");
            exit(1);
        }
        // 读取数据
        fread(pRecord, sizeof(Record), n, fr);
        // 进行排序
        qsort(pRecord, n, sizeof(Record), compareForRecord);

        // 将第一趟的排序后的子集数据写入文件
        char subSet_name[20];
        sprintf(subSet_name, "subSet_%d.bin", i);
        subinput_fp[i] = fopen(subSet_name, "wb");
        if (subinput_fp[i] == nullptr) {
            printf("Fail to open subSet%c!\n", i);
            exit(0);
        }
        fwrite(pRecord, sizeof(Record), n, subinput_fp[i]);
        free(pRecord);
        fclose(subinput_fp[i]);
        std::cout << "第" << i << "个子集排序完成" << std::endl;
    }
    fclose(fr);
    //输出当前系统时间
    printf("当前运行时间：%fs----完成-第一趟排序\n\n", ((double) clock()) / CLK_TCK);
}


// 这是第二趟排序
void secondRun() {
    printf("当前运行时间：%fs----开始-第二趟排序\n\n", ((double) clock()) / CLK_TCK);
    // 输入缓冲区
    Record **pRecords_twoDim = createRecordsTwoDim(WAYS_NUM);
    // 向ways个文件读数据
    // 读取第一趟排序后的子集数据，每个子集都是去当前头部一部分元素，放入其对应的输入缓冲区
    for (int i = 0; i < WAYS_NUM; i++) {
        char subSet_name[20];
        sprintf(subSet_name, "subSet_%d.bin", i);
        subinput_fp[i] = fopen(subSet_name, "rb");
        if (subinput_fp[i] == nullptr) {
            printf("Fail to open subSet%c!\n", i);
            exit(0);
        }
        pRecords_twoDim[i] = (Record *) malloc(sizeof(Record) * SUB_BUF_SIZE);
        if (pRecords_twoDim[i] == nullptr) {
            printf("Error: pRecords_twoDim[%d] is NULL\n", i);
            exit(1);
        }
        fread(pRecords_twoDim[i], sizeof(Record), SUB_BUF_SIZE, subinput_fp[i]);
    }

    // 初始化缓冲区
    int buffer_waitNum[WAYS_NUM]; //
    int waitIndex[WAYS_NUM];        // 每个子集[下一个]正在等的元素下标
    bool isEnd[WAYS_NUM];         // 每个子集是否已经读完
    Record buffer_sorting[WAYS_NUM];     // 排序缓冲区,(正在排序的元素)
    Record buffer_output[OUTPUT_BUF_SIZE];  // 已排序等待被输出的元组(输出缓冲区)
    int outputNum = 0;            // 当前输出缓冲区的元素数量
    // 初始化
    for (int i = 0; i < WAYS_NUM; i++) {
        buffer_waitNum[i] = SUB_BUF_SIZE;
        isEnd[i] = false;   //下标为i的子集是否已经读完
        buffer_sorting[i] = pRecords_twoDim[i][0]; // 把第一个元组放入排序中
        waitIndex[i] = 1; // 下一个等待的元素下标
    }
    int writeCount = 0; // 输出缓冲区输出的次数
    //打开outputFile
    FILE *fw;
    if ((fw = fopen(path_output, "wb")) == nullptr) {
        printf("Fail to open file: buffer_output!\n");
        exit(0);
    }

    // 二路归并排序
    while (true) {
        //这里是未优化版本
        int minValue_index = getMinNotOptimized(buffer_sorting, WAYS_NUM, A_MAX); // 当前最小元素所在的子集

//        //这里是败者树优化版本
//        LoserTree tree;
//        InitialTree(tree,buffer_sorting);
//        int minValue_index = getMinByLoserTree(tree); // 当前最小元素所在的子集

        //这里是最小堆优化版本
//        priority_queue<int> min_heap;
//        int minValue_index = getMinByHeap(min_heap); // 当前最小元素所在的子集

        // 如果所有排序结束
        if (minValue_index == -1) {
            printf("所有的records完成归并!\n");
            for (int i = 0; i < WAYS_NUM; i++) {
                fclose(subinput_fp[i]);
                free(pRecords_twoDim[i]);
            }
            fclose(fw);
            free(pRecords_twoDim);
            printf("当前运行时间：%fs----结束-第二趟排序\n\n", ((double) clock()) / CLK_TCK);
            break;
        }

        buffer_output[outputNum] = buffer_sorting[minValue_index]; // 把当前排序缓冲区的最小值放入输出缓冲区
        outputNum++;
        if (isEnd[minValue_index]) {
            printf("第%d个子集已经读完\n", minValue_index);
            buffer_sorting[minValue_index].A = A_MAX + 1;// 标记为已经输出
        } else {
            buffer_sorting[minValue_index] = pRecords_twoDim[minValue_index][waitIndex[minValue_index]];  // 第min个等待区++
            waitIndex[minValue_index]++;
        }

        // 如果某个子集缓冲区已经比较完
        if (waitIndex[minValue_index] == buffer_waitNum[minValue_index]) {
            size_t read_num = fread(pRecords_twoDim[minValue_index], sizeof(Record), SUB_BUF_SIZE,
                                    subinput_fp[minValue_index]);
            // 子集已比较完(最后一个元素于sorting[]中还未输出外存)
            if (read_num == 0) {
                isEnd[minValue_index] = true;
                waitIndex[minValue_index] = -1;
            } else if (read_num < SUB_BUF_SIZE) {
                buffer_waitNum[minValue_index] = read_num;
                waitIndex[minValue_index] = 0; // 正在等待的元素置零
            } else {
                waitIndex[minValue_index] = 0; // 正在等待的元素置零
            }
        }

        // 判断是否输出数据到外存
        if (outputNum == OUTPUT_BUF_SIZE) {
            // 如果输出缓冲区已满，则输出到外存
            fwrite(buffer_output, sizeof(Record), OUTPUT_BUF_SIZE, fw);
            //printf("writeCount:%d   writeNums:%d  \n", writeCount+1, outputNum);
            outputNum = 0; // 置零
            writeCount++;// 输出缓冲区输出的次数
        } else {
            // 输出缓冲区未满，但全部子集已排序比较完
            int flag = 0;
            // 判断是否所有子集都已经比较完
            for (auto &i: buffer_sorting) {
                if (i.A <= A_MAX) {
                    flag = 1;
                    break;
                }
            }
            // 如果所有子集都已经比较完，但是输出缓冲区未满，则也得把最后剩余的量输出到外存
            if (flag == 0) {
                fwrite(buffer_output, sizeof(Record), outputNum, fw);
                //printf("writeCount:%d   writeNums:%d  \n", writeCount + 1, outputNum);
                outputNum = 0; // 置零
                writeCount++;
            }
        }
    }
}

#endif