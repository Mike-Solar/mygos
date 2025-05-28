
// tasks.c

#include "os.h"
#include "utils.h"


void
user_task0(int id)
{
    static int sum = 0;

    print_create_task(id, (reg_t)user_task0, "Task 0");
    while(1)
    {
        printf("Task 0: sum = %d\n", sum);
        if(sum > 25)
        {
            uart_puts("Task 0: sum exceeded 25, reset to 0\n");
            sum = 0;
        }
        else sum++;

        task_delay(DELAY);
        // task_yield();
    }
}

void
user_task1(int id)
{
    static int product = 1;

    print_create_task(id, (reg_t)user_task1, "Task 1");
    while(1)
    {
        printf("Task 1: product = %d\n", product);
        if(product > 1000)
        {
            uart_puts("Task 1: product exceeded 1000, reset to 1\n");
            product = 1;
        }
        else product *= 2;

        task_delay(DELAY);
        // task_yield();
    }
}

#define USE_LOCK
void
user_task_lock(int id)
{
    print_create_task(id, (reg_t)user_task_lock, "Task Lock");
    while(1)
    {
#ifdef USE_LOCK
        spin_lock();
#endif
        uart_puts("Task Lock: Begin ... \n");
        for(int i = 0; i < 5; i++)
        {
            uart_puts("Task Lock: Running... \n");
            task_delay(DELAY * 5);
        }
        uart_puts("Task Lock: End ... \n");
#ifdef USE_LOCK
        spin_unlock();
#endif
    }
}
