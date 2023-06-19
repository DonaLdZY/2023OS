#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"
#include "bytememory.h"
#include "syscall.h"
#include "tss.h"
#include "shell.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;
// 元内存管理器
ByteMemoryManager kernelByteMemoryManager;
// 系统调用
SystemService systemService;
// Task State Segment
TSS tss;

void delay(unsigned int t){
    while (t--);
}

void thread1(void *arg)
{
    void *addr = malloc(20);
    printf("thread1 get addr0(20) %x\n", addr);
    delay(0x3fffffff);
    void *addr1 = malloc(114514);
    printf("thread1 get addr1(114514) %x\n",addr1);
    void *addr2 = malloc(30);
    printf("thread1 get addr2(30) %x\n",addr2);
    free(addr);
    printf("thread1 free addr0(20) %x\n",addr);
    void *addr3 = malloc(20);
    printf("thread1 get addr3(20) %x\n",addr3);
    free(addr1);
    free(addr2);
    free(addr3);
}
void thread2(void *arg)
{
    void *addr = malloc(20);
    printf("thread2 get addr(20) %x\n",addr);
    delay(0x3fffffff);
    free(addr);
}

void iprocess()
{
    printf("start process\n");
    void *addr = malloc(20);
    printf("iprocess get addr0(20) %x\n", addr);
    void *addr2 = malloc(114514);
    printf("iprocess get addr1(114514) %x\n", addr2);
    asm_halt();
}

void first_thread(void *arg)
{

    printf("start first_thread\n");
    executeThread(thread1, nullptr, "t1", 1);
    executeThread(thread2, nullptr, "t2", 1);
    delay(0xffffffff);
    executeProcess((const char *)iprocess, 1);
    executeProcess((const char *)iprocess, 1);
    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 初始化系统调用
    systemService.initialize();
    // 设置0号系统调用
    systemService.setSystemCall(0, (int)syscall_0);
    // 设置1号系统调用
    systemService.setSystemCall(1, (int)syscall_write);
    // 设置2号系统调用
    systemService.setSystemCall(2, (int)syscall_fork);
    // 设置3号系统调用
    systemService.setSystemCall(3, (int)syscall_exit);
    // 设置4号系统调用
    systemService.setSystemCall(4, (int)syscall_wait);
    // 设置5号系统调用
    systemService.setSystemCall(5, (int)syscall_move_cursor);
    // 设置6号系统调用
    systemService.setSystemCall(6, (int)syscall_malloc);
    // 设置7号系统调用
    systemService.setSystemCall(7, (int)syscall_free);
    // 设置8号系统调用
    systemService.setSystemCall(8, (int)syscall_executeThread);
    // 设置9号系统调用
    systemService.setSystemCall(9, (int)syscall_executeProcess);
    // 内存管理器
    memoryManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = ProgramStatus::RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
