
## 目录结构

```plaintext
netSocket/
├── Makefile            # 根目录的 Makefile
├── reader_files/       # 读进程资源保存位置
│   ├── cal.c      
│   └── test.txt        # 客户端可执行文件
├── bin/                # 存放生成的可执行文件
│   ├── server_ftp      # 服务器端可执行文件
│   └── client_ftp      # 客户端可执行文件
├── obj/                # 存放生成的对象文件(.o)
├── src/                # 所有源文件的根目录
│   ├── common/         # 写进程和读进程共有源文件
│   │   ├── data_checkout.c
│   │   ├── convert_encoding.c
│   │   ├── getfilename.c
│   │   ├── initSemShm.c
│   │   ├── sendjson.c
|   |   ├── sembuf.c
│   │   └── judge.c 
│   ├── writer/         # 写进程源文件
│   │   ├── writer.c
│   │   ├── fileSendData.c
│   │   └── stringSendData.c
│   └── reader/         # 读进程源文件
|       ├── chooseDir.c
│       ├── reader.c
│       └── writeIntoFile.c
└── include/            # 头文件存放目录
    └── process.h       # 公共头文件        
```




## 简介

- 使用共享内存实现了一个写进程和读进程之间的信息传输，支持大容量传输数据
- 使用管道进行读进程和写进程之间的信息反馈交互
- 使用 `CJson` 封装信息反馈数据的格式，使其统一格式，便于读写正确
- 使用信号量机制，保证读进程和写进程访问共享内存按序访问
- 写进程可以传输字符串、文件、任意目录下的文件，读进程接收数据，可以选择存储的目录位置
- 解决了同名文件的问题，也就是读进程接收文件后，指定目录下存在同名文件，使用编号方式处理，如 `test(1).c`
- 进行了数据完整性检验，使用若校验 (校验和) 方式校验
- 能传输各种文件，如 `txt`、`mp3`、`png`等格式文件，并且传输速率较快

## 环境配置


- `csjon`配置（没有`git`就去安装一个）

```shell
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON/
mkdir build
cd build/
cmake ..
make
make install
```

## 编译运行

```shell
# 编译
make

# 运行
./bin/writer
./bin/reader

# 删除所有可执行文件
make clean
```


## 开发日志


- 2024/9/11  
    - [x] 新增处理中文乱码问题
- 2024/9/18  
    - [x] 新增处理大文件一次性无法完全传输问题，使用分块传输思想解决
- 2024/9/25  
    - [x] 新增数据完整性检验部分，使用的` MD5` 码检验
- 2024/10/11 
    - [x] 添加管道功能，实现读进程和写进程额外信息交互，使用 `json` 数据封装以及管道通信
    - [x] 整理所有的源文件，整理成模块，可读性提升
- 2024/10/12 
    - [x] 修改了除传输数据，其他都通过管道进行信息交互，使用 `json` 封装
    - [x] 修复了能传输各种格式文件，比如图片、`mp3`
    - [x] 修复了速率问题，传输大容量文件速率提升
- 2024/10/16 
    - [x] 修复了传输字符串，接收方输入文件名不能为空，(之前可以为空，导致程序死掉)
- 2024/10/22 
    - [x] 修复了读进程和写进程双向管道之间的冲突，使用信号量标识，唯一访问权限，避免偶然性访问冲突，导致程序卡死
- 2024/10/23 
    - [x] 修复了 10/22 的那个问题，没处理好，如果写进程存在 write read 管道，且代码部分挨着很容易出现冲突问题，管道缓冲区等被填充还未被读取走，容易死锁，解决方案创建两个有名管道，一个写进程写、读进程读，一个写进程读、读进程写的两个管道，就不存在冲突问题了，也就不用使用信号量唯一访问了
    - [x] 修复了完整性代码，删除了MD5校验，数据传输部分不需要这么强的校验，使用若校验: 校验和校验，而且不用每一部分都去校验，最终总文件校验即可，虽然若校验可能30%~40%部分校验不到，但是数据传输的高速很重要，发送数据错误几率低，MD5检验是应用层用户自己手动需要校验的变校验
- 2024/10/24 
    - [x] 又发现一个BUG，多次发送字符串出现问题，信号量多使用了一次V操作，因为退出时使用了，结尾还使用了V(本意激活写进程继续发送下一个文件)
    - [x] 写进程往共享内存写入数据后，应该休眠一会儿(1毫秒)，确保接收方收到再通过管道发送数据长度，否则有BUG





## 运行演示



### 写进程

<img src="https://bu.dusays.com/2024/10/25/671bbc1040320.png" alt="写进程图片" width="500px" height="200px">





### 读进程

<img src="https://bu.dusays.com/2024/10/25/671bbc1274a5b.png" alt="读进程图片" width="500px" height="200px">

<img src="https://bu.dusays.com/2024/10/25/671bbc115e4aa.png" alt="读进程图片" width="500px" height="200px">
