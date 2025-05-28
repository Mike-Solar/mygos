
// user.c

#include "os.h"

#define DELAY 1000

extern void trap_test();

void
print_create_task(int id, reg_t pc, const char* name)
{
    printf("====== Task Created: %d (%s) ======\n", id, name);
    printf("PC = 0x%08x\n", pc);
}

void
print_delete_task(int id, const char* name)
{
    printf("====== Task Deleted: %d (%s) ======\n", id, name);
}


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


/* NOTICE: DON'T LOOP INFINITELY IN main() */
void
os_main(void)
{
    // task_create(user_task0);
    // task_create(user_task1);
    // task_create(user_task2);

    task_create(user_task_lock);
    task_create(task_count30);
    task_create(task_count50);
    task_create(task_count70);
}
