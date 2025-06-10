//
// Created by katherinesolar on 25-5-23.
//
//#include "kmemory.h"
#include "page.h"
#include "device_tree_parser.h"
#include "uart.h"
uint64_t kernel_pagetable[PAGE_SIZE / 8];

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
		kernel_pagetable[i]=0;
	}
	uart_puts("kernel page table inited...\n");

	// 建立恒等映射
	map_kernel_identity(kernel_pagetable);

	// 启用分页
	enable_paging(kernel_pagetable);

	// 测试页分配
	int *page=alloc_pages(1);
	free_pages(page);
	 for (;;);
}