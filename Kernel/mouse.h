#ifndef MocaPIT
#define MocaPIT

#include "util.h"

static void mouse_wait(int32 type) {
    int32 timeout = 100000;

    if (type == 0) {
        while (timeout--) {
            if (PORT_IN(0x64) & (1 << 0)) {
                return;
            }
        }
    } else {
        while (timeout--) {
            if (!(PORT_IN(0x64) & (1 << 1))) {
                return;
            }
        }
    }
}

static void mouse_write(uint8 val) {
    mouse_wait(1);
    PORT_OUT(0x64, 0xd4);
    mouse_wait(1);
    PORT_OUT(0x60, val);
}

static uint8 mouse_read(void) {
    mouse_wait(0);
    return PORT_IN(0x60);
}

static void init_mouse(void) {
    mouse_wait(1);
    PORT_OUT(0x64, 0xa8);

    mouse_wait(1);
    PORT_OUT(0x64, 0x20);
    uint8 status = mouse_read();
    mouse_read();
    status |= (1 << 1);
    status &= ~(1 << 5);
    mouse_wait(1);
    PORT_OUT(0x64, 0x60);
    mouse_wait(1);
    PORT_OUT(0x60, status);
    mouse_read();

    mouse_write(0xff);
    mouse_read();

    mouse_write(0xf6);
    mouse_read();

    mouse_write(0xf4);
    mouse_read();
}

#endif