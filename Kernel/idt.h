#ifndef IDT
#define IDT
//#include "util.h"

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

//idt _idt[256];
extern idt	_idt[256];
//extern uint32  isr1;
static idtr	_idtr;

void set_gate(uint8 num, void* base, uint8 flags)
{
	idt *_Idt = &_idt[num];
	_Idt->reserved		= 0x0;
	_Idt->low		= (uint32) base & 0xFFFF;
	_Idt->high		= ((uint32)base >> 16) & 0xFFFF;
	_Idt->kernel_cs		= 0x08;
	_Idt->attributes 	= flags | 0x60;
}

void syscall1()
{
    Print((uint8 *)"CALL ONE", WHITE, BLACK);
    //plot_px(10, 10, RED);
}
void syscall2()
{
    Print((uint8 *)"Call two", WHITE, BLACK);
    //plot_px(15, 15, BLUE);
}

void *syscalls[2] = {syscall1, syscall2};

__attribute__((naked))
void syscall_disp()
{
    /*int i;
    __asm__ __volatile__(
        "movl %%edx, %0" : "=r"(i)
    );
    PrintHex((uint32)i);*/
    __asm__ volatile (".intel_syntax noprefix\n"
        ".equ MAX_SYSCALLS, 2\n"
        "cmp eax, MAX_SYSCALLS-1\n"
        "ja invalid\n"
        "push eax\n"
        "push gs\n"
        "push fs\n"
        "push es\n"
        "push ds\n"
        "push ebp\n"
        "push edi\n"
        "push esi\n"
        "push edx\n"
        "push ecx\n"
        "push ebx\n"
        "push esp\n"
        "call [syscalls+eax*4]\n"
        "add esp, 4\n"
        "pop ebx\n"
        "pop ecx\n"
        "pop edx\n"
        "pop esi\n"
        "pop edi\n"
        "pop ebp\n"
        "pop ds\n"
        "pop es\n"
        "pop fs\n"
        "pop gs\n"
        "add esp, 4\n"
        "iretd\n"
        "invalid:\n"
        "iretd\n"
        ".att_syntax");
}

static void pic_eoi(uint8 current_vector) {
    if (current_vector >= 8) {
        PORT_OUT(0xa0, 0x20);
    }

    PORT_OUT(0x20, 0x20);
}

#define PIT_FREQUENCY_HZ 1000

static void init_pit(void) {
    uint16 x = 1193182 / PIT_FREQUENCY_HZ;
    if ((1193182 % PIT_FREQUENCY_HZ) > (PIT_FREQUENCY_HZ / 2))
        x++;

    PORT_OUT(0x40, (uint8)(x & 0x00ff));
    PORT_OUT(0x40, (uint8)((x & 0xff00) >> 8));
}

__attribute__ ((interrupt))
void div_by_z(interrupt_frame *frame)
{
	Print((uint8 *)"\n\tDivide By Zero Error\n\n", WHITE, BLACK);
	frame->ip++;
}

__attribute__((interrupt)) static void pit_handler(void *p) {
    ticks++;

    // refresh wm at 30 hz
    if (!(ticks % (PIT_FREQUENCY_HZ / 30))) {
		// Nothing
    }

	//PrintHex((uint32) ticks);
	//Print((uint8 *)"\n\n", WHITE, BLACK);

   pic_eoi(0);
}

__attribute__ ((interrupt))
void page_fault(interrupt_frame *frame, uint32 err_code)
{
	Print((uint8 *)"Page Fault", WHITE, BLACK);
	uint32 fault_addr;
	__asm volatile("movl %%cr2, %0":"=r"(fault_addr));

	__asm("cli; hlt");
}

__attribute__ ((interrupt))
void exception_handler_err(interrupt_frame *_interrupt, uint32 err_code)
{
	Print((uint8 *)"ERR CODE", WHITE, BLACK);
	PrintHex(err_code);
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

static int32 HANDLER_CYCLE = 0;
typedef struct {
    uint8 flags;
    uint8 x_mov;
    uint8 y_mov;
} mouse_packet_t;

static mouse_packet_t current_packet;
static uint8 discard = 0;

__attribute__ ((interrupt))
void mouse_handler(interrupt_frame *_interrupt)
{
    //PrintHex((uint32) _interrupt->flags);
    //__asm("hlt");
    if(ticks < 250)
    {
        current_packet.flags = PORT_IN(0x60);
        goto end;
        //PORT_IN(0x60);
    }
    
    switch(HANDLER_CYCLE) {
        case 0: {
            current_packet.flags = PORT_IN(0x60);
            //PrintHex((uint32) current_packet.flags);
            if(current_packet.flags & (1 << 6) || current_packet.flags & (1 << 7))
                discard = 1;
            if(!(current_packet.flags & (1 << 3)))
                discard = 1;
            HANDLER_CYCLE++;
            break;
        } 
        case 1: {
            current_packet.x_mov = PORT_IN(0x60);
            HANDLER_CYCLE++;
            break;
        }
        case 2: {
            current_packet.y_mov = PORT_IN(0x60);
            HANDLER_CYCLE = 0;
            if(discard==1)
            {
                discard = 0;
                break;
            }
        }
        default: break;
    }

    end:
    //PrintHex((uint32) current_packet.flags);
    PORT_IN(0x60);
    pic_eoi(12);
    //return;
    //pic_eoi(12);
    
    //__asm__ ("hlt");
    //pic_eoi(12);
    //Print((uint8 *)"MOUSE", WHITE, BLACK);
    //_interrupt->ip++;
}

__attribute__((interrupt))
void interrupt_03(interrupt_frame *_interrupt)
{
	uint8 good = 0x02;
	while(good & 0x02)
		good = PORT_IN(0x64);
	PORT_OUT(0x64, 0xFE);
	_interrupt->ip++;
	__asm("hlt");
}

__attribute__((interrupt)) static void unhandled_interrupt(void *p) {
    (void)p;
    asm volatile (
        "cli\n\t"
        "1: hlt\n\t"
        "jmp 1b\n\t"
    );
}

static void pic_set_mask(uint8 line, int status) {
    uint16 port;
    uint8 value;

    if (line < 8) {
        port = 0x21;
    } else {
        port = 0xa1;
        line -= 8;
    }

    if (!status)
        value = PORT_IN(port) & ~((uint8)1 << line);
    else
        value = PORT_IN(port) | ((uint8)1 << line);

    PORT_OUT(port, value);
}

static void pic_remap(uint8 pic0_offset, uint8 pic1_offset) {
    PORT_OUT(0x20, 0x11);
    PORT_OUT(0xa0, 0x11);
    PORT_OUT(0x21, pic0_offset);
    PORT_OUT(0xa1, pic1_offset);
    PORT_OUT(0x21, 4);
    PORT_OUT(0xa1, 2);
    PORT_OUT(0x21, 1);
    PORT_OUT(0xa1, 1);
    PORT_OUT(0x21, 0xff);
    PORT_OUT(0xa1, 0xff);
}

/*
 * TODO: Fill these two arrays out with interrupt/exception functions.
 *	For now, they are filled out with current exception/interrupt handlers.
 *	Would like to fill out with user-defined functions for each possible interrupt
 *	and exception.
 * */
static uint8 inerr_index = 0;
void *interrupts_no_err[24] = {
	div_by_z, exc_handler, exc_handler, interrupt_03,
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
	page_fault, exception_handler_err
};

void idt_init()
{
	
	//_idtr.limit = (sizeof(idt) * 256) - 1;
	_idtr.limit = (uint16) sizeof(_idt);
	_idtr.base = (uint32)&_idt;

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

    //set_gate(0x1, keyboard, 0x8E);
	set_gate(0x20, pit_handler, 0x8E);
    set_gate(0x20+12, mouse_handler, 0x8E);

	//set_gate(0x20, pit_handler, gate_interrupt);
    //set_gate(0x80, syscall_disp, 0xEE);

	__asm__ __volatile__("lidt %0" : : "m"(_idtr));
}

#endif
