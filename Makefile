# Makefile - 构建 bootloader 并用 QEMU 启动

OUT_DIR = build
BOOT_BIN = $(OUT_DIR)/boot.bin

.PHONY: all run clean

all: $(BOOT_BIN)

# 编译 boot.asm 为 16 位裸机 bin 文件
$(BOOT_BIN): boot.asm
	mkdir -p $(OUT_DIR)
	nasm -f bin boot.asm -o $(BOOT_BIN)

run: all
	qemu-system-i386 -fda $(BOOT_BIN)

clean:
	rm -rf $(OUT_DIR)
