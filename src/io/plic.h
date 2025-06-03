//
// Created by mikesolar on 25-6-3.
//

#ifndef PLIC_H
#define PLIC_H
#include <typedefs.h>

uint32_t plic_claim();
void plic_complete(uint32_t plic_id);

void plic_init();
#endif //PLIC_H
