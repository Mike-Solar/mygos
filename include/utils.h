
// utils.h

#ifndef __UTILS_H__
#define __UTILS_H__

#include "os.h"

#define DELAY 1000

static inline void
print_create_task(reg_t id, reg_t pc, const char* name)
{
    printk("====== Task Created: %p (%s) ======\n", id, name);
    printk("PC = 0x%08x\n", pc);
}

static inline void
print_delete_task(reg_t id, const char* name)
{
    printk("====== Task Deleted: %p (%s) ======\n", id, name);
}

#endif // __UTILS_H__
