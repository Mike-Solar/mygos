# Makefile - 构建 bootloader 并用 QEMU 启动

OUT_DIR = build
BOOT_BIN = $(OUT_DIR)/boot.bin

.PHONY: all run clean

all: $(BOOT_BIN)

# 编译 boot.asm 为 16 位裸机 bin 文件
$(BOOT_BIN): io.c io.h
	mkdir -p $(OUT_DIR)
	riscv64-unknown-elf-gcc io.c kput.S

clean:
	rm -rf $(OUT_DIR)
