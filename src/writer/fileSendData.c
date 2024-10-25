#include "process.h"

// 文件发送函数
void fileSendData(char *input, int semid, char *buffer, int wfd, int rfd)
{
    int read_size;                   // 一次性读取的字节数，从文件中
    unsigned int total_checksum = 0; // 总体校验和
    char tmp[128];                   // 发送数据的长度
    char response[10];

    // 打开文件
    FILE *file = fopen(input, "rb");
    if (file == NULL)
    {
        perror("文件打开失败");
        return;
    }

    // 分块传输文件
    printf("开始发送数据....\n");
    P(semid, 0); // 等待接收方准备好接收数据块
    while ((read_size = fread(buffer, 1, BLOCK_SIZE - 1, file)) > 0)
    {
        // 计算每块数据的校验和，并累加到总校验和
        total_checksum += simple_checksum(buffer, read_size);

        V(semid, 1); // 通知接收方取数据

        usleep(1000); // 发送慢一点，确保接收方收到

        // 发送长度
        snprintf(tmp, sizeof(tmp), "%d", read_size); // 将整数转换为字符串
        write(wfd, tmp, strlen(tmp) + 1);            // 通过管道发送字符串

        if (feof(file))
            break;

        P(semid, 0); // 等待接收方准备好接收数据块
    }

    // 发送当前任务结束标志
    P(semid, 0);
    strcpy(buffer, "END_OF_DATA"); // 明确的结束标志
    V(semid, 1);

    read(rfd, response, sizeof response);

    // 传输结束后，将总体校验和通过管道发送给写进程
    unsigned char final_checksum = total_checksum & 0xFF; // 取低8位
    write(wfd, &final_checksum, 1);                       // 管道传输总体校验和

    fclose(file);
}