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
#define PHYS_MEM_SIZE   (8192L * 1024L * 1024L)  // 物理内存总大小（如 128MB）
#define PAGE_COUNT      (PHYS_MEM_SIZE / PAGE_SIZE)
#define MAX_ORDER       6
#define PAGE_SHIFT      12      // 4KB页（2^12 = 4096）
#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define PFN_PHYS(pfn)   ((uintptr_t)(pfn) << PAGE_SHIFT)
#define PHYS_PFN(phys)  ((uintptr_t)(phys) >> PAGE_SHIFT)
#define  KERNEL_SPACE ((uint64_t)__heap_end - KERNEL_PHYS_ADDR);

extern const uintptr_t KERNEL_PHYS_ADDR;
extern const uintptr_t KERNEL_VIRT_ADDR;
#define PHYS_TO_VIRT(phys) ((void*)((uint64_t)(phys) + KERNEL_VIRT_ADDR))

extern const uintptr_t OPEN_SBI_PHYS_ADDR;
extern void *_kernel_end;
extern uintptr_t KERNEL_PHYS_END;
extern void _start(void);

typedef struct pte {
	uint64_t V:1;
	uint64_t R:1;
	uint64_t W:1;
	uint64_t X:1;
	uint64_t U:1;
	uint64_t G:1;
	uint64_t A:1;
	uint64_t D:1;
	uint64_t RSW:2;
	uint64_t PPN_0:9;
	uint64_t PPN_1:9;
	uint64_t PPN_2:26;
	uint64_t reserved: 10;
} pte_t;
typedef struct stap {
	uint64_t PPN_0:9;
	uint64_t PPN_1:9;
	uint64_t PPN_2:26;
	uint64_t ASID: 12;
	uint64_t MODE:4;
}stap_t;
typedef struct phys_addr {
	uint64_t offset:12;
	uint64_t PPN_0:9;
	uint64_t PPN_1:9;
	uint64_t PPN_2:26;
	uint64_t reserved:8;
} phys_addr_t;
typedef struct virt_addr {
	uint64_t offset:12;
	uint64_t VPN_0:9;
	uint64_t VPN_1:9;
	uint64_t VPN_2:9;
	uint64_t reserved:25;
} virt_addr_t;
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
extern struct zone memory_zone;
stap_t early_paging_init(void); //设置早期分页
void early_enable_paging(stap_t stap); //启用分页

void enable_paging(pte_t* pagetable); //启用分页
void map_kernel_identity(pte_t* pagetable);
void phys_mem_init();
void map_pages(pte_t* pt, uint64_t va, uint64_t pa, uint64_t size, int perm);

pte_t* walk(pte_t* pagetable, uint64_t va, int alloc);
void *alloc_pages(uint8_t n);
void free_pages(void* pa);

static inline void zero(void *dst, int len) {\
	uint8_t *dst8 = (uint8_t*)dst;
	for (int i = 0; i < len; i++) {
		dst8[i] = 0;
	}
}
extern pte_t early_pt_2[PAGE_SIZE / sizeof(pte_t)] __attribute__((section(".early_page")));
extern pte_t early_pt_1[PAGE_SIZE / sizeof(pte_t)] __attribute__((section(".early_page")));
extern pte_t early_pt_0[PAGE_SIZE / sizeof(pte_t)][PAGE_SIZE / sizeof(pte_t)] __attribute__((section(".early_page")));
extern void * __heap_end;
#define  KERNEL_SPACE ((uint64_t)__heap_end - KERNEL_PHYS_ADDR)
#endif //PAGE_H
