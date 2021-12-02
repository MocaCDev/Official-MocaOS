ASM = nasm
FILES = boot second_stage
BIN_FILES = $(FILES:%=bin/%.bin)

.PHONY: run
.PHONY: compile
.PHONY: clean

run: $(FILES) compile
	dd if=/dev/zero of=OS.bin bs=512 count=20
	cat $(BIN_FILES) test_font.bin bin/kernel.bin > temp.bin
	dd if=temp.bin of=OS.bin conv=notrunc
	rm -rf temp.bin
	qemu-system-i386 -drive format=raw,file=OS.bin,if=ide,index=0,media=disk

$(FILES):
	nasm -f bin -o bin/$@.bin Bootloader/$@.s
	nasm -f bin -o test_font.bin test_font.s
compile:
	clang -std=c17 -m32 -march=i386 -ffreestanding -fno-builtin -nostdinc -O1 -c Kernel/kernel.c -o bin/kernel.o	
	ld -m elf_i386  -TKernel/kernel.ld bin/kernel.o --oformat binary -o bin/kernel.bin
clean:
	rm -rf *bin/*
	rm -rf OS.bin
