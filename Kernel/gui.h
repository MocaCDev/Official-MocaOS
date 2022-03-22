#ifndef Moca_GUI
#define Moca_GUI

typedef struct MocaGUI
{
	uint32		*last_fb;
} _MocaGUI;

_MocaGUI *init_gui()
{
	/* Get physical memory region for GUI. */
	MProcess p = m_alloc(2);
	_MocaGUI *gui = (_MocaGUI *)p.address;

	return gui;
}

void put_pixel(uint32 x, uint32 y, uint32 color)
{
	uint32 *fb = (uint32 *)FB;

	tc.cursor_x += x;
	tc.cursor_y = 0;

	fb += (tc.cursor_x * 8) + (tc.cursor_y * 16 * WIDTH);
	
	uint32 i = 0;
	//while(i < 15)
	//{
		*fb = color;
		fb++;
	//}
	
}

#endif
