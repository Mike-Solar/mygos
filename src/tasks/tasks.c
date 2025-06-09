
// tasks.c

#include "os.h"
#include "riscv.h"
#include "utils.h"
#include "typedefs.h"
#include "page.h"

void
user_task0()
{
    static int sum = 0;

    print_create_task(r_mscratch(), (reg_t)user_task0, "Task 0");
    while(1)
    {
        printk("Task 0: sum = %d\n", sum);
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
user_task1()
{
    static int product = 1;

    print_create_task(r_mscratch(), (reg_t)user_task1, "Task 1");
    while(1)
    {
        printk("Task 1: product = %d\n", product);
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
user_task_lock()
{
    print_create_task(r_mscratch(), (reg_t)user_task_lock, "Task Lock");
    while(1)
    {
        struct spin_lock_t lock;
        spin_init(&lock);
#ifdef USE_LOCK
        spin_lock(&lock);
#endif
        uart_puts("Task Lock: Begin ... \n");
        for(int i = 0; i < 5; i++)
        {
            uart_puts("Task Lock: Running... \n");
            task_delay(DELAY * 5);
        }
        uart_puts("Task Lock: End ... \n");
#ifdef USE_LOCK
        spin_unlock(&lock);
#endif
    }
}


void
page_test()
{
    print_create_task(r_mscratch(), (reg_t)page_test, "Page Test");

    void* p1 = alloc_pages(2);
    printk("p1 = %p\n", p1);
    free_pages(p1);

    void* p2 = alloc_pages(7);
    printk("p2 = %p\n", p2);

    void* p3 = alloc_pages(4);
    printk("p3 = %p\n", p3);
    free_pages(p3);

    free_pages(p2);

    printk("\n");

    print_delete_task(r_mscratch(), "Page Test");
}
