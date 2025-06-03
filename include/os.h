
// os.h

#pragma once

#include "task.h"
#include "timer.h"


// 任务调度器相关函数声明

extern void switch_to(task_context_ptr next_task);      // 切换到下一个任务的上下文

void    sched_init();                                   // 初始化调度器
void    schedule();                                     // 任务轮转调度
void    task_yield();                                   // 任务主动让出 CPU，允许其他任务运行
int32_t task_create(void (*task)());                    // 创建一个新任务，传入任务函数指针
void    task_delete(task_context_ptr task_context_ptr); // 删除指定任务
void    task_delete_current();                          // 删除当前任务
void    task_delay(volatile int count);                 // 延迟执行，消耗 CPU 时间

uint32_t         task_get_count();                      // 获取当前活动任务数
uint32_t         task_get_current();                    // 获取当前任务编号
task_context_ptr task_get_current_context();            // 获取当前任务的上下文指针


// io 相关函数声明

void     plic_init();                     // 初始化 PLIC（外部中断控制器），设置 UART0 的中断优先级和使能
uint32_t plic_claim();                    // 查询 PLIC，获取当前需要处理的中断源 ID（如果有）
void     plic_complete(uint32_t plic_id); // 完成中断处理，通知 PLIC 已经处理完指定的中断源 ID

void uart_init();                         // 初始化 UART，配置波特率和数据格式
void uart_putc(char ch);                  // 输出一个字符到串口（阻塞，等待发送缓冲区空）
void uart_puts(char* s);                  // 输出字符串到串口（逐字符发送）
char uart_getc();                         // 接收一个字符（阻塞，直到接收缓冲区有数据）

uint32_t printf(const char* s, ...);      // 格式化输出到串口，类似于 printf
void     panic(char* s);                  // 输出错误信息并进入死循环


// 异常/中断处理相关函数声明

void trap_init();                  // 初始化异常/中断处理

void task_interrupt_handler();     // 任务切换处理函数，处理机器模式软件中断
void external_interrupt_handler(); // 外部中断处理函数，处理来自 PLIC 的中断请求
void timer_interrupt_handler();    // 机器模式定时器中断处理函数，处理定时器中断


/* memory management */
extern void* page_alloc(int npages);
extern void  page_free(void* p);


/* lock */
extern int spin_lock(void);
extern int spin_unlock(void);


/* software timer */
timer_ptr timer_create(void (*callback)(void*), void* arg, uint32_t timeout);
void      timer_delete(timer_ptr timer);
void      timer_check();
void      timer_load(uint32_t interval);