#include "process.h"

// 字符串发送函数
void stringSendData(char *input, int semid, char *buffer, int wfd, int rfd)
{
    unsigned int total_checksum = 0; // 总体校验和
    char tmp[128];                   // 发送数据的长度
    char response[10];               // 接收反馈

    // 传递字符串数据
    size_t input_len = strlen(input), offset = 0;
    printf("开始发送数据....\n");
    while (offset < input_len)
    {
        size_t chunk_size = (input_len - offset) > BLOCK_SIZE ? BLOCK_SIZE : (input_len - offset);

        // 发送数据块
        P(semid, 0);
        memcpy(buffer, input + offset, chunk_size);
        V(semid, 1);

        // 计算每块数据的校验和，并累加到总校验和
        total_checksum += simple_checksum(buffer, chunk_size);

        usleep(1000); // 发送慢一点

        // 发送数据部分的长度
        snprintf(tmp, sizeof(tmp), "%ld", chunk_size); // 将整数转换为字符串
        write(wfd, tmp, strlen(tmp) + 1);              // 通过管道发送长度

        offset += chunk_size;
    }

    // 发送当前任务结束标志
    P(semid, 0);
    strcpy(buffer, "END_OF_DATA"); // 明确的结束标志
    V(semid, 1);

    // 接受读进程准备接收校验和的反馈
    read(rfd, response, sizeof response);

    // 传输结束后，将总体校验和通过管道发送给写进程
    unsigned char final_checksum = total_checksum & 0xFF; // 取低8位
    write(wfd, &final_checksum, 1);                       // 管道传输总体校验和
}