.section text
.globl kput

kput:
    addi sp, sp, -16
    sw ra, 0(sp)
    sw s0, 4(sp)
    mv s0, a0
.loop:
    lbu a0,0(s0)
    beqz a0, .puts_end
    jal putc
    addi s0,s0,1
    j .loop
.puts_end:
    lw ra, 0(sp)
    lw s0, 4(sp)
    addi sp, sp, 16
    ret
putc:
    li t0, 0x10000000
    sb a0, 0(t0)
    ret