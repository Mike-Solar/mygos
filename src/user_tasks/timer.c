
// timer.c

#include "mygos.h"

#include "riscv.h"
#include "utils.h"


void
on_shot()
{
    printf("进入 on_shot\n");
    for(int i = 0; i < 10; i++)
    {
        uart_puts("Timer Shot: Hello from on_shot!\n");
        task_delay(DELAY * 10);
    }
}

// 测试计时器
void
task_timer()
{
    print_create_task(r_mscratch(), (reg_t)task_timer, "Task Timer");

    timer_create(on_shot, 0, 10); // 10 ticks 后触发一次

    print_delete_task(r_mscratch(), "Task Timer");
}
