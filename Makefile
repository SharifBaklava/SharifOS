# === Paths ===
KERNEL_SRC_DIR := src/impl/kernel
I686_SRC_DIR := src/impl/i686
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/isodir
GRUB_DIR := $(ISO_DIR)/boot/grub

# === Source files ===
KERNEL_C_SRCS := $(shell find $(KERNEL_SRC_DIR) -name '*.cpp')
I686_C_SRCS := $(shell find $(I686_SRC_DIR) -name '*.cpp')
I686_ASM_SRCS := $(shell find $(I686_SRC_DIR) -name '*.asm')

# === Object files ===
KERNEL_OBJS := $(patsubst $(KERNEL_SRC_DIR)/%.cpp, $(BUILD_DIR)/kernel/%.o, $(KERNEL_C_SRCS))
I686_C_OBJS := $(patsubst $(I686_SRC_DIR)/%.cpp, $(BUILD_DIR)/i686/%.o, $(I686_C_SRCS))
I686_ASM_OBJS := $(patsubst $(I686_SRC_DIR)/%.asm, $(BUILD_DIR)/i686/%.o, $(I686_ASM_SRCS))
I686_OBJS := $(I686_C_OBJS) $(I686_ASM_OBJS)

# === Tools ===
CC := i686-elf-g++
LD := i686-elf-ld
NASM := nasm
OBJCOPY := i686-elf-objcopy

# === Build output ===
BOOT_OBJ := $(BUILD_DIR)/boot.o
KERNEL_OBJ := $(BUILD_DIR)/kernel.o
BIN := $(BUILD_DIR)/sharifos.bin
ISO := $(BUILD_DIR)/sharifos.iso

# === Main Targets ===
.PHONY: all clean kernel i686 iso dirs

all: $(ISO)

# === ISO creation ===
$(ISO): $(BIN) | $(GRUB_DIR)
	cp grub.cfg $(GRUB_DIR)/
	cp $(BIN) $(ISO_DIR)/boot/sharifos.bin
	grub-mkrescue -o $@ $(ISO_DIR)

# === Kernel binary ===
$(BIN): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

# === Kernel main C file ===
$(KERNEL_OBJ): src/impl/kernel/main.cpp | dirs
	$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# === Boot assembly file ===
$(BOOT_OBJ): src/impl/i686/boot/main.asm | dirs
	$(NASM) -f elf32 $< -o $@

# === Directories ===
dirs:
	mkdir -p $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)/i686
	mkdir -p $(GRUB_DIR)

# === Clean ===
clean:
	rm -rf build
	rm -f $(ISO)
