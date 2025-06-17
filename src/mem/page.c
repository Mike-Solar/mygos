//
// Created by katherinesolar on 25-5-27.
//

#include "page.h"
#include "list.h"


#include "device_tree_parser.h"
#include "kmath.h"
#define PAGE_SHIFT	12
uint64_t page_count=0;
// 物理页帧状态位图（每个位表示一页：0-空闲，1-已分配）
 struct page phys_page_array[PAGE_COUNT];
struct page *phys_page_array_phys_addr;
struct page *phys_page_array_virt_addr;
extern void *early_page_dir;
struct zone memory_zone __attribute__((section(".buddy_meta")));
const uintptr_t KERNEL_PHYS_ADDR=0x80200000uL;
const uintptr_t KERNEL_VIRT_ADDR=0xFFFFFFFF80000000uL;
#define PHYS_TO_VIRT(phys) ((void*)((uint64_t)(phys) + KERNEL_VIRT_ADDR))

const uintptr_t OPEN_SBI_PHYS_ADDR=0x80000000uL;
#define ALIGN_UP(x, align)  (((x) + ((align) - 1)) & ~((align) - 1))

void buddy_init_stage1(void) {
	// 使用物理地址初始化链表
	for (int i = 0; i <= MAX_ORDER; i++) {

		init_list_head(&memory_zone.free_area[i].free_list);

		memory_zone.free_area[i].nr_free=0;
	}
	memory_zone.free_area[MAX_ORDER].nr_free=page_count/EXP_OF_2(MAX_ORDER);
	phys_page_array_phys_addr=phys_page_array;
}
/* 第二阶段初始化（启用分页后）
void buddy_init_stage2(void) {

	phys_page_array_virt_addr=PHYS_TO_VIRT(phys_page_array_phys_addr);
	// 将物理地址转换为虚拟地址
	for (int i = 0; i <= MAX_ORDER; i++) {
		struct list_head *curr = memory_zone.free_area[i].free_list.next;
		while (curr != &memory_zone.free_area[i].free_list) {
			// Step 1: 将链表节点物理地址转为虚拟地址
			struct list_head *virt_node = PHYS_TO_VIRT(curr);
            
			// Step 2: 修正前后指针的地址
			virt_node->prev = PHYS_TO_VIRT(virt_node->prev);
			virt_node->next = PHYS_TO_VIRT(virt_node->next);
            
			curr = virt_node->next;
		}
		// 修正链表头指针
		memory_zone.free_area[i].free_list.next = PHYS_TO_VIRT(memory_zone.free_area[i].free_list.next);
		memory_zone.free_area[i].free_list.prev = PHYS_TO_VIRT(memory_zone.free_area[i].free_list.prev);
	}
}*/
void early_enable_paging(stap_t satp) {
	// 设置 satp 寄存器（Sv39 模式）
	asm volatile("csrw satp, %0" : : "r"(satp));
	asm volatile("sfence.vma"); // 刷新 TLB
}
void enable_paging(pte_t* pagetable) {
	// 设置 satp 寄存器（Sv39 模式）
	uint64_t satp_value = ((uint64_t)pagetable >> 12) | (8ULL << 60); // MODE=8 (Sv39)
	asm volatile("csrw satp, %0" : : "r"(satp_value));
	asm volatile("sfence.vma"); // 刷新 TLB
}
//建立初始映射
stap_t early_paging_init(void) {
	zero(early_pt_2, sizeof(pte_t)*PAGE_SIZE/sizeof(pte_t));
	zero(early_pt_1, sizeof(pte_t)*PAGE_SIZE/sizeof(pte_t));
	zero(early_pt_0, sizeof(pte_t)*PAGE_SIZE/sizeof(pte_t)*PAGE_SIZE/sizeof(pte_t));
	stap_t stap;
	stap.MODE=8;
	phys_addr_t *phys_addr=(phys_addr_t*) &early_pt_2;
	stap.PPN_0=phys_addr->PPN_0;
	stap.PPN_1=phys_addr->PPN_1;
	stap.PPN_2=phys_addr->PPN_2;
	//一个大页，映射内核后面的1G

	pte_t pte={0};
	pte.R=0;
	pte.W=0;
	pte.X=0;
	pte.V=1;
	pte.PPN_2=(uintptr_t)(&early_pt_1) >> 30;
	pte.PPN_1=(uintptr_t)(&early_pt_1) >> 21;
	pte.PPN_0=(uintptr_t)(&early_pt_1) >> 12;
	early_pt_2[0]=pte;



	uintptr_t hardware_start_addr=0x10000000uL;
	pte.R=1;
	pte.W=1;
	pte.X=1;
	pte.PPN_2=hardware_start_addr>>30;
	pte.PPN_1=hardware_start_addr>>21;
	pte.PPN_0=hardware_start_addr>>12;
	early_pt_0[0][0]=pte;
	uintptr_t cur=OPEN_SBI_PHYS_ADDR;
	for (int j=0;j<PAGE_SIZE / sizeof(pte_t) && cur < OPEN_SBI_PHYS_ADDR+memory.size;j++) {
		for (int i=1;i<512 && cur <= KERNEL_PHYS_END;i++) {
			pte.R=1;
			pte.W=1;
			pte.X=1;
			pte.PPN_2=cur >> 30;
			pte.PPN_1=cur >> 21;
			pte.PPN_0=cur >> 12;
			if (cur<KERNEL_PHYS_END) {
				pte.RSW |=1;
			}
			early_pt_0[j][i]=pte;
			cur+=PAGE_SIZE;
			pte.RSW=0;
		}
		pte.R=0;
		pte.W=0;
		pte.X=0;
		pte.PPN_2=(uintptr_t)(&early_pt_0[j]) >> 30;
		pte.PPN_1=(uintptr_t)(&early_pt_0[j]) >> 21;
		pte.PPN_0=(uintptr_t)(&early_pt_0[j]) >> 12;
		early_pt_1[j]=pte;
	}
	return stap;
}

void* early_alloc() {
	uintptr_t cur=KERNEL_PHYS_END;
	for (int j=0;j<PAGE_SIZE/sizeof(pte_t) && cur <= OPEN_SBI_PHYS_ADDR+memory.size;j++) {
		for (int i=0;i<PAGE_SIZE/sizeof(pte_t);i++, cur+=PAGE_SIZE) {
			if (early_pt_0[j][i].RSW & 1 == 1) {
				continue;
			}
			early_pt_0[j][i].RSW |= 1;
			uintptr_t ptr;
			ptr |= early_pt_0[j][i].PPN_0 << 30;
			ptr |= early_pt_0[j][i].PPN_1 << 21;
			ptr |= early_pt_0[j][i].PPN_2 << 12;
			return (void*)ptr;
		}
	}
	return NULL;
}
// 建立内核恒等映射（虚拟地址 = 物理地址）
void map_kernel_identity(pte_t* pagetable) {
	// 映射内核代码和数据（假设物理地址从 0x80000000 开始）
	map_pages(pagetable, KERNEL_VIRT_ADDR, KERNEL_PHYS_ADDR, KERNEL_SPACE, PTE_R | PTE_W | PTE_X);

	// 映射设备寄存器（如 UART 地址 0x10000000）
	map_pages(pagetable, 0x10000000uL, 0x10000000uL, PAGE_SIZE, PTE_R | PTE_W);
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
	uintptr_t kernel_start = KERNEL_PHYS_ADDR;    // 内核起始物理地址
	uintptr_t kernel_end   = KERNEL_PHYS_END;    // 内核结束物理地址
	for (uintptr_t pa = kernel_start; pa < kernel_end; pa += PAGE_SIZE) {
		uint64_t index = (pa - phys_mem_start) / PAGE_SIZE;
		phys_page_array[index].is_used = true;
	}
}


// 将虚拟地址 [va, va+size) 映射到物理地址 [pa, pa+size)
void map_pages(pte_t* pt, uint64_t virt_addr, uint64_t phys_addr, uint64_t size, int perm) {
	for (; size > 0; virt_addr += PAGE_SIZE, phys_addr += PAGE_SIZE, size -= PAGE_SIZE) {
		pte_t *pte = walk(pt, virt_addr, 1); // 查找或创建 PTE
		if (perm & PTE_R == PTE_R) {
			pte->R=1;
		}
		if (perm & PTE_W == PTE_W) {
			pte->W=1;
		}
		if (perm & PTE_X == PTE_X) {
			pte->X=1;
		}
		pte->V=1;
		pte->A=0;
		pte->PPN_2=phys_addr>>30;
		pte->PPN_1=phys_addr>>21;
		pte->PPN_0=phys_addr>>12;
	}
	asm volatile("sfence.vma"); // 刷新 TLB
}
// 从虚拟地址 va 遍历页表，返回对应 PTE 的指针（alloc=1 时自动创建缺失的页表）
pte_t* walk(pte_t* pagetable, uint64_t virt_addr, int alloc) {
	for (int level = 2; level >= 0; level--) {
		pte_t *pte = &pagetable[PX(level, virt_addr)];  // PX 宏提取当前层索引
		if (!pte->V) {                   // PTE 无效
			if (!alloc || !((pagetable = early_alloc())))
				return NULL;
			pte->V = 1;
			pte->PPN_2=(uintptr_t)pagetable>>30;
			pte->PPN_1=(uintptr_t)pagetable>>21;
			pte->PPN_0=(uintptr_t)pagetable>>12;
		}
		pagetable = (pte_t*)(pte->PPN_2<<30 | pte->PPN_1 << 21 | pte->PPN_0 << 12);   // 进入下一级页表
	}
	return &pagetable[PX(0, virt_addr)];                // 返回叶级 PTE
}
// 分配多页内存
void *alloc_pages(uint8_t n) {
	if (n>MAX_ORDER) {
		return NULL;
	}
	uint16_t pages_num=EXP_OF_2(n);
	if (list_empty(&memory_zone.free_area[n].free_list)) {
		if (n==MAX_ORDER) {
			return NULL;
		}
		struct page *p = list_entry(memory_zone.free_area[n+1].free_list.next, struct page, list);
		struct list_head *cur=&p->list;
		for (int i=0;i<pages_num;i++) {
			list_entry(cur,struct page,list)->is_used=true;
			cur=cur->next;
		}
		struct list_head *last=cur;
		for (int i=0;i<pages_num;i++) {
			struct list_head *temp=cur->next;
			list_move(cur,memory_zone.free_area[i].free_list.next);
			cur->next=temp;
		}
		last->next=&p->list;
		memory_zone.free_area[n+1].nr_free--;
		memory_zone.free_area[n].nr_free++;
		p->order=n;
		return (void *)((p->pfn) << PAGE_SHIFT);
	}
	struct page *p = list_entry(memory_zone.free_area[n].free_list.next, struct page, list);
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
	list_add(&page->list,&memory_zone.free_area[page->order].free_list);
	memory_zone.free_area[block_num].nr_free++;
}




