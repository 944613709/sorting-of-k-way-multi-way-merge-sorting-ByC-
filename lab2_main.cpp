#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <time.h>
#include "externalSorting.h"

int main() {
    clock_t start = clock();
    generate_records();
      int print_num_forExample = 5;//����չʾʾ���ĸ���
      std::cout << "������ɵ�����(��������ʾ��)��\n";
      print_binFile_for_records(inputFile, print_num_forExample);
      std::cout<<"\n";
    firstSort();
    secondSort();

    printf("�����ⲿ���������ʱ�䣺%fs\n\n", ((double)(clock() - start)) / CLK_TCK);
    std::cout << "�ⲿ������������������(��������ʾ��)��\n";
    print_binFile_for_records(outputFile, print_num_forExample);
//    print_binFile_for_records(outputFile, RECORDS_TOTAL_NUM+1);
    return 0;
}

