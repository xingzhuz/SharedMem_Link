# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -g

# 源文件目录
WRITER_DIR = src/writer
READER_DIR = src/reader
COMMON_DIR = src/common

# 头文件目录
INCLUDE_DIR = include

# 对象文件目录
OBJ_DIR = obj

# 可执行文件目录
BIN_DIR = bin

# 可执行文件名
WRITER_TARGET = $(BIN_DIR)/writer
READER_TARGET = $(BIN_DIR)/reader

# 自动获取各目录下的 .c 文件
WRITER_SRCS = $(wildcard $(WRITER_DIR)/*.c)
READER_SRCS = $(wildcard $(READER_DIR)/*.c)
COMMON_SRCS = $(wildcard $(COMMON_DIR)/*.c)

# 将 .c 文件转换为 .o 文件，存放在 obj/ 目录中
WRITER_OBJS = $(patsubst $(WRITER_DIR)/%.c, $(OBJ_DIR)/%.o, $(WRITER_SRCS))
READER_OBJS = $(patsubst $(READER_DIR)/%.c, $(OBJ_DIR)/%.o, $(READER_SRCS))
COMMON_OBJS = $(patsubst $(COMMON_DIR)/%.c, $(OBJ_DIR)/%.o, $(COMMON_SRCS))

# 包含头文件
INCLUDES = -I$(INCLUDE_DIR)

# 伪目标：同时生成 writer 和 reader 的可执行文件
all: $(WRITER_TARGET) $(READER_TARGET)

# 规则：生成写进程可执行文件
$(WRITER_TARGET): $(WRITER_OBJS) $(COMMON_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lcjson

# 规则：生成读进程可执行文件
$(READER_TARGET): $(READER_OBJS) $(COMMON_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lcjson

# 规则：编译 writer/ 目录下的 .c 文件生成 .o 文件
$(OBJ_DIR)/%.o: $(WRITER_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 规则：编译 reader/ 目录下的 .c 文件生成 .o 文件
$(OBJ_DIR)/%.o: $(READER_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 规则：编译 common/ 目录下的 .c 文件生成 .o 文件
$(OBJ_DIR)/%.o: $(COMMON_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 创建 bin 和 obj 目录
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# 清理生成的文件
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -f fifo_write fifo_read
