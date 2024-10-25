#include "process.h"

#define FIFO_WRITE "fifo_write" // 写入管道名
#define FIFO_READ "fifo_read"   // 读取管道名

int main()
{
    char temp_input[BUFFER_SIZE];
    char filename[256];
    char input[BUFFER_SIZE];                       // 用于存储用户输入内容或文件路径
    char *buffer;                                  // 共享内存指针
    char response[256], status[256], message[256]; // 接收接收方的数据校验反馈

    // 初始化信号量和共享内存
    int semid = initSemShm(&buffer);

    // 创建两个有名管道
    if (mkfifo(FIFO_WRITE, 0666) == -1)
    {
        perror("mkfifo FIFO_WRITE failed");
        exit(1);
    }
    if (mkfifo(FIFO_READ, 0666) == -1)
    {
        perror("mkfifo FIFO_READ failed");
        exit(1);
    }

    // 打开有名管道 FIFO_WRITE 进行写入，FIFO_READ 用于读取
    int fifo_write_fd = open(FIFO_WRITE, O_WRONLY);
    if (fifo_write_fd == -1)
    {
        perror("open FIFO_WRITE failed");
        exit(1);
    }
    int fifo_read_fd = open(FIFO_READ, O_RDONLY);
    if (fifo_read_fd == -1)
    {
        perror("open FIFO_READ failed");
        exit(1);
    }

    // 将共享内存初始化为空字符串，防止接收方读取到垃圾数据
    memset(buffer, 0, BLOCK_SIZE);

    while (1)
    {
        // 获取用户输入
        printf("输入要发送的字符串或文件路径: (输入'q'退出)");
        fgets(temp_input, sizeof(temp_input), stdin);
        temp_input[strcspn(temp_input, "\n")] = 0; // 移除换行符

        // 转换为 utf-8 编码，处理中文编码问题
        convert_encoding(temp_input, input, sizeof(input));

        // 判断是否是退出条件
        if (strcmp(input, "q") == 0) // 输入 'q'，则退出
        {
            // 通过管道发送退出信号
            send_json_message(fifo_write_fd, "EOF", "exit");
            break;
        }

        else if (is_file(input)) // 如果输入是文件路径
        {
            // 通过管道发送是否是文件（是）
            send_json_message(fifo_write_fd, "success", "ISFILE");
            usleep(1000);

            // 获取文件名并传递
            get_filename(input, filename);
            // 发送文件名
            send_json_message(fifo_write_fd, "success", filename);

            // 文件的写入共享内存，发送数据
            fileSendData(input, semid, buffer, fifo_write_fd, fifo_read_fd);
        }
        else // 输入的是字符串
        {
            // 判断是不是写错了的文件名字
            if (has_extension(input))
            {
                printf("文件不存在, 请重新输入!\n");
                continue;
            }

            // 通过管道发送是否是文件（不是）
            send_json_message(fifo_write_fd, "error", "ISNOTFILE");

            // 字符串的写入共享内存
            stringSendData(input, semid, buffer, fifo_write_fd, fifo_read_fd);
        }

        // 接收  状态反馈
        read(fifo_read_fd, response, sizeof(response));

        // 解析 json 数据
        parse_json_message(response, status, message);

        if (strcmp(status, "success") == 0)
        {
            printf("接收方数据校验成功!\n");
        }
        else if (strcmp(status, "error") == 0)
        {
            printf("接收方数据校验错误!\n");
        }

        printf("继续发送\n");
    }

    // 分离共享内存
    shmdt(buffer);

    // 关闭管道
    close(fifo_write_fd);
    close(fifo_read_fd);

    // 删除有名管道
    unlink(FIFO_WRITE);
    unlink(FIFO_READ);

    return 0;
}