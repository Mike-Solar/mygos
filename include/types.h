
// types.h

#ifndef __TYPES_H__
#define __TYPES_H__


typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

/*/
 * Register Width
/*/
typedef uint64_t reg_t;
typedef uint64_t ptr_t;


#define nullptr ((void*)0)

typedef struct timer* timer_ptr; // 计时器指针类型


#endif /* __TYPES_H__ */
