//
// Created by katherinesolar on 25-5-24.
//

#ifndef ENDIAN_H
#define ENDIAN_H
#include <typedefs.h>
inline uint32_t be32toh(uint32_t val) {
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
inline uint64_t be64toh(uint64_t val) {
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
#endif //ENDIAN_H
