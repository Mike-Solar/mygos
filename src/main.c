//
// Created by katherinesolar on 25-5-23.
//
//#include "kmemory.h"
#include "page.h"
#include "device_tree_parser.h"
#include "uart.h"
pte_t kernel_pagetable[PAGE_SIZE / sizeof(pte_t)];
pte_t early_pt_2[PAGE_SIZE / sizeof(pte_t)]__attribute__((section(".early_page")));
pte_t early_pt_1[PAGE_SIZE / sizeof(pte_t)]__attribute__((section(".early_page")));
pte_t early_pt_0[PAGE_SIZE / sizeof(pte_t)][PAGE_SIZE / sizeof(pte_t)] __attribute__((section(".early_page")));

void kernel_init(int hart_id, void *dtd) {
	parse_device_tree(dtd);
	uart_init();
	uart_puts("loading kernel...\n");
	page_count=memory.size / PAGE_SIZE;
	// 初始化物理页帧分配器
	phys_mem_init();
	uart_puts("phys page inited...\n");

	// 创建内核页表
	for (int i=0;i<PAGE_SIZE/8;i++) {
		zero(&kernel_pagetable[i],sizeof(pte_t));
	}
	uart_puts("kernel page table inited...\n");

	//stap_t stap= early_paging_init();

	// 启用分页
	//early_enable_paging(stap);
	map_kernel_identity(kernel_pagetable);

	// 测试页分配
	int *page=alloc_pages(1);
	free_pages(page);
	 for (;;);
}