#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iconv.h>
#include <errno.h>
#include <cjson/cJSON.h>

#define BLOCK_SIZE 4096         // 每次传输 4K
#define BUFFER_SIZE (1024 * 32) // 代码中各种字符数组的缓冲区大小

// 共有的部分 -------------------------------
// 初始化信号量和共享内存
int initSemShm(char **buffer);

// 信号量的 PV 操作
void P(int semid, int semnum);
void V(int semid, int semnum);

// 判断输入是否为文件路径
int is_file(const char *path);

// 判断是否是文件，以是否有后缀名作为判断条件
int has_extension(const char *path);

// 检查文件是否存在的辅助函数
int file_exists(const char *filename);

// 发送 json 数据
void send_json_message(int fd, const char *status, const char *message);

// 解析 json 数据
void parse_json_message(char *buffer, char *status, char *message);

// 计算校验和
unsigned char simple_checksum(const char *data, size_t length);

// writer 部分 ---------------------------------------
// 处理中文编码问题
void convert_encoding(const char *input, char *output, size_t output_size);

// 获取文件名
void get_filename(const char *path, char *filename);

// 输入为文件发送数据部分
void fileSendData(char *input, int semid, char *buffer, int wfd, int rfd);

// 输入为字符串发送数据部分
void stringSendData(char *input, int semid, char *buffer, int wfd, int rfd);

// reader 部分 -----------------------------------------
// 接收方写数据在文件里
unsigned int writeIntoFile(FILE *file, char *buffer, int semid, int rfd);

// 获取带编号的文件名
void generate_unique_filename(const char *save_dir, char *filename, size_t size);

// 处理将文件写入到指定目录下，并处理同名文件问题
void chooseDir(char *directory, char *fullpath, size_t fullpathSize, char *filename, size_t filenameSize, FILE **file);
