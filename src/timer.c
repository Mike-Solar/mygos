
// timer.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


// interval ~= 1s
// 时间间隔约等于 1 秒
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

// 定义计时器列表
#define MAX_TIMER 10
static struct timer timer_list[MAX_TIMER];


// load timer interval(in ticks) for next timer interrupt.
// 加载下一个定时器中断的时间间隔（以计时周期为单位）
void
timer_load(int interval)
{
    // each CPU has a separate source of timer interrupts.
    // 每个 CPU 都有一个独立的定时器中断源
    int id = r_mhartid();

    *(uint64_t*)CLINT_MTIMECMP(id) = *(uint64_t*)CLINT_MTIME + interval;
}

// 初始化定时器
void
timer_init()
{
    // 初始化计时器列表
    struct timer* t = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        t->func = 0; // 使用 .func 来标记该项是否被使用
        t->arg  = 0;
        t++;
    }

    // On reset, mtime is cleared to zero, but the mtimecmp registers are not reset.
    // So we have to init the mtimecmp manually.
    // 在复位时，mtime 被清零，但 mtimecmp 寄存器不会被重置。
    // 因此我们需要手动初始化 mtimecmp。
    timer_load(TIMER_INTERVAL);

    // enable machine-mode timer interrupts.
    // 启用机器模式下的定时器中断
    w_mie(r_mie() | MIE_MTIE);

    // enable machine-mode global interrupts.
    // 启用机器模式下的全局中断
    // w_mstatus(r_mstatus() | MSTATUS_MIE);
}

// 创建一个新的定时器
struct timer*
timer_create(void (*handler)(void*), void* arg, uint32_t timeout)
{
    /* TBD: params should be checked more, but now we just simplify this */
    // 参数检查：处理函数和超时时间不能为空
    if(0 == handler || 0 == timeout) return 0;

    /* use lock to protect the shared timer_list between multiple tasks */
    // 使用锁来保护多个任务之间共享的 timer_list
    spin_lock();

    struct timer* t = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        if(0 == t->func) break;
        t++;
    }

    if(0 != t->func) t = 0; // 如果没有可用的计时器，则返回 NULL
    else
    {
        t->func         = handler;
        t->arg          = arg;
        t->timeout_tick = _tick + timeout;
    }

    spin_unlock();

    return t;
}

void
timer_delete(struct timer* timer)
{
    spin_lock(); // 使用锁来保护 timer_list

    struct timer* t = &(timer_list[0]);
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

/* this routine should be called in interrupt context (interrupt is disabled) */
// 该函数应在中断上下文中调用（中断被禁用）
static inline void
timer_check()
{
    struct timer* t = &(timer_list[0]);
    for(int i = 0; i < MAX_TIMER; i++)
    {
        if(0 != t->func)                 // 保证该计时器项被使用
        {
            if(_tick >= t->timeout_tick) // 如果当前 tick 大于等于超时时间
            {
                t->func(t->arg);         // 调用回调函数

                /* once time, just delete it after timeout */
                t->func = 0;
                t->arg  = 0;

                break;
            }
        }
        t++;
    }
}

// 定时器中断处理函数
void
timer_handler()
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Tick: %d\n", ++_tick);

    timer_check();

    timer_load(TIMER_INTERVAL);

    schedule(); // 抢占式多任务跳转
}
