
// user.c

#include "os.h"
#include "utils.h"


// tasks
extern void task_count30(uint32_t id);
extern void task_count50(uint32_t id);
extern void task_count70(uint32_t id);

extern void user_task0(uint32_t id);
extern void user_task1(uint32_t id);
extern void user_task_lock(uint32_t id);

extern void task_timer(uint32_t id);

extern void page_test(uint32_t id);


void
os_main(void)
{
    task_create(task_timer);
    task_create(user_task_lock);
    task_create(task_count30);
    task_create(task_count50);
    task_create(task_count70);

    task_create(page_test);
}
