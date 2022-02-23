#ifndef IDT
#define IDT

#define gate_interrupt	0x8E
#define	gate_trap	0x8F

typedef struct
{
	uint16		low;
	uint16		kernel_cs;
	uint8		reserved;
	uint8		attributes;
	uint16		high;
} __attribute__((packed)) idt;

typedef struct
{
	uint16	limit;
	uint32	base;
} __attribute__((packed)) idtr;

typedef struct
{
	uint32	ip;
	uint32	cs;
	uint32	flags;
	uint32	sp;
	uint32	ss;
} __attribute__((packed)) interrupt_frame;

static idt _idt[256];
//extern idt	_idt[256];
//extern uint32  isr1;
static idtr	_idtr;

void set_gate(uint8 num, void* base, uint8 flags)
{
	idt *_Idt = &_idt[num];
	_Idt->reserved		= 0x0;
	_Idt->low		= (uint32) base & 0xFFFF;
	_Idt->high		= ((uint32)base >> 16) & 0xFFFF;
	_Idt->kernel_cs		= 0x08;
	_Idt->attributes 	= flags;// | 0x60;
}

/*
 * Function wrapper for each interrupt.
 * */
__attribute__ ((interrupt))
void exception_handler_err(interrupt_frame *_interrupt, uint32 err_code)
{
	Print((uint8 *)"ERR CODE", WHITE, BLACK);
	__asm("cli; hlt");
	//if(err_code == 0xFFFC)
	//	__asm("int $0x3");
}
__attribute__ ((interrupt))
void exc_handler(interrupt_frame *_interrupt)
{
	//PrintHex(_interrupt->ip, MakeColor(255, 255, 255), MakeColor(0, 0, 0));
	Print((uint8 *)"NO ERR CODE", WHITE, BLACK);
	__asm("cli; hlt");
}
__attribute__ ((interrupt))
void int_handler(interrupt_frame *_interrupt)
{
	Print((uint8 *)"Interrupt", WHITE, BLACK);
	__asm("cli; hlt");
}

__attribute__((interrupt))
void interrupt_03(interrupt_frame *_interrupt)
{
	uint8 good = 0x02;
	while(good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	_interrupt->ip++;
	__asm("hlt");
}

/*
 * TODO: Fill these two arrays out with interrupt/exception functions.
 *	For now, they are filled out with current exception/interrupt handlers.
 *	Would like to fill out with user-defined functions for each possible interrupt
 *	and exception.
 * */
static uint8 inerr_index = 0;
void *interrupts_no_err[24] = {
	exc_handler, exc_handler, exc_handler, interrupt_03,
	exc_handler, exc_handler, exc_handler, exc_handler,
	exc_handler, exc_handler, exc_handler, exc_handler,
	exc_handler, exc_handler, exc_handler, exc_handler,
	exc_handler, exc_handler, exc_handler, exc_handler,
	exc_handler, exc_handler, exc_handler, exc_handler
};

static uint8 iwerr_index = 0;
void *interrupts_with_err[8] = {
	exception_handler_err, exception_handler_err, 
	exception_handler_err, exception_handler_err,
	exception_handler_err, exception_handler_err, 
	exception_handler_err, exception_handler_err
};

void idt_init()
{
	
	//_idtr.limit = (sizeof(idt) * 256) - 1;
	_idtr.limit = (uint16) sizeof(_idt);
	_idtr.base = (uint32)&_idt;

	//memset(&_idt, 0, sizeof(idt) * 256);

	for(uint8 i = 0; i < 32; i++)
	{
		if(i == 8 || i == 10 || i == 11 || i == 12 ||
		   i == 13 || i == 14 || i == 17 || i == 21)
		{
			set_gate(i, interrupts_with_err[iwerr_index], gate_trap);
			iwerr_index++;
		}
		else
		{
			set_gate(i, interrupts_no_err[inerr_index], gate_trap);
			inerr_index++;
		}
	}

	for(uint16 i = 32; i < 256; i++)
	{
		set_gate(i, int_handler, gate_interrupt);
	}

	__asm__ __volatile__("lidt %0" : : "memory"(_idtr));
	//__asm__ __volatile__("sti");
}

#endif
