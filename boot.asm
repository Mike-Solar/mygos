[BITS 16]
[ORG 0x7C00]

start:
    ; 设置段寄存器
    xor ax, ax
    mov ds, ax

    ; 指向我们要输出的字符串
    mov si, message

.print_loop:
    lodsb               ; 加载 [SI] -> AL，SI++
    cmp al, 0
    je .done            ; 如果是 '\0' 就结束

    mov ah, 0x0E        ; BIOS TTY 输出功能
    int 0x10            ; 输出 AL 中的字符
    jmp .print_loop

.done:
    cli
    hlt

message:
    db 'Hello, OS World!', 0

; 填充到 512 字节
times 510 - ($ - $$) db 0
dw 0xAA55
