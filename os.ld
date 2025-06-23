
/* rvos.ld 用于 RVOS 的链接脚本 */


#include "platform.h"


OUTPUT_ARCH( "riscv" )  /* OUTPUT_ARCH 命令指定输出文件的目标架构 */
ENTRY( _start )         /* ENTRY 命令用于设置程序的入口点 */


/* MEMORY 命令描述目标设备的内存布局 */
MEMORY
{
    ram (wxa!ri) : ORIGIN = 0x80200000, LENGTH = LENGTH_RAM /* 128M */
}


/* SECTIONS 命令告诉链接器如何将输入节映射到输出节，以及如何将输出节放置在内存中 */
SECTIONS
{
    /* 程序代码段 */
    .text : {
        PROVIDE(_text_start = .);
        *(.text .text.*)
        PROVIDE(_text_end = .);
    } >ram

    .early_page (NOLOAD): {
            . = ALIGN(4096);
            early_pg_dir = .;
            . += 4096;  /* 分配一页（4KB）作为早期页目录 */
    } > ram
    /* 只读数据段 */
    .rodata : {
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        PROVIDE(_rodata_end = .);
    } >ram

    /* 初始化数据段 */
    .data : {
        /* 对齐到 4096 字节 */
        . = ALIGN(4096);
        PROVIDE(_data_start = .);
        *(.sdata .sdata.*) /* 无需区分 .sdata 和 .data，RVOS 不使用小数据区 */
        *(.data .data.*)
        PROVIDE(_data_end = .);
    } >ram

    /* 未初始化数据段 */
    .bss :{
        PROVIDE(_bss_start = .);
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } >ram

    /* 定义内存区域的符号 */
    PROVIDE(_memory_start = ORIGIN(ram));
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

    PROVIDE(_heap_start = _bss_end);
    PROVIDE(_heap_size = _memory_end - _heap_start);
}
