
// timer.h

#pragma once

#include "types.h"


// 计时器
struct timer
{
    void (*func)(void*);   // 回调函数
    void*    arg;          // 回调函数参数
    uint32_t timeout_tick; // 超时时间（以 tick 为单位）
};


typedef struct timer* timer_ptr; // 计时器指针类型


// 时间间隔约等于 1 秒
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

#define MAX_TIMER 10
