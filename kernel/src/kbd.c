#include <kbd.h>
#include <console.h>
#include <pic.h>
#include <idt.h>
#include <io.h>
#include <string.h>
#include <param.h>
#include <klib.h>
#include <acpi.h>
#include <scancodes.h>
#include <vfs.h>
#include <devfs.h>
#include <sync.h>

#define DATA_PORT 		0x60
#define STATUS_REG		0x64
#define COMMAND_REG		0x64

#define OUTPUT_STATUS_BIT	0b1
#define INPUT_STATUS_BIT	0b10

static void wait_on_read() {
	while (!(in8(STATUS_REG) & OUTPUT_STATUS_BIT));
}

static void wait_on_write() {
	while (in8(STATUS_REG) & INPUT_STATUS_BIT);
}

static uint8_t read_data() {
	wait_on_read();
	return in8(DATA_PORT);
}

static void write_data(uint8_t byte) {
	wait_on_write();
	out8(DATA_PORT, byte);
}

#define KBD_BUFF_SIZE	4096

static uint8_t 	kbd_buffer[KBD_BUFF_SIZE];
static int 		kbd_buff_index = 0;

static spinlock_t kbd_lock;

static void kbd_isr(trapframe_t *tf) {
	int scancode = read_data();
	kprintfln("kbd event: %x", scancode);

	/*
	key_event e;
	get_key(scancode, &e);

	if (e.event == E_MAKE) {
		char c = get_ascii(e.key);
		if (c != 0)
			kprintf("%c", c);
	}
	*/
}

#define CMD_SCAN_CODE		0xf0
#define GET_SCAN_CODE		0
#define CMD_DISABLE_1ST		0xad
#define CMD_ENABLE_1ST		0xae
#define CMD_DISABLE_2ND		0xa7
#define CMD_READ_CONF		0x20
#define CMD_WRITE_CONF		0x60
#define CMD_TEST_PS2		0xaa
#define PS2_PASS			0x55
#define CMD_TEST_PORT_1		0xab
#define PORT_1_PASS			0

#define KBD_ACK 	0xfa
#define KBD_RES		0xfe

static void command(int code) {
	wait_on_write();
	out8(COMMAND_REG, code);
}

void init_kbd() {
	kprintfln("init kbd");

	//init mutex
	spinlock_init(&kbd_lock);

	//disable controllers
	command(CMD_DISABLE_1ST);
	command(CMD_DISABLE_2ND);

	//flush out buffer
	in8(DATA_PORT);

	//read config byte
	command(CMD_READ_CONF);
	uint8_t conf = read_data();
	kprintfln("kbd conf: %b", conf);
	conf &= !0b00100011; //disable INTs and translation
	command(CMD_WRITE_CONF);
	write_data(conf);

	//test controller
	command(CMD_TEST_PS2);
	uint8_t test = read_data();
	kprintfln("ps/2 test: %x", test);

	if (test != PS2_PASS)
		panic("ps/2 controller failed test!");

	//test first port
	command(CMD_TEST_PORT_1);
	test = read_data();
	kprintfln("port 1 test: %x", test);

	if (test != PORT_1_PASS)
		panic("ps/2 port 1 failed test!");

	//enable first port and INTs
	command(CMD_READ_CONF);
	conf = read_data();
	conf |= 0b1; //enable INTs for 1st port
	command(CMD_WRITE_CONF);
	write_data(conf);
	command(CMD_ENABLE_1ST);

	//assume scan code set 2

	//set kbd isr
	set_isr(IRQ_BASE + IRQ_KBD, kbd_isr);

	//enable kbd
	uint8_t mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << IRQ_KBD);
	pic_write_data(PIC_MASTER, mask);
}
