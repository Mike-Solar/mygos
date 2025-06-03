//
// Created by katherinesolar on 25-5-23.
//
#include "kmemory.h"
#include "page.h"
#include "device_tree_parser.h"

int __attribute__((section(".boot.text"))) kernel_init(void *dtd) {
	parse_device_tree(dtd);
	page_count=memory.size / PAGE_SIZE;
	// 初始化物理页帧分配器
	phys_mem_init();

	// 创建内核页表
	uint64_t* kernel_pagetable = (uint64_t *)page_alloc();
	zero(kernel_pagetable, PAGE_SIZE);

	// 建立恒等映射
	map_kernel_identity(kernel_pagetable);

	// 启用分页
	enable_paging(kernel_pagetable);
}