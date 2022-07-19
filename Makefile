ASM = nasm
FILES = boot second_stage
BIN_FILES = $(FILES:%=bin/%.bin)

.PHONY: run
.PHONY: build
.PHONY: compile
.PHONY: clean

run: $(FILES) compile build
	qemu-system-i386 -drive format=raw,file=OS.bin,if=ide,index=0,media=disk

build: $(FILES) compile
	dd if=/dev/zero of=OS.bin bs=512 count=32
	cat $(BIN_FILES) test_font.bin tester.bin bin/kernel.bin > temp.bin
	dd if=temp.bin of=OS.bin conv=notrunc
	rm -rf temp.bin

$(FILES):
	nasm -f bin -o bin/$@.bin Bootloader/$@.s
	nasm -f bin -o test_font.bin test_font.s
compile:
	clang -std=gnu99  -masm=intel -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -m32 -march=i386 -fno-pic -MMD -mno-80387 -fno-stack-protector -ffreestanding -fno-builtin -nostdinc -O1 -c Kernel/kernel.c -o bin/kernel.o
	ld -m elf_i386 -TKernel/kernel.ld bin/kernel.o --oformat binary -o bin/kernel.bin
	rm -rf paging.o
clean:
	rm -rf *bin/*
	rm -rf OS.bin
