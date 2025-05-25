//
// Created by katherinesolar on 25-5-23.
//

#ifndef KMEMORY_H
#define KMEMORY_H
#include <typedefs.h>

#include "sbi_console.h"
#include "sbi_init.h"
#define KERNEL_ADDRESS 0x80000000
struct heap_block {
	uint64_t size;
	struct heap_block *next;
	struct heap_block *prev;
	uint8_t is_free;
	void *data;
};
extern struct heap_block *heap_head;

void *kmalloc(uint64_t size);
void kfree(void *ptr);
#endif //KMEMORY_H
