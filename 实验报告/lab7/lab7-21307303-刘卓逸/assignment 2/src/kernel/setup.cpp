#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    // stdio.moveCursor(0);
    
    // 定义你自己分配想要分配的页数来模拟你实现的算法正确与否，这里我们提供了一个样例，你也可以自行修改：
    char *pages_0 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 128);
    printf("Allocated 128 pages for pages_0, starting at %d.\n", pages_0);

    char *pages_1 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 64);
    printf("Allocated 64 pages for pages_1, starting at %d.\n", pages_1);

    char *pages_2 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 16);
    printf("Allocated 16 pages for pages_2, starting at %d.\n", pages_2);

    char *pages_3 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 8);
    printf("Allocated 8 pages for pages_3, starting at %d.\n", pages_3);

    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_0), 128);
    printf("Released 128 pages for pages_0.\n");

    memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, int(pages_2), 16);
    printf("Released 16 pages for pages_2.\n");

    char *pages_4 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 16);
    printf("Allocated 16 pages for pages_4, starting at %d.\n", pages_4);

    char *pages_5 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 129);
    printf("Allocated 129 pages for pages_5, starting at %d.\n", pages_5);

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
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
