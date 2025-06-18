
// timer.c


#include "timer.h"

#include "platform.h"
#include "riscv.h"


uint32_t     _tick = 0;             // 当前 tick 计数器
struct timer timer_list[MAX_TIMER]; // 定义计时器列表


// 加载下一个定时器中断的时间间隔（以计时周期为单位）
// 每个 CPU 都有一个独立的定时器中断源
inline void
_timer_load(uint32_t interval)
{
    *(uint64_t*)CLINT_MTIMECMP(r_mhartid()) = *(uint64_t*)CLINT_MTIME + interval;
}

// 检查定时器列表，调用超时的计时器回调函数，并删除它们
inline void
_timer_checkout()
{
    timer_ptr t = &(timer_list[0]);
    for(uint32_t i = 0; i < MAX_TIMER; i++, t++)
    {
        if(0 != t->func && _tick >= t->timeout_tick) // 保证该计时器项被使用，且当前 tick 大于等于超时时间
        {
            // 调用回调函数
            // 删除该计时器
            t->func(t->arg);
            t->func = 0;
            t->arg  = 0;
        }
    }
}

// 初始化定时器
void
timer_init()
{
    // 初始化计时器列表
    timer_ptr timer = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        timer->func = 0; // 使用 func 来标记该项是否被使用
        timer->arg  = 0;
        timer++;
    }

    // 在复位时，mtime 被清零，但 mtimecmp 寄存器不会被重置。
    // 因此我们需要手动初始化 mtimecmp。
    _timer_load(TIMER_INTERVAL);

    // 启用机器模式下的定时器中断
    w_mie(r_mie() | MIE_MTIE);

    // 启用机器模式下的全局中断
    // w_mstatus(r_mstatus() | MSTATUS_MIE);
}

// 创建一个新的定时器
timer_ptr
timer_create(void (*callback)(void*), void* arg, uint32_t timeout)
{
    // 参数检查：处理函数和超时时间不能为空
    if(!callback || !timeout) return nullptr;

    spin_lock(); // 使用锁来保护多个任务之间共享的 timer_list

    timer_ptr t = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        if(0 == t->func) break;
        t++;
    }

    if(0 != t->func) t = nullptr; // 如果没有可用的计时器，则返回 NULL
    else
    {
        t->func         = callback;        // 设置回调函数
        t->arg          = arg;             // 设置回调函数参数
        t->timeout_tick = _tick + timeout; // 设置超时时间（当前 tick + 超时值）
    }

    spin_unlock();

    return t;
}

void
timer_delete(timer_ptr timer)
{
    spin_lock(); // 使用锁来保护 timer_list

    timer_ptr t = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        if(t == timer)
        {
            t->func = 0;
            t->arg  = 0;
            break;
        }
        t++;
    }

    spin_unlock();
}
