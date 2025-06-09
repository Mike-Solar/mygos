#include "../../include/endian.h"
#include "kstring.h"
#include "device_tree_parser.h"
//
// Created by katherinesolar on 25-5-24.
//
uint32_t  __attribute__((section(".boot.text"))) be32toh(uint32_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	// 小端主机：需要交换字节序
	return ((val >> 24) & 0x000000FF) |
		   ((val >> 8)  & 0x0000FF00) |
		   ((val << 8)  & 0x00FF0000) |
		   ((val << 24) & 0xFF000000);
#else
	// 大端主机：直接返回原值
	return val;
#endif
}
uint64_t __attribute__((section(".boot.text"))) be64toh(uint64_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	// 小端主机：需要交换字节序

	return  ((val >> 56) & 0x00000000000000FFLL) |
			((val >> 40) & 0x000000000000FF00LL) |
			((val >> 24) & 0x0000000000FF0000LL) |
			((val >> 8)  & 0x00000000FF000000LL) |
			((val << 8)  & 0x000000FF00000000LL) |
			((val << 24) & 0x0000FF0000000000LL) |
			((val << 40) & 0x00FF000000000000LL) |
			((val >> 56) & 0xFF00000000000000LL);
#else
	// 大端主机：直接返回原值
	return val;
#endif
}
static __inline__ int __attribute__((section(".boot.text"))) kstrlen_b(char *str) {
	int len = 0;
	// 循环计算字符串长度
	while(*str++) len++;
	return len;
}
static __inline__ int __attribute__((section(".boot.text"))) kstrcmp_b(const char* str1, const char* str2)
{
	// 循环比较字符，直到遇到字符串结束符'\0'
	while(*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}

	// 返回两个字符串的差值
	return *(unsigned char*)str1 - *(unsigned char*)str2;
}
//解析设备树
void __attribute__((section(".boot.text"))) parse_device_tree(struct fdt_header* header) {
	if (be32toh(header->magic) != 0xd00dfeed) {
	}
	// 获取关键偏移量
	uint32_t struct_block = be32toh(header->off_dt_struct);
	uint32_t strings_block = be32toh(header->off_dt_strings);

	// 获取StructBlock开头位置
	uint32_t *ptr = (uint32_t*)((void *)header + struct_block);

	// 解析设备树
	while (1) {
		uint32_t token = be32toh(*ptr);
		ptr++;

		switch (token) {
			case FDT_BEGIN_NODE: {
				// 节点名称（以 \0 结尾）
				char *name = (char*)ptr;
				ptr += (kstrlen_b(name) + 4) / 4; // 按 4 字节对齐
				break;
			}
			case FDT_PROP: {
				// 属性长度和名称偏移
				uint32_t len = be32toh(ptr[0]);
				uint32_t nameoff = be32toh(ptr[1]);
				char *prop_name = (char*)((void *)header + strings_block + nameoff);
				void *prop_data = (void*)(ptr + 2);

				// 提取内存信息
				if (kstrcmp_b(prop_name, "reg") == 0) {
					parse_memory_reg(prop_data, len);
				}

				ptr += 2 + (len + 3) / 4; // 数据按 4 字节对齐
				break;
			}
			case FDT_END_NODE:
				// 节点结束
					break;
			case FDT_END:
				// 整个设备树解析完成
					return;
			default:
				// 未知标记（如 FDT_NOP）
					break;
		}
	}
}
__attribute__((section(".boot.data"))) struct memory memory;
struct memory * memory_ptr;
// 假设父节点定义了 #address-cells=2 和 #size-cells=2
void __attribute__((section(".boot.text"))) parse_memory_reg(void *data, uint32_t len) {
	uint64_t *reg = (uint64_t*)data;
	memory.base = be64toh(reg[0]); // 内存起始地址
	memory.size = be64toh(reg[1]); // 内存大小
}
