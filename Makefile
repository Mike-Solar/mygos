RISCV_PREFIX = riscv64-unknown-elf
CC = $(RISCV_PREFIX)-gcc
LD = $(RISCV_PREFIX)-ld
OBJCOPY = $(RISCV_PREFIX)-objcopy

all: build/kernel.bin

build:
	mkdir -p build

build/kernel.elf: start.S | build
	$(CC) -nostartfiles -T linker.ld -o $@ $<

build/kernel.bin: build/kernel.elf
	$(OBJCOPY) -O binary $< $@

run: build/kernel.bin
	qemu-system-riscv64 -machine virt -nographic -bios default -kernel $<
