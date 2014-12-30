#pragma once

#include <types.h>

#define PIC_MASTER		0
#define PIC_SLAVE		1

#define IRQ_BASE 		32

#define PIC_IRQ_TIMER	0
#define PIC_IRQ_KBD		1
#define PIC_IRQ_KBD		1
#define PIC_IRQ_KBD		1

#define IRQ_SPURIOUS_MASTER		7
#define IRQ_SPURIOUS_SLAVE		15

void initPIC();

u8 pic_read_data(u8 pic);

void pic_write_data(u8 pic, u8 data);

void pic_eoi(u8 pic);

u8 pic_isr(u8 pic);















