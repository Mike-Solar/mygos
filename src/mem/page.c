//
// Created by katherinesolar on 25-5-27.
//

#include "page.h"
#include "list.h"
#include <stddef.h>


#include "typedefs.h"
#include "kmath.h"
#define PAGE_SHIFT	12
uint64_t page_count=0;
// 物理页帧状态位图（每个位表示一页：0-空闲，1-已分配）
 struct page phys_page_array[PAGE_COUNT];
struct page *phys_page_array_phys_addr;
struct page *phys_page_array_virt_addr;

struct zone memory_zone_phys __attribute__((section(".buddy_meta")));
struct zone *memory_zone;
#define ALIGN_UP(x, align)  (((x) + ((align) - 1)) & ~((align) - 1))

void buddy_init_stage1(void) {
	// 使用物理地址初始化链表
	for (int i = 0; i <= MAX_ORDER; i++) {

		init_list_head(&memory_zone_phys.free_area[i].free_list);

		memory_zone_phys.free_area[i].nr_free=0;
	}
	memory_zone_phys.free_area[MAX_ORDER].nr_free=page_count/EXP_OF_2(MAX_ORDER);
	phys_page_array_phys_addr=phys_page_array;
}
/* 第二阶段初始化（启用分页后） */
void buddy_init_stage2(void) {

	phys_page_array_virt_addr=PHYS_TO_VIRT(phys_page_array_phys_addr);
	// 将物理地址转换为虚拟地址
	for (int i = 0; i <= MAX_ORDER; i++) {
		struct list_head *curr = memory_zone->free_area[i].free_list.next;
		while (curr != &memory_zone->free_area[i].free_list) {
			// Step 1: 将链表节点物理地址转为虚拟地址
			struct list_head *virt_node = PHYS_TO_VIRT(curr);
            
			// Step 2: 修正前后指针的地址
			virt_node->prev = PHYS_TO_VIRT(virt_node->prev);
			virt_node->next = PHYS_TO_VIRT(virt_node->next);
            
			curr = virt_node->next;
		}
		// 修正链表头指针
		memory_zone->free_area[i].free_list.next = PHYS_TO_VIRT(memory_zone->free_area[i].free_list.next);
		memory_zone->free_area[i].free_list.prev = PHYS_TO_VIRT(memory_zone->free_area[i].free_list.prev);
	}
}

void enable_paging(uint64_t* pagetable) {
	// 设置 satp 寄存器（Sv39 模式）
	uint64_t satp_value = ((uint64_t)pagetable >> 12) | (8ULL << 60); // MODE=8 (Sv39)
	asm volatile("csrw satp, %0" : : "r"(satp_value));
	asm volatile("sfence.vma"); // 刷新 TLB
}
// 建立内核恒等映射（虚拟地址 = 物理地址）
void map_kernel_identity(uint64_t* pagetable) {
	// 映射内核代码和数据（假设物理地址从 0x80000000 开始）
	map_pages(pagetable, 0x80000000, 0x80000000, 0x8200000, PTE_R | PTE_W | PTE_X);

	// 映射设备寄存器（如 UART 地址 0x10000000）
	map_pages(pagetable, 0x10000000, 0x10000000, PAGE_SIZE, PTE_R | PTE_W);
}

// 物理内存起始地址（由设备树或硬编码确定）
uintptr_t phys_mem_start = 0x80000000uL;
void phys_mem_init() {
	// 清零空闲列表
	phys_page_array[0].pfn = 0;
	init_list_head(&phys_page_array[0].list);
	phys_page_array[0].is_used = false;
	struct list_head *cur=&phys_page_array[0].list;
	for(int i=1;i<page_count;i++) {
		phys_page_array[i].pfn = i;
		list_add(&phys_page_array[i].list,cur);
		cur=&phys_page_array[i].list;
		phys_page_array[i].is_used = false;
		phys_page_array[i].order = 0;
	}
	// 标记已使用的页（如内核代码、保留区域）
	uintptr_t kernel_start = 0x80000000uL;    // 内核起始物理地址
	uintptr_t kernel_end   = 0x80200000uL;    // 内核结束物理地址
	for (uintptr_t pa = kernel_start; pa < kernel_end; pa += PAGE_SIZE) {
		uint64_t index = (pa - phys_mem_start) / PAGE_SIZE;
		phys_page_array[index].is_used = true;
	}
}


// 将虚拟地址 [va, va+size) 映射到物理地址 [pa, pa+size)
void map_pages(uint64_t* pt, uint64_t va, uint64_t pa, uint64_t size, int perm) {
	for (; size > 0; va += PAGE_SIZE, pa += PAGE_SIZE, size -= PAGE_SIZE) {
		uint64_t *pte = walk(pt, va, 1); // 查找或创建 PTE
		*pte = PA2PTE(pa) | perm | PTE_V; // 设置 PTE
	}
	asm volatile("sfence.vma"); // 刷新 TLB
}

// 从虚拟地址 va 遍历页表，返回对应 PTE 的指针（alloc=1 时自动创建缺失的页表）
uint64_t* walk(uint64_t* pagetable, uint64_t va, int alloc) {
	for (int level = 2; level > 0; level--) {
		uint64_t *pte = &pagetable[PX(level, va)];  // PX 宏提取当前层索引
		if (!(*pte & PTE_V)) {                   // PTE 无效
			if (!alloc || !(pagetable = alloc_pages(1)))
				return 0;
			*pte = PA2PTE(pagetable) | PTE_V;    // 设置新页表的 PTE
		}
		pagetable = (uint64_t*)PTE2PA(*pte);   // 进入下一级页表
	}
	return &pagetable[PX(0, va)];                // 返回叶级 PTE
}
// 分配多页内存
void *alloc_pages(uint8_t n) {
	if (n>MAX_ORDER) {
		return NULL;
	}
	uint16_t pages_num=EXP_OF_2(n);
	if (list_empty(&memory_zone->free_area[n].free_list)) {
		if (n==MAX_ORDER) {
			return NULL;
		}
		struct page *p = list_entry(memory_zone->free_area[n+1].free_list.next, struct page, list);
		struct list_head *cur=&p->list;
		for (int i=0;i<pages_num;i++) {
			list_entry(cur,struct page,list)->is_used=true;
			cur=cur->next;
		}
		struct list_head *last=cur;
		for (int i=0;i<pages_num;i++) {
			struct list_head *temp=cur->next;
			list_move(cur,memory_zone->free_area[i].free_list.next);
			cur->next=temp;
		}
		last->next=&p->list;
		memory_zone->free_area[n+1].nr_free--;
		memory_zone->free_area[n].nr_free++;
		p->order=n;
		return (void *)((p->pfn) << PAGE_SHIFT);
	}
	struct page *p = list_entry(memory_zone->free_area[n].free_list.next, struct page, list);
	struct list_head *cur=&p->list;
	for (int i=0;i<pages_num;i++) {
		struct list_head *temp=cur->next;
		list_entry(cur,struct page,list)->is_used=true;
		list_move(cur,p->list.next);
		cur=temp;
	}
	return (void *)((p->pfn) << PAGE_SHIFT);
}

void free_pages(void* p) {
	uint64_t pfn=PHYS_PFN(p);
	struct page *page=&phys_page_array_virt_addr[pfn];
	uint16_t block_num=EXP_OF_2(page->order);
	list_add(&page->list,&memory_zone->free_area[page->order].free_list);
	memory_zone->free_area[block_num].nr_free++;
}




