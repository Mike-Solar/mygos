//
// Created by katherinesolar on 25-5-23.
//
//#include "kmemory.h"
#include "page.h"
#include "device_tree_parser.h"
uint64_t __attribute__((section(".boot.data"))) kernel_pagetable[PAGE_SIZE / 8];

void __attribute__((section(".boot.text"))) kernel_init(void *dtd) {
	parse_device_tree(dtd);
	page_count=memory.size / PAGE_SIZE;
	// 初始化物理页帧分配器
	phys_mem_init();

	// 创建内核页表
	for (int i=0;i<PAGE_SIZE/8;i++) {
		kernel_pagetable[i]=0;
	}

	// 建立恒等映射
	// map_kernel_identity(kernel_pagetable);

	// 启用分页
	enable_paging(kernel_pagetable);
}