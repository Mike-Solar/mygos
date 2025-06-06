//
// Created by mikesolar on 25-6-6.
//
.section .text
.global _acquire_spin_lock
.global _release_spin_lock

_acquire_spin_lock:
    li t1, 1
retry:
    amoswap.w.aq t0, t1, (a0)
    bnez t0, retry
    fence r, rw
    ret
_release_spin_lock:
    fence rw, w
    sw zero, (a0)
    fence rw, rw
    ret
