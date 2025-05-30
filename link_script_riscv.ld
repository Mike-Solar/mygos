/* linker.ld */
MEMORY {
    RAM (rwx) : ORIGIN = 0xFFFFFFC000000000, LENGTH = 256M
}

SECTIONS {
    . = 0xFFFFFFC000000000;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) }
    .buddy_meta : {
        __buddy_meta_start = .;
        *(.buddy_meta)
        __buddy_meta_end = .;
    }
    /* 堆区域：从 0xFFFFFFC020000000 开始 */
    .heap : {
        __heap_start = .;
        . += 1024M;  /* 预留 1024MB 堆空间 */
        __heap_end = .;
    } > RAM

}

