ENTRY(_start)

/* 定义物理和虚拟基址 */
PHYS_BASE = 0x80200000;
VIRT_BASE = 0xFFFFFFFF80000000;

MEMORY {
    /* 物理内存区域：内核加载到此 */
    LOAD_MEM (rwx) : ORIGIN = PHYS_BASE, LENGTH = 128M
    /* 虚拟内存区域：内核运行时的虚拟地址空间 */
    RAM (rwx) : ORIGIN = VIRT_BASE, LENGTH = 8192M
}

SECTIONS {
    /* 启动代码必须位于物理地址 */
    . = PHYS_BASE;
    .boot : {
        *(.boot.data)      /* 启动全局变量段 */
        *(.boot.text)      /* 启动代码段 */
        . = ALIGN(4K);     /* 按页对齐 */
    } >LOAD_MEM
    .buddy_meta : {
        __buddy_meta_start = .;
        *(.buddy_meta)
        __buddy_meta_end = .;
    } >RAM AT>LOAD_MEM
    /* 后续段链接到虚拟地址，但加载到物理地址 */
    . = VIRT_BASE;
    .text : {
    		PROVIDE(_text_start = .);   /* 将 _text_start 定义为当前地址 */
    		*(.text .text.*)            /* 将所有 .text 和 .text.* 节添加到当前地址 */
    		PROVIDE(_text_end = .);     /* 将 _text_end 定义为当前地址 */
    } > RAM AT>LOAD_MEM
    .rodata : {
    	PROVIDE(_rodata_start = .);
    	*(.rodata .rodata.*)
    	PROVIDE(_rodata_end = .);
    } > RAM AT>LOAD_MEM
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
    } >RAM AT>LOAD_MEM
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
    } >RAM AT>LOAD_MEM

    /* 堆区域：从 0xFFFFFFC020000000 开始 */
    .heap : {
        __heap_start = .;
        . += 1024M;  /* 预留 1024MB 堆空间 */
        __heap_end = .;
    } >RAM AT>LOAD_MEM

}

