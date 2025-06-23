//
// Created by katherinesolar on 25-5-24.
//

#ifndef DEVICE_TREE_PARSER_H
#define DEVICE_TREE_PARSER_H
#include "typedefs.h"
struct fdt_header {
	uint32_t magic;          // 魔数（0xd00dfeed）
	uint32_t totalsize;      // DTB 总大小
	uint32_t off_dt_struct;  // 结构块偏移
	uint32_t off_dt_strings; // 字符串块偏移
	uint32_t off_mem_rsvmap; // 保留内存块偏移
	uint32_t version;        // DTB 版本
	uint32_t last_comp_version; // 兼容的最低版本
	uint32_t boot_cpuid_phys;   // 启动 CPU 的物理 ID
	uint32_t size_dt_strings;   // 字符串块大小
	uint32_t size_dt_struct;    // 结构块大小
} __attribute__((packed));
//内存信息
struct memory {
	uint64_t base;
	uint64_t size;
};
//设备信息
struct device {
	struct memory memory;
};
extern struct memory memory;
void parse_memory_reg(void *data, uint32_t len);
void parse_device_tree(struct fdt_header* header);
#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP       0x00000003
#define FDT_NOP        0x00000004
#define FDT_END        0x00000009

#endif //DEVICE_TREE_PARSER_H
