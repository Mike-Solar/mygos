
// user.c

#include "os.h"
#include "utils.h"


// tasks
extern void task_count30(int id);
extern void task_count50(int id);
extern void task_count70(int id);

extern void user_task0(int id);
extern void user_task1(int id);
extern void user_task_lock(int id);

extern void task_timer(int id);


/* NOTICE: DON'T LOOP INFINITELY IN main() */
void
os_main(void)
{
    // task_create(user_task0);
    // task_create(user_task1);
    // task_create(user_task2);

    task_create(task_timer); // 创建计时器任务
    task_create(user_task_lock);
    task_create(task_count30);
    task_create(task_count50);
    task_create(task_count70);
}
