# hit-database-lab2
 hit-database-lab2

哈工大数据库系统实验2-模拟外部排序

# 说明

模拟真实情况，生成足够多的数据记录

采用败者树，最小堆，置换选择等算法来模拟外部排序及其优化

## 1：描述实验环境的构建（记录和文件的准备）；	

该实验模拟外部排序，*考虑内存有限的条件，来在内存中多次生成随机记录，直到生成到指定数据记录数，*设定条件:内存<总计Record大小<外存

\#define RECORDS_TOTAL_NUM 20000  // 定义记录总数
 \#define A_MAX 30000   // 定义记录中A属性的最大值
 \#define BUFFER_TOTAL_SIZE 1000   // 定义总缓冲区大小
 \#define SUB_BUF_SIZE 20   // 有序子集合子缓冲区大小
 \#define OUTPUT_BUF_SIZE 200 // 输出缓冲区大小

\#define WAYS_NUM 20     // 定义归并排序的路数

## 2：给出基本算法设计的伪代码或流程图；	

​                                               ![image-20230515171140784](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151711870.png)               

​                            第一趟排序图

**1.**   **第一步-生成初始归并段：**

a)   将外存大文件input_record作为流输入，利用置换-选择排序算法或者其他算法，将原文件划分为多个数据子集，输出为多个小的已排序好的records（初始归并段），保存至subSet 

b)   本实验中直接采用的是划分路数个数的子集，使得每个子集大小可以装入内存，对每个子集单独在内存中进行排序，排序后，输出为多个小的已排序好的records（初始归并段），保存至subSet 

 

 ![image-20230515171150205](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151711273.png)

​					第二趟排序图

\2.   **第二步-归并：**

a)   输入缓冲区初始化

​         i.     把之前的每个已排序好的初始归并段（子集）的前面一部分元素，分配到到内存中的其对应的输入缓冲区

b)   排序缓冲区初始化

​         i.     将每个输入缓冲区的头元素（最小元素），加入到排序缓冲区

c)   输入缓冲区工作：

​         i.     每次输入缓冲区元素被排序缓冲区取出一个元素之后，实时从对应初始归并段中补充元素

d)   排序缓冲区工作：

​         i.     通过特定算法（算法1：顺序遍历所有元素，找出最小值   算法2：通过最维护最小堆，找出最小值  算法3：通过维护败者树，找出最小值），选出排序缓冲区中最小元素，然后加入到输出缓冲区

​        ii.     每次排序缓冲区的元素被输出缓冲区取出一个之后，实时从输入缓冲区补充一个元素进排序缓冲区

e)   输出缓冲区工作：

​         i.     如果输出缓冲区满了，则把缓冲区内容输出到外存中，清空输出缓冲区继续工作

​        ii.     直到所有输入缓冲区为空

\3.   当归并完成时候，排序结束，此时output_record是已经排序好的文件

### 败者树

​                               ![image-20230515171252122](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151712203.png)

​    **设计的败者树示意图**

l 按照这个图顺序定义b0,b1~b4的顺序，b0必须从第一个最小叶子结点作为b0，才符合b_parnet = (b_index+b_size)/2 

l 叶子节点对应着初始归并段的输入缓冲区

l 每次都是记录败者节点，拿着胜者去向上比较



# 要求

1：自己设计记录格式，至少包括2个属性（A和B），其中A为数值型，B作为记录的内容类型不限。

2：随机生成足够数量的记录，并存储为外存文件（尽量选择2进制格式）。

3：基于数值型属性A，用高级语言实现多路归并排序算法，并分析性能（时间和空间）。

# 程序运行结果

\1.   随机生成20000条乱序数据记录存入input_record.bin二进制文件

​                               ![image-20230515171351575](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151713646.png)

​                控制台信息

 ![image-20230515171354771](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151713822.png)

生成的记录

\2.   进行第一趟排序，生成初始归并段

![image-20230515171402422](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151714516.png)

​                                控制台信息

 ![image-20230515171407973](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151714039.png)

生成的排序好的子集

\3.   进行第二趟排序，归并，并生成最终文件output_records.bin

   ![image-20230515171412262](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151714306.png)

控制台信息

 ![image-20230515171416389](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151714462.png)

​                         排序好的记录

\2.   外部排序结果展示

![image-20230515171430025](https://farsblog.oss-cn-beijing.aliyuncs.com/PicGo/202305151714266.png)

 
