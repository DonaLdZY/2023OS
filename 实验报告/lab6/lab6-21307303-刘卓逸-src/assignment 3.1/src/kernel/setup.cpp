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

const int N=5;

Semaphore chopstick[5];

template<int T>
void philosopher(void *arg)
{
	int id=T;
	//int id=4;
	int left=id;
	int right=(id+1)%N;
	while (1){
		chopstick[left].P();
		printf("> %d takes chopstick %d\n",id,left);
		chopstick[right].P();
		printf("> %d takes chopstick %d\n",id,right);
		
		printf("%d is eating\n",id);
		int delay=0x3fffffff;
		while (delay>0) 
			--delay;
			
		chopstick[left].V();
		printf("> %d puts down chopstick %d\n",id,left);
		chopstick[right].V();
		printf("> %d puts down chopstick %d\n",id,right);
		
		delay=0x3fffffff;
		while (delay>0) 
			--delay;
		
	}
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
    for (int i = 0; i < 5; ++i)
    {
    	chopstick[i].initialize(1);
    }
    programManager.executeThread(philosopher<0>, nullptr, "p0", 1);
    programManager.executeThread(philosopher<1>, nullptr, "p1", 1);
    programManager.executeThread(philosopher<2>, nullptr, "p2", 1);
    programManager.executeThread(philosopher<3>, nullptr, "p3", 1);
    programManager.executeThread(philosopher<4>, nullptr, "p4", 1);
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
