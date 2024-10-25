#include "process.h"

// 读进程接收数据，然后写入文件的函数（支持二进制数据）
unsigned int writeIntoFile(FILE *file, char *buffer, int semid, int rfd)
{
    char response[128];
    read(rfd, response, sizeof(response)); // 读取发送过来的数据
    int data_size = atoi(response);        // 将字符串转换为整数

    // 计算校验和
    unsigned int temp = simple_checksum(buffer, data_size);

    // 将接收到的数据块写入文件
    fwrite(buffer, 1, data_size, file); // 写入文件时使用二进制模式

    V(semid, 0); // 通知发送方可以继续发下一个数据块

    // 计算接收数据块的校验和，并累加到总校验和
    return temp;
}
