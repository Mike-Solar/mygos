SRCS_ASM = \
	src/start.S \
	src/mem.S \
	src/entry.S \

SRCS_C = \
	src/kernel.c \
	src/io/uart.c \
	src/io/printf.c \
	src/io/plic.c \
	src/page.c \
	src/sched.c \
	src/user.c \
	src/trap.c \
	src/timer.c \
	# src/riscv.c \

GDBINIT = ./gdbinit

# Common part for the Makefile.
# This file will be included by the Makefile of each project.

# Custom Macro Definition (Common part)

# Define macros for conditional compilation.
# Support customization in the Makefile files for each project separately.
# And also support customization in common.mk

ifeq (${SYSCALL}, y)
DEFS += -DCONFIG_SYSCALL
endif


DEFS +=

CROSS_COMPILE 	= riscv64-unknown-elf-
CFLAGS 			+= -nostdlib -fno-builtin -g -Wall
CFLAGS 			+= -march=rv32g -mabi=ilp32
CFLAGS 			+= -I./include

QEMU 			= qemu-system-riscv32
QFLAGS 			= -nographic -smp 1 -machine virt -bios none

GDB 			= gdb-multiarch
CC 				= ${CROSS_COMPILE}gcc
OBJCOPY 		= ${CROSS_COMPILE}objcopy
OBJDUMP 		= ${CROSS_COMPILE}objdump
MKDIR 			= mkdir -p
RM 				= rm -rf

OUTPUT_PATH 	= build

# SRCS_ASM & SRCS_C are defined in the Makefile of each project.
OBJS_ASM := $(addprefix ${OUTPUT_PATH}/, $(patsubst %.S, %.o, ${SRCS_ASM}))
OBJS_C   := $(addprefix $(OUTPUT_PATH)/, $(patsubst %.c, %.o, ${SRCS_C}))
OBJS = ${OBJS_ASM} ${OBJS_C}

ELF = ${OUTPUT_PATH}/os.elf
BIN = ${OUTPUT_PATH}/os.bin

USE_LINKER_SCRIPT ?= true
ifeq (${USE_LINKER_SCRIPT}, true)
LDFLAGS = -T ${OUTPUT_PATH}/os.ld.generated
else
LDFLAGS = -Ttext=0x80000000
endif

.DEFAULT_GOAL := all
all: ${OUTPUT_PATH} ${ELF}

${OUTPUT_PATH}:
	@${MKDIR} $@

# start.o must be the first in dependency!
#
# For USE_LINKER_SCRIPT == true, before do link, run preprocessor manually for
# linker script.
# -E specifies GCC to only run preprocessor
# -P prevents preprocessor from generating linemarkers (#line directives)
# -x c tells GCC to treat your linker script as C source file
${ELF}: ${OBJS}
ifeq (${USE_LINKER_SCRIPT}, true)
	${CC} -E -P -x c ${DEFS} ${CFLAGS} src/os.ld > ${OUTPUT_PATH}/os.ld.generated
endif
	${CC} ${CFLAGS} ${LDFLAGS} -o ${ELF} $^
	${OBJCOPY} -O binary ${ELF} ${BIN}

${OUTPUT_PATH}/%.o : %.c
	@mkdir -p $(dir $@)
	${CC} ${DEFS} ${CFLAGS} -c -o $@ $<

${OUTPUT_PATH}/%.o : %.S
	@mkdir -p $(dir $@)
	${CC} ${DEFS} ${CFLAGS} -c -o $@ $<


run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ${ELF}


.PHONY : debug
debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ${ELF} -s -S &
	@${GDB} ${ELF} -q -x ${GDBINIT}


.PHONY : code
code: all
	@${OBJDUMP} -S ${ELF} | less


.PHONY : clean
clean:
	@${RM} ${OUTPUT_PATH}

