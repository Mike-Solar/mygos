//
// Created by katherinesolar on 25-5-30.
//

#ifndef KMATH_H
#define KMATH_H

#include <typedefs.h>
inline uint64_t kpowl(uint64_t base, uint64_t exp) {
	uint64_t result = 1;
	for (uint64_t i=0;i<exp;i++) {
		result *= base;
	}
	return result;
}
inline uint32_t kpow(uint32_t base, uint32_t exp) {
	uint32_t result = 1;
	for (uint32_t i=0;i<exp;i++) {
		result *= base;
	}
	return result;
}
#endif //KMATH_H
