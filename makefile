ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector \
         -nostdlib \
         -nostdinc \
         -Wall \
         -Wextra

LDFLAGS = -m elf_i386 \
          -T linker.ld

OBJECTS = boot_kernel.o main.o rendering.o keyboard.o


all: unit-os.iso


boot_kernel.o: boot_kernel.asm
	$(ASM) -f elf32 boot_kernel.asm -o boot_kernel.o


main.o: main.c rendering.h keyboard.h
	$(CC) $(CFLAGS) -c main.c -o main.o


rendering.o: rendering.c rendering.h
	$(CC) $(CFLAGS) -c rendering.c -o rendering.o


keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c -o keyboard.o


unit-os: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o unit-os


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


run: unit-os.iso
	qemu-system-i386 -cdrom unit-os.iso


clean:
	rm -f *.o unit-os unit-os.iso
	rm -rf iso
