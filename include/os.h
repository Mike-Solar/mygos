
// os.h

#ifndef __OS_H__
#define __OS_H__

#include "io.h"
#include "task.h"


/* memory management */
extern void* page_alloc(int npages);
extern void  page_free(void* p);


/* lock */
extern int spin_lock(void);
extern int spin_unlock(void);

/* software timer */
extern struct timer* timer_create(void (*handler)(void* arg), void* arg, uint32_t timeout);
extern void          timer_delete(struct timer* timer);


// 计时器
struct timer
{
    void (*func)(void*);   // 回调函数
    void*    arg;          // 回调函数参数
    uint32_t timeout_tick; // 超时时间（以 tick 为单位）
};


#endif /* __OS_H__ */
