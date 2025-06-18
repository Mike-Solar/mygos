
// mygos.h

#ifndef __MYGOS_H__
#define __MYGOS_H__


#include "types.h"


/* 输入输出处理 */

extern char uart_getc();                                        // 从串口接收一个字符（阻塞，等待接收缓冲区有数据）
extern void uart_putc(char ch);                                 // 输出一个字符到串口（阻塞，等待发送缓冲区空）
extern void uart_puts(char* s);                                 // 输出字符串到串口（逐字符发送）

extern void     panic(char* s);                                 // 输出错误信息并进入死循环
extern uint32_t printf(const char* s, ...);                     // 格式化输出到串口，类似于 printf
extern uint32_t os_message(const char* s, ...);                 // 打印系统信息，类似于 printf，但用于系统消息
extern void     get_input(char* input_buffer, int buffer_size); // 获取用户输入，存入指定缓冲区


/* 锁 */

extern void spin_lock();
extern void spin_unlock();


/* 任务调度 */

typedef struct task_context* task_context_ptr;

extern void    task_yield();                                   // 任务主动让出 CPU，允许其他任务运行
extern int32_t task_create(void (*task)());                    // 创建一个新任务，传入任务函数指针
extern void    task_delete(task_context_ptr task_context_ptr); // 删除指定任务
extern void    task_delete_current();                          // 删除当前任务
extern void    task_delay(volatile int count);                 // 延迟执行，消耗 CPU 时间

extern uint32_t         task_get_count();                      // 获取当前活动任务数
extern uint32_t         task_get_current_context_id();         // 获取当前任务编号
extern task_context_ptr task_get_current_context_ptr();        // 获取当前任务的上下文指针


/* 内存管理 */

extern void* page_alloc(int npages); // 分配一个内存块，由连续的物理页组成
extern void  page_free(void* p);     // 释放一个内存块，回收连续的物理页


/* 定时器 */

extern timer_ptr timer_create(void (*callback)(void*), void* arg, uint32_t timeout); // 创建一个新的定时器
extern void      timer_delete(timer_ptr timer);                                      // 删除指定定时器


#endif // __MYGOS_H__
