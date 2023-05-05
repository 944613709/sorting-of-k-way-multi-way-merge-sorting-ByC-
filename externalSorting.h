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
#define WAYS_NUM 18          // ����鲢�����·��
const char *path_input = "input_records.bin";  // �������ļ���
const char *path_output = "ouput_records.bin"; // �������ļ���
FILE *subinput_fp[WAYS_NUM];  // �������ļ�ָ��
int recordNums[WAYS_NUM];     // ÿ���Ӽ��д洢�ļ�¼��
#define RECORDS_TOTAL_NUM 12345   // �����¼����
#define A_MAX 30000     // �����¼��A���Ե����ֵ
#define BUFFER_TOTAL_SIZE 700      // �����ܻ�������С
#define SUB_BUF_SIZE 20      // �����Ӽ����ӻ�������С
#define OUTPUT_BUF_SIZE 200  // �����������С
using namespace std;

/**
 * �������Records�����ֵΪmax_value,�ڴ����������ݲ�д�����
 * д��inputFile
 */
void generateRecords() {
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----��ʼ-����Records����\n\n", ((double) clock()) / CLK_TCK);

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

    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----���-����Records����\n\n", ((double) clock()) / CLK_TCK);
}


int compareForRecord(const void *a, const void *b) {
    return ((Record *) a)->A - ((Record *) b)->A;
}

// һ������
void firstRun() {
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----��ʼ-��һ������\n\n", ((double) clock()) / CLK_TCK);

    printf("һ�����ֵ��Ӽ�����: %d", WAYS_NUM);
    for (int i = 0; i < WAYS_NUM; i++) {
        // ����ÿ���Ӽ��д洢�ļ�¼����С
        recordNums[i] =
                i < RECORDS_TOTAL_NUM % WAYS_NUM ? RECORDS_TOTAL_NUM / WAYS_NUM + 1 : RECORDS_TOTAL_NUM / WAYS_NUM;
    }
    printf("\n");

    // �������ж�ȡ����
    FILE *fr;
    if ((fr = fopen(path_input, "rb")) == nullptr) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    // ���������򲢱����ļ�
    for (int i = 0; i < WAYS_NUM; i++) {
        int n = recordNums[i];// ÿ���Ӽ��д洢�ļ�¼��
        auto *pRecord = (Record *) malloc(n * sizeof(Record));
        if (pRecord == nullptr) {
            printf("Error: pRecord is NULL\n");
            exit(1);
        }
        // ��ȡ����
        fread(pRecord, sizeof(Record), n, fr);
        // ��������
        qsort(pRecord, n, sizeof(Record), compareForRecord);

        // ����һ�˵��������Ӽ�����д���ļ�
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
        std::cout << "��" << i << "���Ӽ��������" << std::endl;
    }
    fclose(fr);
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----���-��һ������\n\n", ((double) clock()) / CLK_TCK);
}


// ���ǵڶ�������
void secondRun() {
    printf("��ǰ����ʱ�䣺%fs----��ʼ-�ڶ�������\n\n", ((double) clock()) / CLK_TCK);
    // ���뻺����
    Record **pRecords_twoDim = createRecordsTwoDim(WAYS_NUM);
    // ��ways���ļ�������
    // ��ȡ��һ���������Ӽ����ݣ�ÿ���Ӽ�����ȥ��ǰͷ��һ����Ԫ�أ��������Ӧ�����뻺����
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

    // ��ʼ��������
    int buffer_waitNum[WAYS_NUM]; //
    int waitIndex[WAYS_NUM];        // ÿ���Ӽ�[��һ��]���ڵȵ�Ԫ���±�
    bool isEnd[WAYS_NUM];         // ÿ���Ӽ��Ƿ��Ѿ�����
    Record buffer_sorting[WAYS_NUM];     // ���򻺳���,(���������Ԫ��)
    Record buffer_output[OUTPUT_BUF_SIZE];  // ������ȴ��������Ԫ��(���������)
    int outputNum = 0;            // ��ǰ�����������Ԫ������
    // ��ʼ��
    for (int i = 0; i < WAYS_NUM; i++) {
        buffer_waitNum[i] = SUB_BUF_SIZE;
        isEnd[i] = false;   //�±�Ϊi���Ӽ��Ƿ��Ѿ�����
        buffer_sorting[i] = pRecords_twoDim[i][0]; // �ѵ�һ��Ԫ�����������
        waitIndex[i] = 1; // ��һ���ȴ���Ԫ���±�
    }
    int writeCount = 0; // �������������Ĵ���
    //��outputFile
    FILE *fw;
    if ((fw = fopen(path_output, "wb")) == nullptr) {
        printf("Fail to open file: buffer_output!\n");
        exit(0);
    }

    // ��·�鲢����
    while (true) {
        //������δ�Ż��汾
        int minValue_index = getMinNotOptimized(buffer_sorting, WAYS_NUM, A_MAX); // ��ǰ��СԪ�����ڵ��Ӽ�

//        //�����ǰ������Ż��汾
//        LoserTree tree;
//        InitialTree(tree,buffer_sorting);
//        int minValue_index = getMinByLoserTree(tree); // ��ǰ��СԪ�����ڵ��Ӽ�

        //��������С���Ż��汾
//        priority_queue<int> min_heap;
//        int minValue_index = getMinByHeap(min_heap); // ��ǰ��СԪ�����ڵ��Ӽ�

        // ��������������
        if (minValue_index == -1) {
            printf("���е�records��ɹ鲢!\n");
            for (int i = 0; i < WAYS_NUM; i++) {
                fclose(subinput_fp[i]);
                free(pRecords_twoDim[i]);
            }
            fclose(fw);
            free(pRecords_twoDim);
            printf("��ǰ����ʱ�䣺%fs----����-�ڶ�������\n\n", ((double) clock()) / CLK_TCK);
            break;
        }

        buffer_output[outputNum] = buffer_sorting[minValue_index]; // �ѵ�ǰ���򻺳�������Сֵ�������������
        outputNum++;
        if (isEnd[minValue_index]) {
            printf("��%d���Ӽ��Ѿ�����\n", minValue_index);
            buffer_sorting[minValue_index].A = A_MAX + 1;// ���Ϊ�Ѿ����
        } else {
            buffer_sorting[minValue_index] = pRecords_twoDim[minValue_index][waitIndex[minValue_index]];  // ��min���ȴ���++
            waitIndex[minValue_index]++;
        }

        // ���ĳ���Ӽ��������Ѿ��Ƚ���
        if (waitIndex[minValue_index] == buffer_waitNum[minValue_index]) {
            size_t read_num = fread(pRecords_twoDim[minValue_index], sizeof(Record), SUB_BUF_SIZE,
                                    subinput_fp[minValue_index]);
            // �Ӽ��ѱȽ���(���һ��Ԫ����sorting[]�л�δ������)
            if (read_num == 0) {
                isEnd[minValue_index] = true;
                waitIndex[minValue_index] = -1;
            } else if (read_num < SUB_BUF_SIZE) {
                buffer_waitNum[minValue_index] = read_num;
                waitIndex[minValue_index] = 0; // ���ڵȴ���Ԫ������
            } else {
                waitIndex[minValue_index] = 0; // ���ڵȴ���Ԫ������
            }
        }

        // �ж��Ƿ�������ݵ����
        if (outputNum == OUTPUT_BUF_SIZE) {
            // ����������������������������
            fwrite(buffer_output, sizeof(Record), OUTPUT_BUF_SIZE, fw);
            //printf("writeCount:%d   writeNums:%d  \n", writeCount+1, outputNum);
            outputNum = 0; // ����
            writeCount++;// �������������Ĵ���
        } else {
            // ���������δ������ȫ���Ӽ�������Ƚ���
            int flag = 0;
            // �ж��Ƿ������Ӽ����Ѿ��Ƚ���
            for (auto &i: buffer_sorting) {
                if (i.A <= A_MAX) {
                    flag = 1;
                    break;
                }
            }
            // ��������Ӽ����Ѿ��Ƚ��꣬�������������δ������Ҳ�ð����ʣ�������������
            if (flag == 0) {
                fwrite(buffer_output, sizeof(Record), outputNum, fw);
                //printf("writeCount:%d   writeNums:%d  \n", writeCount + 1, outputNum);
                outputNum = 0; // ����
                writeCount++;
            }
        }
    }
}

#endif