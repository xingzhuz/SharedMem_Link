#include "process.h"

#define FIFO_WRITE "fifo_write" // 写入管道名
#define FIFO_READ "fifo_read"   // 读取管道名

int main()
{
    char *buffer; // 共享内存
    FILE *file = NULL;
    char filename[256];
    char status[128], message[128];    // 解析的 json 格式数据部分]
    char directory[256] = "resources"; // 默认目录
    char fullpath[512];                // 文件存储完整路径
    char response[256];                // 接收写进程的反馈信息

    // 初始化信号量和共享内存
    int semid = initSemShm(&buffer);

    // 打开有名管道 FIFO_WRITE 进行读取，FIFO_READ 用于写入
    int fifo_read_fd = open(FIFO_WRITE, O_RDONLY);
    if (fifo_read_fd == -1)
    {
        perror("open FIFO_READ failed");
        exit(1);
    }
    int fifo_write_fd = open(FIFO_READ, O_WRONLY);
    if (fifo_write_fd == -1)
    {
        perror("open FIFO_WRITE failed");
        exit(1);
    }

    while (1)
    {
        unsigned int total_checksum = 0; // 接收到的数据块的总校验和

        ssize_t bytes_read = read(fifo_read_fd, response, sizeof(response));
        if (bytes_read > 0)
        {
            response[bytes_read] = '\0'; // 确保字符串结束

            // 解析 json 数据
            parse_json_message(response, status, message);
        }
        else if (bytes_read == -1)
        {
            perror("管道读取失败");
        }
        else
        {
            printf("管道已关闭，未接收到任何数据。\n");
        }

        // 判断是否为结束标志
        if (strcmp(status, "EOF") == 0)
        {
            printf("接收到结束标志，通信结束。\n");
            break;
        }

        if (strcmp(status, "error") == 0)
        {
            // 获取用户自定义的文件名
            do
            {
                printf("请输入接收数据后的文件名: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = '\0'; // 去掉换行符

                if (strlen(filename) == 0)
                {
                    printf("文件名不能为空，请重新输入。\n");
                }
            } while (strlen(filename) == 0); // 继续循环直到输入有效的文件名

            // 切换到对应的文件位置，并处理同名文件问题
            chooseDir(directory, fullpath, sizeof(fullpath), filename, sizeof(filename), &file);
        }

        else
        {
            // 等待发送的文件名
            read(fifo_read_fd, response, sizeof(response));
            // 解析 json 数据
            parse_json_message(response, status, message);

            printf("发来的文件是: %s\n", message);
            // 复制到 filename 上
            strcpy(filename, message);

            // 将文件保存到指定目录，并处理如果当前目录存在同名文件问题
            chooseDir(directory, fullpath, sizeof(fullpath), filename, sizeof(filename), &file);
        }

        // 逐块接收数据
        while (1)
        {
            // P 操作：等待信号量 1（确保有数据可读取）
            P(semid, 1);

            // 检查是否接收到结束标志（"END_OF_DATA"）
            if (strcmp(buffer, "END_OF_DATA") == 0)
            {
                printf("\n接收完毕, 当前接收任务完成\n");
                V(semid, 0); // 注意这里别忘了，不然发送方就死循环等待了
                break;       // 跳出循环，结束接收
            }

            // 写数据到文件中
            total_checksum += writeIntoFile(file, buffer, semid, fifo_read_fd);
        }

        // 向写进程发送准备接收校验和的反馈
        write(fifo_write_fd, "start", 6);

        // 接收传来的总体校验和
        unsigned char received_final_checksum;
        read(fifo_read_fd, &received_final_checksum, 1);

        // 计算最终校验和的低8位
        unsigned char calculated_final_checksum = total_checksum & 0xFF;

        // 验证校验和
        if (calculated_final_checksum == received_final_checksum)
        {
            printf("校验和匹配，数据传输成功，等待继续传输\n");
            send_json_message(fifo_write_fd, "success", "数据校验成功");
        }
        else
        {
            printf("校验和不匹配，数据部分有误\n");
            send_json_message(fifo_write_fd, "error", "数据校验失败");
        }
    }

    // 分离共享内存
    shmdt(buffer);

    // 关闭管道
    close(fifo_write_fd);
    close(fifo_read_fd);

    // 关闭文件
    if (file)
    {
        fclose(file);
    }

    // 删除有名管道
    unlink(FIFO_WRITE);
    unlink(FIFO_READ);
    return 0;
}
