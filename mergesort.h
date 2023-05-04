#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "record.h"

#define RECORSDS_NUM 12345   // �����¼����
#define WAYS_NUM 18          // ����鲢�����·��
#define MAX_VALUE 30000     // �����¼��A���Ե����ֵ
#define BUFFER_SIZE 700      // �����ܻ�������С
#define SUB_BUF_SIZE 20      // �����Ӽ����ӻ�������С
#define OUTPUT_BUF_SIZE 200  // �����������С

const char* inputFile = "input.bin";  // �������ļ���
const char* outputFile = "ouput.bin"; // �������ļ���

FILE* subinput_fp[WAYS_NUM];  // �������ļ�ָ��
int recordNums[WAYS_NUM];     // ÿ���Ӽ��д洢�ļ�¼��


// �ڴ����������ݲ�д�����
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


// �ȽϺ���������qsort�����жԼ�¼��������
int compare(const void* a, const void* b) {
    return ((Record*)a)->A - ((Record*)b)->A;
}


// һ������
void firstSort() {
    printf("Subset size(%d): ", WAYS_NUM);
    for (int i = 0; i < WAYS_NUM; i++) {
        recordNums[i] = i < RECORSDS_NUM % WAYS_NUM ? RECORSDS_NUM / WAYS_NUM + 1 : RECORSDS_NUM / WAYS_NUM;
        printf("%d ", recordNums[i]);
    }
    printf("\n");

    // �������ж�ȡ����
    FILE* fr;
    if ((fr = fopen(inputFile, "rb")) == NULL) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    // ���������򲢱����ļ�
    for (int i = 0; i < WAYS_NUM; i++) {
        int n = recordNums[i];
        Record* pRecord = (Record*)malloc(n * sizeof(Record));
        if (pRecord == NULL) {
            printf("Error: pRecord is NULL\n");
            exit(1);
        }
        fread(pRecord, sizeof(Record), n, fr);
        // ʹ�ÿ�������⺯��
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


// ��������
void secondSort() {
    // ���뻺����
    Record** pRecord2 = createRecord2(WAYS_NUM); 
    // ��ways���ļ�������
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

    // ��ʼ��������
    int buffer_waitNum[WAYS_NUM];
    int waitNum[WAYS_NUM];        // ÿ���Ӽ�[��һ��]���ڵȵ�Ԫ���±�
    bool isEnd[WAYS_NUM];         // ÿ���Ӽ��Ƿ��Ѿ�����
    Record sorting[WAYS_NUM];     // ���������Ԫ��
    Record output[OUTPUT_BUF_SIZE];  // ������ȴ��������Ԫ��(���������)
    int outputNum = 0;            // ��ǰ�����������Ԫ������
    for (int i = 0; i < WAYS_NUM; i++) {
        buffer_waitNum[i] = SUB_BUF_SIZE;
        isEnd[i] = false;
        sorting[i] = pRecord2[i][0]; // �ѵ�һ��Ԫ�����������
        waitNum[i] = 1;
    }
    int writeCount = 0; // �������������Ĵ���
    FILE* fw;
    if ((fw = fopen("output.bin", "wb")) == NULL) {
        printf("Fail to open file: output!\n");
        exit(0);
    }
    while (true) {
        int min = getMin(sorting, WAYS_NUM, MAX_VALUE); // ��ǰ��СԪ�����ڵ��Ӽ�
        // �����������
        if (min == -1) {
            printf("Output data to disk for %d times\n", writeCount);
            printf("All the records are sorted finally, End!\n");
            // �ͷ���Դ
            for (int i = 0; i < WAYS_NUM; i++) {
                fclose(subinput_fp[i]);
                free(pRecord2[i]);
            }
            fclose(fw);
            free(pRecord2);
            break;
        }
       
        output[outputNum] = sorting[min]; // ��Сֵ�������������
        outputNum++;
        if (isEnd[min] == true) {
            sorting[min].A = MAX_VALUE + 1;
        }
        else {
            sorting[min] = pRecord2[min][waitNum[min]];  // ��min���ȴ���++
            waitNum[min]++;
        }

        // ����Ӽ��������Ѿ��Ƚ���
        if (waitNum[min] == buffer_waitNum[min]) {
            size_t read_num=fread(pRecord2[min], sizeof(Record), SUB_BUF_SIZE, subinput_fp[min]);
            // �Ӽ��ѱȽ���(���һ��Ԫ����sorting[]�л�δ������)
            if (read_num == 0) {
                isEnd[min] = true;
                waitNum[min] = -1;
            }
            else if (read_num < SUB_BUF_SIZE) {
                buffer_waitNum[min] = read_num;
                waitNum[min] = 0; // ���ڵȴ���Ԫ������
            }
            else {
                waitNum[min] = 0; // ���ڵȴ���Ԫ������
            } 
        }

        // �ж��Ƿ�������ݵ����
        if (outputNum == OUTPUT_BUF_SIZE) {
            // ����������������
            fwrite(output, sizeof(Record), OUTPUT_BUF_SIZE, fw);
            //printf("writeCount:%d   writeNums:%d  \n", writeCount+1, outputNum);
            outputNum = 0; // ����
            writeCount++;
        }
        else {
            // ���������δ������ȫ���Ӽ�������Ƚ���
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
                outputNum = 0; // ����
                writeCount++;
            }
        }
    }
}
