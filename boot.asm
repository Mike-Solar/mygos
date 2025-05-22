; boot.asm - 最小可启动的 Bootloader
BITS 16
ORG 0x7C00

start:
    mov ah, 0x0E       ; BIOS TTY 输出功能
    mov al, 'H'
    int 0x10
    mov al, 'i'
    int 0x10

    cli
    hlt

times 510 - ($ - $$) db 0  ; 填充至 512 字节
dw 0xAA55                  ; 启动扇区魔数
