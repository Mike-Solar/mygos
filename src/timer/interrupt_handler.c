
// timer/interrupt_handler.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


extern uint32_t     _tick;                 // 当前 tick 计数器
extern struct timer timer_list[MAX_TIMER]; // 定义计时器列表


// 定时器中断处理函数
void
timer_handler()
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Tick: %d\n", ++_tick);

    timer_check();

    timer_load(TIMER_INTERVAL);

    schedule(); // 抢占式多任务跳转
}
