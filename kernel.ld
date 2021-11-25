SECTIONS {
	.text 0x7E00 :
	{
		*(kernel_entry);
		*(.text*);
	}
	.rodata :
	{
		*(.rodata*);
	}
	.bss :
	{
		*(.bss*);
	}
}
