
// timer.c

#include "os.h"


extern void schedule(void);


// interval ~= 1s
// 时间间隔约等于 1 秒
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

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

// 定时器中断处理函数
void
timer_handler()
{
    _tick++;
    printf("tick: %d\n", _tick);

    timer_load(TIMER_INTERVAL);

    schedule(); // 抢占式多任务跳转
}
