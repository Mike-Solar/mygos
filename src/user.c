
// user.c

#include "os.h"

#define DELAY 1000

void
user_task0(void)
{
    static int sum = 0;

    uart_puts("Task 0: Created!\n");
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
        task_yield();
    }
}

void
user_task1(void)
{
    static int product = 1;

    uart_puts("Task 1: Created!\n");
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
        task_yield();
    }
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void
os_main(void)
{
    task_create(user_task0);
    task_create(user_task1);
}
