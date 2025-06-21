
// tasks.c

#include "mygos.h"
#include "utils.h"


// #define USE_LOOP

void
task_count30()
{
    print_task_start(task_get_current_context_id(), (reg_t)task_count30, "Task Count 30");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 30)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 30: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            break;
#endif
        }
        else printf("Task Count 30: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }

    print_task_end(task_get_current_context_id(), "Task Count 30");
}

void
task_count50()
{
    print_task_start(task_get_current_context_id(), (reg_t)task_count50, "Task Count 50");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 50)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 50: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            break;
#endif
        }
        else printf("Task Count 50: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }

    print_task_end(task_get_current_context_id(), "Task Count 50");
}

void
task_count70()
{
    print_task_start(task_get_current_context_id(), (reg_t)task_count70, "Task Count 70");

    int loop_count = 0;

    while(1)
    {
        if(loop_count >= 70)
        {
#ifdef USE_LOOP
            uart_puts("Task Count 70: Resetting loop count to 0!\n");
            loop_count = 0;
#else
            break;
#endif
        }
        else printf("Task Count 70: Loop %d\n", ++loop_count);

        task_delay(DELAY);
    }

    print_task_end(task_get_current_context_id(), "Task Count 70");
}
