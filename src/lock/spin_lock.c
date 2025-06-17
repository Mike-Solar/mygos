
// lock.c

#include "os.h"

#include "platform.h"
#include "riscv.h"
extern void _acquire_spin_lock(volatile int *lock);
extern void _release_spin_lock(volatile int *lock);

// 直接禁用中断
// 通过修改 mstatus 寄存器的 MIE 位来实现
void spin_lock(struct spin_lock_t *lock)
{
    //之前对任何内存的读和写全部完成后才进行下面的
    _acquire_spin_lock(&lock->is_locked);
    w_mstatus(r_mstatus() & ~MSTATUS_MIE);
    asm volatile ("fence rw,rw\n"::);
    lock->is_locked = true;

}

// 恢复中断
void spin_unlock(struct spin_lock_t *lock)
{
    _release_spin_lock(&lock->is_locked);
    w_mstatus(r_mstatus() | MSTATUS_MIE);
    asm volatile ("fence rw,rw\n"::);
}
void spin_init(struct spin_lock_t *lock)
{
    lock->is_locked = false;
}
