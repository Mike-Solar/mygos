
// trap.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


void
trap_init()
{
    // set the trap-vector base-address for machine-mode
    w_mtvec((reg_t)trap_vector);
}
