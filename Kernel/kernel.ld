SECTIONS {
	.text 0xF000 :
	{
		*(kernel_entry);
		*(.text*);
	}
	.idt BLOCK(.) : ALIGN(.)
	{
		_idt = 0x1000;
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
