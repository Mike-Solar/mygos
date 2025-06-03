
// user.c

#include "os.h"
#include "utils.h"


// tasks
extern void task_count30();
extern void task_count50();
extern void task_count70();

extern void user_task0();
extern void user_task1();
extern void user_task_lock();

extern void task_timer();

extern void page_test();


void
os_main(void)
{
    task_create(task_count30);
    task_create(task_count70);
    task_create(task_count50);


    // task_create(task_timer);
    // task_create(user_task_lock);
    // task_create(task_count30);
    // task_create(task_count50);
    // task_create(task_count70);

    // task_create(page_test);
}
