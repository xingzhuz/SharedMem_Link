#include "process.h"

// 函数：转换字符串编码
// 参数：
//    input       - 输入字符串，假设为 UTF-8 编码
//    output      - 输出缓冲区，用于存储转换后的字符串
//    output_size - 输出缓冲区的大小
// 返回值：
//    无返回值；如果转换失败，会输出错误信息

void convert_encoding(const char *input, char *output, size_t output_size)
{
    // 创建编码转换描述符，将输入编码转换为 UTF-8
    iconv_t cd = iconv_open("UTF-8", "UTF-8"); // 若输入是 UTF-8，可替换为其他编码
    if (cd == (iconv_t)(-1))
    {
        // 创建转换描述符失败，输出错误信息并返回
        perror("iconv_open failed");
        return;
    }

    size_t in_bytes = strlen(input); // 输入字节数
    size_t out_bytes = output_size;  // 输出缓冲区大小（字节数）
    char *in_ptr = (char *)input;    // 输入字符串指针
    char *out_ptr = output;          // 输出缓冲区指针

    // 清空输出缓冲区，确保其为空
    memset(output, 0, output_size);

    // 进行编码转换
    if (iconv(cd, &in_ptr, &in_bytes, &out_ptr, &out_bytes) == (size_t)(-1))
    {
        // 转换失败，输出错误信息
        perror("iconv failed");
    }

    // 关闭转换描述符，释放资源
    iconv_close(cd);
}
