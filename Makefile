ASM = nasm
FILES = boot second_stage
BIN_FILES = $(FILES:%=%.bin)

.PHONY: run
.PHONY: compile
.PHONY: clean

run: $(FILES) compile
	dd if=/dev/zero of=OS.bin bs=512 count=5
	cat $(BIN_FILES)  > temp.bin
	dd if=temp.bin of=OS.bin conv=notrunc
	rm -rf temp.bin
	qemu-system-i386 -drive format=raw,file=OS.bin,if=ide,index=0,media=disk

$(FILES):
	nasm -f bin -o $@.bin $@.s
	nasm -felf32 -o helper.bin helper.s
compile:
	clang -std=c17 -m32 -march=i386 -ffreestanding -fno-builtin -nostdinc -O1 -c kernel.c -o kernel.o	
	ld -m elf_i386  -Tkernel.ld helper.bin kernel.o --oformat binary -o kernel.bin
clean:
