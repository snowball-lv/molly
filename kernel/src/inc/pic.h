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

void init_pic();

uint8_t pic_read_data(uint8_t pic);

void pic_write_data(uint8_t pic, uint8_t data);

void pic_eoi(uint8_t pic);

uint8_t pic_isr(uint8_t pic);















