#include "process.h"

// 判断输入是否为文件路径
int is_file(const char *path)
{
    struct stat buffer;
    // stat 函数的作用是获取 path 对应的文件或目录的信息，并将其存入 buffer 中
    // 通过 S_ISREG(buffer.st_mode) 检查该路径是否指向一个普通文件
    return (stat(path, &buffer) == 0 && S_ISREG(buffer.st_mode)); // 确保输入是文件
}

// 判断是否是文件，以是否有后缀名作为判断条件
int has_extension(const char *path)
{
    const char *dot = strrchr(path, '.'); // 查找最后一个点的位置
    return dot && dot != path;            // 点存在且不是路径的开头
}

// 检查文件是否存在的辅助函数
int file_exists(const char *filename)
{
    return access(filename, F_OK) == 0; // 文件存在返回 0，不存在返回 -1
}