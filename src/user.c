
// user.c

#include "os.h"

#define DELAY 1000

extern void trap_test();

void
user_task0t(void)
{
    uart_puts("Task 0: Created!\n");

    task_yield();
    uart_puts("Task 0: I'm back!\n");
    while(1)
    {
        uart_puts("Task 0: Running...\n");
        task_delay(DELAY);
    }
}

void
user_task1t(void)
{
    uart_puts("Task 1: Created!\n");
    while(1)
    {
        uart_puts("Task 1: Running...\n");
        task_delay(DELAY);
    }
}


void
user_task0(int id)
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
        // task_yield();
    }
}

void
user_task1(int id)
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
        // task_yield();
    }
}

void
user_task2(int id)
{
    uart_puts("Task 2: Created!\n");
    uart_puts("Task 2: This task only loops 30 times.\n");

    // trap_test();

    for(int i = 0; i < 30; i++)
    {
        printf("Task 2: Loop %d\n", i);
        task_delay(DELAY);
        // task_yield();
    }
    uart_puts("Task 2: Finished!\n");

    task_delete(id); // 删除任务 2
}

void
task_count30(int id)
{
    uart_puts("Task Count 30: Created!\n");
    for(int i = 0; i < 30; i++)
    {
        printf("Task Count 30: Loop %d\n", i);
        task_delay(DELAY);
    }
    uart_puts("Task Count 30: Finished!\n");

    task_delete(id); // 删除任务 Count 30
}

void
task_count50(int id)
{
    uart_puts("Task Count 50: Created!\n");
    for(int i = 0; i < 50; i++)
    {
        printf("Task Count 50: Loop %d\n", i);
        task_delay(DELAY);
    }
    uart_puts("Task Count 50: Finished!\n");

    task_delete(id); // 删除任务 Count 50
}

void
task_count70(int id)
{
    uart_puts("Task Count 70: Created!\n");
    for(int i = 0; i < 70; i++)
    {
        printf("Task Count 70: Loop %d\n", i);
        task_delay(DELAY);
    }
    uart_puts("Task Count 70: Finished!\n");

    task_delete(id); // 删除任务 Count 70
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void
os_main(void)
{
    // task_create(user_task0);
    // task_create(user_task1);
    // task_create(user_task2);

    task_create(task_count30);
    task_create(task_count50);
    task_create(task_count70);
}
