/*
 * rvos.ld
 * 用于 RVOS 的链接脚本
 */

#include "platform.h"

/*
 * https://sourceware.org/binutils/docs/ld/Miscellaneous-Commands.html
 * OUTPUT_ARCH 命令指定输出文件的目标架构。
 * "riscv" 是 64 位和 32 位 RISC-V 目标的架构名称。
 * 具体的 -march 和 -mabi 会在调用 gcc 时进一步指定。
 */
OUTPUT_ARCH( "riscv" )

/*
 * https://sourceware.org/binutils/docs/ld/Entry-Point.html
 * ENTRY 命令用于设置程序的入口点，即程序执行的第一条指令。
 * 这里入口点为 "_start"，它在 start.S 中定义。
 */
ENTRY( _start )

/*
 * https://sourceware.org/binutils/docs/ld/MEMORY.html
 * MEMORY 命令描述目标设备的内存布局。
 * 语法：
 *
 * MEMORY
 * {
 *     name [(attr)] : ORIGIN = origin, LENGTH = len
 *     ......
 * }
 * 
 * Each line defines a memory region.
 * Each memory region must have a distinct name within the MEMORY command. Here
 * we only define one region named as "ram".
 * The "attr" string is an optional list of attributes that specify whether to
 * use a particular memory region for an input section which is not explicitly
 * mapped in the linker script. Here we assign 'w' (writeable), 'x' (executable),
 * and 'a' (allocatable). We use '!' to invert 'r' (read-only) and
 * 'i' (initialized).
 * The "ORIGIN" is used to set the start address of the memory region. Here we
 * place it right at the beginning of 0x8000_0000 because this is where the
 * QEMU-virt machine will start executing.
 * Finally LENGTH = 128M tells the linker that we have 128 megabyte of RAM.
 * The linker will double check this to make sure everything can fit.
 */
MEMORY
{
	ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = LENGTH_RAM
}

/*
 * https://sourceware.org/binutils/docs/ld/SECTIONS.html
 * The SECTIONS command tells the linker how to map input sections into output
 * sections, and how to place the output sections in memory.
 * The format of the SECTIONS command is:
 * SECTIONS
 * {
 *     sections-command
 *     sections-command
 *     ......
 * }
 *
 * Each sections-command may of be one of the following:
 * (1) an ENTRY command                 | 一个入口命令
 * (2) a symbol assignment              | 一个符号赋值
 * (3) an output section description    | 一个输出节描述
 * (4) an overlay description           | 一个覆盖描述
 * We here only demo (2) & (3).
 *
 * We use PROVIDE command to define symbols.
 * https://sourceware.org/binutils/docs/ld/PROVIDE.html
 * The PROVIDE（提供） keyword may be used to define a symbol.
 * The syntax（语法） is PROVIDE(symbol = expression).
 * Such symbols as "_text_start", "_text_end" ... will be used in mem.S.
 * Notice the period '.' tells the linker to set symbol(e.g. _text_start) to
 * the CURRENT location ('.' = current memory location). This current memory
 * location moves as we add things.
 */
SECTIONS
{
	/*
	 * We are going to layout all text sections in .text output section,
	 * starting with .text. The asterisk("*") in front of the
	 * parentheses means to match the .text section of ANY object file.
	 */
	.text : {
		PROVIDE(_text_start = .);   /* 将 _text_start 定义为当前地址 */
		*(.text .text.*)            /* 将所有 .text 和 .text.* 节添加到当前地址 */
		PROVIDE(_text_end = .);     /* 将 _text_end 定义为当前地址 */
	} >ram                          /* 将 .text 节放置在 ram 区域 */

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	} >ram

	.data : {
		/*
		 * . = ALIGN(4096) tells the linker to align the current memory
		 * location to 4096 bytes. This will insert padding bytes until
		 * current location becomes aligned on 4096-byte boundary.
		 * This is because our paging system's resolution is 4,096 bytes.
		 */
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		/*
		 * sdata and data are essentially the same thing. We do not need
		 * to distinguish sdata from data.
         * SDATA 和 DATA 本质上是一回事。我们不需要区分 sdata 和 data。
		 */
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram

	.bss :{
		/*
		 * https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
		 * In most cases, common symbols in input files will be placed
		 * in the ‘.bss’ section in the output file.
         * 在大多数情况下，输入文件中的常用符号将放置在输出文件的 '.bss' 部分。
		 */
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
