
// timer/trap_handler.c

#include "os.h"

#include "timer.h"

#include "platform.h"
#include "riscv.h"


extern uint32_t     _tick;                 // 当前 tick 计数器
extern struct timer timer_list[MAX_TIMER]; // 定义计时器列表


// 定时器中断处理函数
void
timer_interrupt_handler()
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Tick: %d\n", ++_tick);

    _timer_checkout();           // 检查定时器列表，调用超时的计时器回调函数，并删除它们
    _timer_load(TIMER_INTERVAL); // 重新加载下一个定时器中断的时间间隔

    schedule();
}
