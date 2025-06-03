//
// Created by katherinesolar on 25-5-27.
//

#ifndef PAGE_H
#define PAGE_H
#include <typedefs.h>

#include "list.h"
#define PTE_V  (1 << 0) // 有效位
#define PTE_R  (1 << 1) // 可读
#define PTE_W  (1 << 2) // 可写
#define PTE_X  (1 << 3) // 可执行
// 从虚拟地址提取各级索引（Sv39）
#define PX(level, va) (((va) >> (12 + 9 * (level))) & 0x1FF)
#define PAGE_SIZE 512
// 物理地址 <-> 页表项转换
#define PA2PTE(pa) ((((uint64_t)(pa)) >> 12) << 10)
#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PAGE_SIZE       4096       // 页大小（4KB）
#define PHYS_MEM_SIZE   (8192L * 1024L * 1024L * 1024L)  // 物理内存总大小（如 128MB）
#define PAGE_COUNT      (PHYS_MEM_SIZE / PAGE_SIZE)
#define MAX_ORDER       6
#define PAGE_SHIFT      12      // 4KB页（2^12 = 4096）
#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define PFN_PHYS(pfn)   ((uintptr_t)(pfn) << PAGE_SHIFT)
#define PHYS_PFN(phys)  ((uintptr_t)(phys) >> PAGE_SHIFT)
#define PHYS_TO_VIRT(phys, delta) ((void*)((uint64_t)(phys) + delta))

struct page {
	bool is_used;
	uint8_t order;                     // 内存块大小是2的几次方
	struct list_head list;         // 空闲链表节点（嵌入到free_area的链表中）
	uint64_t pfn;                  // 页帧号（Page Frame Number）
};
extern uint64_t page_count;
struct free_area {
	struct list_head free_list;
	uint64_t nr_free;
};
struct zone {
	struct free_area free_area[  MAX_ORDER+1];
};
extern struct zone memory_zone_phys;
extern struct zone *memory_zone;
void enable_paging(uint64_t* pagetable);
void map_kernel_identity(uint64_t* pagetable);
void phys_mem_init();
void map_pages(uint64_t* pt, uint64_t va, uint64_t pa, uint64_t size, int perm);
uint64_t* walk(uint64_t* pagetable, uint64_t va, int alloc);
void* page_alloc();
void page_free(void* pa);

#endif //PAGE_H
