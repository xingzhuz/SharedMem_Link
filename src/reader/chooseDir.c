
#include "process.h"

// 处理将文件写入到指定目录下，并处理同名文件问题
void chooseDir(char *directory, char *fullpath, size_t fullpathSize, char *filename, size_t filenameSize, FILE **file)
{
    // 获取用户指定的目录
    printf("请输入文件存放目录（默认为'resources'，直接回车使用默认目录）: ");
    fgets(directory, sizeof(directory), stdin);
    directory[strcspn(directory, "\n")] = '\0'; // 去掉换行符

    // 如果用户输入为空，使用默认目录
    if (strlen(directory) == 0)
    {
        strcpy(directory, "resources");
    }

    // 创建目录（如果不存在）
    mkdir(directory, 0755); // 可根据需要设置权限

    // 构建完整路径
    snprintf(fullpath, fullpathSize, "%s/%s", directory, filename);

    // 自定义的文件名存在的处理过程
    if (file_exists(fullpath))
    {
        printf("指定目录下文件已经存在, 为您创建副本: \n");
        // 同名文件处理，加编号处理
        generate_unique_filename(directory, filename, filenameSize);

        // 拼接正确的路径
        snprintf(fullpath, fullpathSize, "%s/%s", directory, filename);
        printf("新文件名: %s\n", filename);
    }

    // 打开文件用于写入
    *file = fopen(fullpath, "wb");
    if (*file == NULL)
    {
        perror("文件打开失败!");
        return;
    }
}