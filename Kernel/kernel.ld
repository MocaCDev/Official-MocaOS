SECTIONS {
	.text 0x1F00 :
	{
		*(kernel_entry);
		*(.text*);
	}
	.idt BLOCK(.) : ALIGN(0x1000)
	{
		_idt = 0x1000;
		. = . * 0x100;
	}
	.rodata :
	{
		*(.rodata*);
	}
	.bss :
	{
		*(.bss*);
	}
	end = .;
}
