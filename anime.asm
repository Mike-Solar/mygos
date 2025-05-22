[BITS 16]
[ORG 0x7C00]

start:
    ; 初始化段寄存器
    xor ax, ax
    mov ds, ax

.loop:
    mov si, spinner     ; SI 指向动画字符数组

.spin:
    lodsb               ; 取下一个字符 -> AL，SI++
    cmp al, 0
    je .loop            ; 如果到 '\0'，重新从头开始

    mov ah, 0x0E        ; BIOS TTY 输出字符
    int 0x10            ; 输出 AL

    ; 简单延时（低级忙等待）
    call delay

    ; 输出回退字符（退格）
    mov al, 0x08        ; ASCII Backspace
    int 0x10

    jmp .spin

; 延时子程序（粗略）
delay:
    mov cx, 0FFFFh
.delay_loop:
    nop
    loop .delay_loop
    ret

spinner:
    db '/', '-', '\\', '|', 0

; 填充到 512 字节
times 510 - ($ - $$) db 0
dw 0xAA55
