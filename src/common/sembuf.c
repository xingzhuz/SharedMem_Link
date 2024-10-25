#include "process.h"

// P 操作：用于占用资源，将信号量值减 1，如果信号量值为 0，则阻塞等待
void P(int semid, int semnum)
{
    struct sembuf sop;
    sop.sem_num = semnum;  // 指定信号量编号
    sop.sem_op = -1;       // P 操作，将信号量值减 1
    sop.sem_flg = 0;       // 默认标志，阻塞等待
    semop(semid, &sop, 1); // 执行信号量操作
}

// V 操作：用于释放资源，将信号量值加 1，通知其他进程资源可用
void V(int semid, int semnum)
{
    struct sembuf sop;
    sop.sem_num = semnum;  // 指定信号量编号
    sop.sem_op = 1;        // V 操作，将信号量值加 1
    sop.sem_flg = 0;       // 默认标志
    semop(semid, &sop, 1); // 执行信号量操作
}