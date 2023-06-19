#include "syscall.h"
#include "interrupt.h"
#include "stdlib.h"
#include "asm_utils.h"
#include "os_modules.h"
#include "stdio.h"
#include "thread.h"
int system_call_table[MAX_SYSTEM_CALL];

SystemService::SystemService() {
    initialize();
}

void SystemService::initialize()
{
    memset((char *)system_call_table, 0, sizeof(int) * MAX_SYSTEM_CALL);
    // 代码段选择子默认是DPL=0的平坦模式代码段选择子，DPL=3，否则用户态程序无法使用该中断描述符
    interruptManager.setInterruptDescriptor(0x80, (uint32)asm_system_call_handler, 3);
}

bool SystemService::setSystemCall(int index, int function)
{
    system_call_table[index] = function;
    return true;
}

int syscall_0(int first, int second, int third, int forth, int fifth)
{
    printf("systerm call 0: %d, %d, %d, %d, %d\n",
           first, second, third, forth, fifth);
    return first + second + third + forth + fifth;
}


int write(const char *str) {
    return asm_system_call(1, (int)str);
}

int syscall_write(const char *str) {
    return stdio.print(str);
}

int fork() {
    return asm_system_call(2);
}

int syscall_fork() {
    return programManager.fork();
}

void exit(int ret) {
    asm_system_call(3, ret);
}

void syscall_exit(int ret) {
    programManager.exit(ret);
}

int wait(int *retval) {
    return asm_system_call(4, (int)retval);
}

int syscall_wait(int *retval) {
    return programManager.wait(retval);
}

void move_cursor(int i, int j) {
    asm_system_call(5, i, j);
}
void syscall_move_cursor(int i, int j) {
    stdio.moveCursor(i, j);
}

extern ByteMemoryManager kernelByteMemoryManager;
void *malloc(int size){
    return (void*)asm_system_call(6,size);
}
void *syscall_malloc(int size)
{
    PCB *pcb = programManager.running;
    if (pcb->pageDirectoryAddress)
    {
        printf("user allocate\n");
        // 每一个进程有自己的ByteMemoryManager
        return pcb->byteMemoryManager.allocate(size);
    }
    else
    {
        printf("kernel allocate\n");
        // 所有内核线程共享一个ByteMemoryManager
        return kernelByteMemoryManager.allocate(size);
    }
}

void free(void *address){
    asm_system_call(7,(int)address);
}
void syscall_free(void *address)
{
    PCB *pcb = programManager.running;
    if (pcb->pageDirectoryAddress)
    {
        pcb->byteMemoryManager.release(address);
    }
    else
    {
        kernelByteMemoryManager.release(address);
    }
}

extern ProgramManager programManager;
int executeThread(ThreadFunction function, void *parameter, const char *name, int priority){
    return asm_system_call(8,(int)function,(int)parameter,(int)name,(int)priority);
}
int syscall_executeThread(ThreadFunction function, void *parameter, const char *name, int priority){
    return programManager.executeThread(function,parameter,name,priority);
}   
int executeProcess(const char *filename, int priority){
    return asm_system_call(9,(int)filename,(int)priority);
}
int syscall_executeProcess(const char *filename, int priority){
    return programManager.executeProcess(filename,priority);
}

