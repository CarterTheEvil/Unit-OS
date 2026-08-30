# ================================================
# Unit-OS Makefile
# ================================================

ASM = nasm
CC = gcc
LD = ld


# ------------------------------------------------
# Compiler flags
# ------------------------------------------------

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector \
         -nostdlib \
         -Wall \
         -Wextra


# ------------------------------------------------
# Linker flags
# ------------------------------------------------

LDFLAGS = -m elf_i386 \
          -T linker.ld


# ------------------------------------------------
# Kernel object files
# ------------------------------------------------

OBJECTS = \
    boot_kernel.o \
    main.o \
    rendering.o \
    keyboard.o \
    interrupts.o \
    timer.o


# ================================================
# Default target
# ================================================

all: unit-os.iso


# ================================================
# Bootloader
# ================================================

boot_kernel.o: boot_kernel.asm
	$(ASM) -f elf32 boot_kernel.asm -o boot_kernel.o


# ================================================
# C files
# ================================================

main.o: main.c rendering.h keyboard.h
	$(CC) $(CFLAGS) -c main.c -o main.o


rendering.o: rendering.c rendering.h
	$(CC) $(CFLAGS) -c rendering.c -o rendering.o


keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c -o keyboard.o


interrupts.o: interrupts.c
	$(CC) $(CFLAGS) -c interrupts.c -o interrupts.o


# ================================================
# Link kernel
# ================================================

unit-os: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o unit-os


# ================================================
# Create ISO
# ================================================

unit-os.iso: unit-os

	mkdir -p iso/boot/grub

	cp unit-os iso/boot/unit-os

	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "Unit-OS" {' >> iso/boot/grub/grub.cfg
	echo '    multiboot /boot/unit-os' >> iso/boot/grub/grub.cfg
	echo '    boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	grub-mkrescue -o unit-os.iso iso


# ================================================
# Run Unit-OS
# ================================================

run: unit-os.iso
	qemu-system-i386 -cdrom unit-os.iso


# ================================================
# Clean
# ================================================

clean:
	rm -f *.o
	rm -f unit-os
	rm -f unit-os.iso
	rm -rf iso
