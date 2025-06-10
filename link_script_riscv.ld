ENTRY(_start)

/* 定义物理和虚拟基址 */
PHYS_BASE = 0x80200000;
VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS {
    . = PHYS_BASE;
    /* 首先放置入口段 */
    .text.entry : {
        *(.text.entry)  /* 确保 _start 在此处 */
    }
    .text : {
    		PROVIDE(_text_start = .);   /* 将 _text_start 定义为当前地址 */
    		*(.text .text.*)            /* 将所有 .text 和 .text.* 节添加到当前地址 */
    		PROVIDE(_text_end = .);     /* 将 _text_end 定义为当前地址 */
    }

    .buddy_meta : {
        __buddy_meta_start = .;
        *(.buddy_meta)
        __buddy_meta_end = .;
    }

    .rodata : {
    	PROVIDE(_rodata_start = .);
    	*(.rodata .rodata.*)
    	PROVIDE(_rodata_end = .);
    }

    .early_page (NOLOAD): {
        . = ALIGN(4096);
        __early_page_start = .;
        *(.early_page)
        .  = ALIGN(4096);
        __early_page_end = .;
    }
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
    }
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
    }

    /* 堆区域：从 0xFFFFFFC020000000 开始 */
    .heap : {
        __heap_start = .;
        . += 1024M;  /* 预留 1024MB 堆空间 */
        __heap_end = .;
    }

}

