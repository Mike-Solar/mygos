
// tasks.c

#include "os.h"
#include "utils.h"


// #define USE_LOOP

void
task_count30(int id)
{
    print_create_task(id, (reg_t)task_count30, "Task Count 30");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 30)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 30: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            print_delete_task(id, "Task Count 30");
            task_delete(id); // 删除任务 Count 30
            break;
#endif
        }
        else printf("Task Count 30: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }
}

void
task_count50(int id)
{
    print_create_task(id, (reg_t)task_count50, "Task Count 50");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 50)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 50: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            print_delete_task(id, "Task Count 50");
            task_delete(id); // 删除任务 Count 50
            break;
#endif
        }
        else printf("Task Count 50: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }
}

void
task_count70(int id)
{
    print_create_task(id, (reg_t)task_count70, "Task Count 70");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 70)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 70: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            print_delete_task(id, "Task Count 70");
            task_delete(id); // 删除任务 Count 70
            break;
#endif
        }
        else printf("Task Count 70: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }
}
