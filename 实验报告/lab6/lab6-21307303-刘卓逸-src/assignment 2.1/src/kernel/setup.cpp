#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

Semaphore eatburger,makeburger;

int cheese_burger;

void a_cooker(void *arg)
{
    int delay=0;
    //makeburger.P();
    while (cheese_burger>=2);
    printf("> start making a burger,please wait...\n");
    delay = 0xaffffff;
    while (delay)
        --delay;
    if (cheese_burger>=2)
    	printf("* Finish making a new burger, but there are already 2 burgers!\n");
    else{
    cheese_burger++;
    printf("+ Finish making a new burger, there are %d burgers on the table\n",cheese_burger);
    }
    //makeburger.V();
}

void a_foodie(void *arg)
{
    while (cheese_burger<=0);
    //eatburger.P();
    printf("> start eatting a burger...\n");
    int delay = 0xfffffff;
    while (delay) 
    	--delay;
    	
    cheese_burger--;
    printf("- take a burger,there are %d burgers left\n",cheese_burger);
    //eatburger.V();
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    cheese_burger = 0;
    makeburger.initialize(1);
    eatburger.initialize(1);
    programManager.executeThread(a_cooker, nullptr, "third thread", 1);
    programManager.executeThread(a_cooker, nullptr, "fourth thread", 1);
    programManager.executeThread(a_cooker, nullptr, "fifth thread", 1);
    programManager.executeThread(a_cooker, nullptr, "sixth thread", 1);
    programManager.executeThread(a_foodie, nullptr, "second thread", 1);
    programManager.executeThread(a_foodie, nullptr, "second thread", 1);
    programManager.executeThread(a_foodie, nullptr, "seventh thread", 1);
    programManager.executeThread(a_foodie, nullptr, "eighth thread", 1);
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
