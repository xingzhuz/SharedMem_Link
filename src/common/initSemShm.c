#include "process.h"

// 初始化信号量，共享内存相关函数
int initSemShm(char **buffer)
{
    int shmid, semid;

    key_t shm_key, sem_key;

    // 使用 ftok 生成键值
    shm_key = ftok("/tmp", 's'); // 使用 '/tmp' 文件路径生成共享内存的键值
    if (shm_key == -1)
    {
        perror("ftok shm_key error");
        return -1;
    }

    sem_key = ftok("/tmp", 's'); // 使用 '/tmp' 文件路径生成信号量的键值
    if (sem_key == -1)
    {
        perror("ftok sem_key error");
        return -1;
    }

    // 获取信号量集
    semid = semget(sem_key, 2, IPC_CREAT | 0664);
    if (semid == -1)
    {
        perror("semget error");
        return -1;
    }

    // 初始化信号量
    // 信号量 0：控制写入，初始值 1
    // 信号量 1：控制读入，初始值 0
    semctl(semid, 0, SETVAL, 1); // 信号量 0：控制写入，初始值 1
    semctl(semid, 1, SETVAL, 0); // 信号量 1：控制读取，初始值 0

    // 获取共享内存
    shmid = shmget(shm_key, BLOCK_SIZE, IPC_CREAT | 0664);
    if (shmid == -1)
    {
        perror("shmget error");
        return -1;
    }

    // 将共享内存关联到当前进程
    *buffer = (char *)shmat(shmid, NULL, 0);
    if (*buffer == (void *)-1)
    {
        perror("shmat error");
        return -1;
    }

    return semid;
}