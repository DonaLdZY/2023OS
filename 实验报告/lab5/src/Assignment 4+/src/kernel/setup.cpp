#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;



void task_6(void *arg){
	printf("start pid %d name \"%s\"\n", programManager.running->pid, programManager.running->name);
	for (int i=1;i<=6;i++){
		for (int j=1;j<=10000;j++){for (int k=1;k<=13000;k++);}
		printf("pid %d \"%s\" running %2d / %2d\n",programManager.running->pid,programManager.running->name,i,6);
	}
	printf(" pid %d \"%s\" finish! \n",programManager.running->pid,programManager.running->name);
	//programManager.executeThread(task_quick,nullptr,"veryquick",100);
	
}
void second_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World!\n", programManager.running->pid, programManager.running->name);
}
void first_thread(void *arg)
{
    // 第1个线程不可以返回
    printf("pid %d name \"%s\": Hello World! by liuzhy\n", programManager.running->pid, programManager.running->name);
    if (!programManager.running->pid)
    {
        //programManager.executeThread(task_6, nullptr, "task6_1", 1);
        //programManager.executeThread(task_6, nullptr, "task6_2", 2);
        //programManager.executeThread(task_6, nullptr, "task6_3", 3);
        
    }
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
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 10);
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
