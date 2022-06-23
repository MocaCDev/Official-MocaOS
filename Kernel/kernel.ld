SECTIONS {
	.text 0x1F00 :
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
