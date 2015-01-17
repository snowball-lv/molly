#include <isr.h>
#include <console.h>
#include <types.h>
#include <pic.h>

static void *_ISRS[MAX_INTERRUPTS] = {0};

void set_isr(word id, void *handler) {
	_ISRS[id] = handler;
}

typedef void (*isr_handler)(word id);
typedef void (*isr_handler2)(word id, uint32_t error);

void default_isr_irq_master(word id) {

	//check if IRQ is spurious
	if (id == IRQ_BASE + IRQ_SPURIOUS_MASTER) {
		if (pic_isr(PIC_MASTER) & IRQ_SPURIOUS_MASTER) {
			//legit
			printfln("*** legit spurious master irq!");
		} else {
			//spurious
			printfln("*** spurious master irq!");
			return;
		}
	}
	
	isr_handler handler = _ISRS[id];
	if (handler) {
		handler(id);
		pic_eoi(PIC_MASTER);
		return;
	}
	
	printfln("*** unhandled irq master exception! %d", id);
	__asm__("hlt");
}

void default_isr_irq_slave(word id) {

	//check if IRQ is spurious
	if (id == IRQ_BASE + IRQ_SPURIOUS_SLAVE) {
		if (pic_isr(PIC_SLAVE) & (IRQ_SPURIOUS_SLAVE >> 8)) {
			//legit
			printfln("*** legit spurious slave irq!");
		} else {
			//spurious
			printfln("*** spurious slave irq!");
			return;
		}
	}
	
	isr_handler handler = _ISRS[id];
	if (handler) {
		handler(id);
		pic_eoi(PIC_SLAVE);
		return;
	}
	
	printfln("*** unhandled irq slave exception! %d", id);
	__asm__("hlt");
}

void default_isr(word id) {

	isr_handler handler = _ISRS[id];
	if (handler) {
		handler(id);
		return;
	}
	
	printfln("*** unhandled exception! %d", id);
	__asm__("hlt");
}

void default_isr_error(word id, uint32_t error) {

	isr_handler2 handler = _ISRS[id];
	if (handler) {
		handler(id, error);
		return;
	}
	
	printfln("*** unhandled exception! %d, %x", id, error);
	__asm__("hlt");
}

#define DEF(x) 	void isr_wrapper##x()
#define NAME(x)	isr_wrapper##x

DEF(0);		DEF(1);		DEF(2);		DEF(3);	
DEF(4);		DEF(5);		DEF(6);		DEF(7);	
DEF(8);		DEF(9);		DEF(10);	DEF(11);	
DEF(12);	DEF(13);	DEF(14);	DEF(15);	

DEF(16);	DEF(17);	DEF(18);	DEF(19);	
DEF(20);	DEF(21);	DEF(22);	DEF(23);	
DEF(24);	DEF(25);	DEF(26);	DEF(27);	
DEF(28);	DEF(29);	DEF(30);	DEF(31);	

DEF(32);	DEF(33);	DEF(34);	DEF(35);	
DEF(36);	DEF(37);	DEF(38);	DEF(39);	
DEF(40);	DEF(41);	DEF(42);	DEF(43);	
DEF(44);	DEF(45);	DEF(46);	DEF(47);	

DEF(48);	DEF(49);	DEF(50);	DEF(51);	
DEF(52);	DEF(53);	DEF(54);	DEF(55);	
DEF(56);	DEF(57);	DEF(58);	DEF(59);	
DEF(60);	DEF(61);	DEF(62);	DEF(63);	

DEF(64);	DEF(65);	DEF(66);	DEF(67);	
DEF(68);	DEF(69);	DEF(70);	DEF(71);	
DEF(72);	DEF(73);	DEF(74);	DEF(75);	
DEF(76);	DEF(77);	DEF(78);	DEF(79);	

DEF(80);	DEF(81);	DEF(82);	DEF(83);	
DEF(84);	DEF(85);	DEF(86);	DEF(87);	
DEF(88);	DEF(89);	DEF(90);	DEF(91);	
DEF(92);	DEF(93);	DEF(94);	DEF(95);	

DEF(96);	DEF(97);	DEF(98);	DEF(99);	
DEF(100);	DEF(101);	DEF(102);	DEF(103);	
DEF(104);	DEF(105);	DEF(106);	DEF(107);	
DEF(108);	DEF(109);	DEF(110);	DEF(111);	

DEF(112);	DEF(113);	DEF(114);	DEF(115);	
DEF(116);	DEF(117);	DEF(118);	DEF(119);	
DEF(120);	DEF(121);	DEF(122);	DEF(123);	
DEF(124);	DEF(125);	DEF(126);	DEF(127);	

DEF(128);	DEF(129);	DEF(130);	DEF(131);	
DEF(132);	DEF(133);	DEF(134);	DEF(135);	
DEF(136);	DEF(137);	DEF(138);	DEF(139);	
DEF(140);	DEF(141);	DEF(142);	DEF(143);	

DEF(144);	DEF(145);	DEF(146);	DEF(147);	
DEF(148);	DEF(149);	DEF(150);	DEF(151);	
DEF(152);	DEF(153);	DEF(154);	DEF(155);	
DEF(156);	DEF(157);	DEF(158);	DEF(159);	

DEF(160);	DEF(161);	DEF(162);	DEF(163);	
DEF(164);	DEF(165);	DEF(166);	DEF(167);	
DEF(168);	DEF(169);	DEF(170);	DEF(171);	
DEF(172);	DEF(173);	DEF(174);	DEF(175);	

DEF(176);	DEF(177);	DEF(178);	DEF(179);	
DEF(180);	DEF(181);	DEF(182);	DEF(183);	
DEF(184);	DEF(185);	DEF(186);	DEF(187);	
DEF(188);	DEF(189);	DEF(190);	DEF(191);	

DEF(192);	DEF(193);	DEF(194);	DEF(195);	
DEF(196);	DEF(197);	DEF(198);	DEF(199);	
DEF(200);	DEF(201);	DEF(202);	DEF(203);	
DEF(204);	DEF(205);	DEF(206);	DEF(207);	

DEF(208);	DEF(209);	DEF(210);	DEF(211);	
DEF(212);	DEF(213);	DEF(214);	DEF(215);	
DEF(216);	DEF(217);	DEF(218);	DEF(219);	
DEF(220);	DEF(221);	DEF(222);	DEF(223);	

DEF(224);	DEF(225);	DEF(226);	DEF(227);	
DEF(228);	DEF(229);	DEF(230);	DEF(231);	
DEF(232);	DEF(233);	DEF(234);	DEF(235);	
DEF(236);	DEF(237);	DEF(238);	DEF(239);	

DEF(240);	DEF(241);	DEF(242);	DEF(243);	
DEF(244);	DEF(245);	DEF(246);	DEF(247);	
DEF(248);	DEF(249);	DEF(250);	DEF(251);	
DEF(252);	DEF(253);	DEF(254);	DEF(255);	

typedef void (*fptr)();

fptr _ISR_WRAPPERS[MAX_INTERRUPTS] = {
	NAME(0),	NAME(1),	NAME(2),	NAME(3),
	NAME(4),	NAME(5),	NAME(6),	NAME(7),
	NAME(8),	NAME(9),	NAME(10),	NAME(11),
	NAME(12),	NAME(13),	NAME(14),	NAME(15),

	NAME(16),	NAME(17),	NAME(18),	NAME(19),
	NAME(20),	NAME(21),	NAME(22),	NAME(23),
	NAME(24),	NAME(25),	NAME(26),	NAME(27),
	NAME(28),	NAME(29),	NAME(30),	NAME(31),

	NAME(32),	NAME(33),	NAME(34),	NAME(35),
	NAME(36),	NAME(37),	NAME(38),	NAME(39),
	NAME(40),	NAME(41),	NAME(42),	NAME(43),
	NAME(44),	NAME(45),	NAME(46),	NAME(47),

	NAME(48),	NAME(49),	NAME(50),	NAME(51),
	NAME(52),	NAME(53),	NAME(54),	NAME(55),
	NAME(56),	NAME(57),	NAME(58),	NAME(59),
	NAME(60),	NAME(61),	NAME(62),	NAME(63),

	NAME(64),	NAME(65),	NAME(66),	NAME(67),
	NAME(68),	NAME(69),	NAME(70),	NAME(71),
	NAME(72),	NAME(73),	NAME(74),	NAME(75),
	NAME(76),	NAME(77),	NAME(78),	NAME(79),

	NAME(80),	NAME(81),	NAME(82),	NAME(83),
	NAME(84),	NAME(85),	NAME(86),	NAME(87),
	NAME(88),	NAME(89),	NAME(90),	NAME(91),
	NAME(92),	NAME(93),	NAME(94),	NAME(95),

	NAME(96),	NAME(97),	NAME(98),	NAME(99),
	NAME(100),	NAME(101),	NAME(102),	NAME(103),
	NAME(104),	NAME(105),	NAME(106),	NAME(107),
	NAME(108),	NAME(109),	NAME(110),	NAME(111),

	NAME(112),	NAME(113),	NAME(114),	NAME(115),
	NAME(116),	NAME(117),	NAME(118),	NAME(119),
	NAME(120),	NAME(121),	NAME(122),	NAME(123),
	NAME(124),	NAME(125),	NAME(126),	NAME(127),

	NAME(128),	NAME(129),	NAME(130),	NAME(131),
	NAME(132),	NAME(133),	NAME(134),	NAME(135),
	NAME(136),	NAME(137),	NAME(138),	NAME(139),
	NAME(140),	NAME(141),	NAME(142),	NAME(143),

	NAME(144),	NAME(145),	NAME(146),	NAME(147),
	NAME(148),	NAME(149),	NAME(150),	NAME(151),
	NAME(152),	NAME(153),	NAME(154),	NAME(155),
	NAME(156),	NAME(157),	NAME(158),	NAME(159),

	NAME(160),	NAME(161),	NAME(162),	NAME(163),
	NAME(164),	NAME(165),	NAME(166),	NAME(167),
	NAME(168),	NAME(169),	NAME(170),	NAME(171),
	NAME(172),	NAME(173),	NAME(174),	NAME(175),

	NAME(176),	NAME(177),	NAME(178),	NAME(179),
	NAME(180),	NAME(181),	NAME(182),	NAME(183),
	NAME(184),	NAME(185),	NAME(186),	NAME(187),
	NAME(188),	NAME(189),	NAME(190),	NAME(191),

	NAME(192),	NAME(193),	NAME(194),	NAME(195),
	NAME(196),	NAME(197),	NAME(198),	NAME(199),
	NAME(200),	NAME(201),	NAME(202),	NAME(203),
	NAME(204),	NAME(205),	NAME(206),	NAME(207),

	NAME(208),	NAME(209),	NAME(210),	NAME(211),
	NAME(212),	NAME(213),	NAME(214),	NAME(215),
	NAME(216),	NAME(217),	NAME(218),	NAME(219),
	NAME(220),	NAME(221),	NAME(222),	NAME(223),

	NAME(224),	NAME(225),	NAME(226),	NAME(227),
	NAME(228),	NAME(229),	NAME(230),	NAME(231),
	NAME(232),	NAME(233),	NAME(234),	NAME(235),
	NAME(236),	NAME(237),	NAME(238),	NAME(239),

	NAME(240),	NAME(241),	NAME(242),	NAME(243),
	NAME(244),	NAME(245),	NAME(246),	NAME(247),
	NAME(248),	NAME(249),	NAME(250),	NAME(251),
	NAME(252),	NAME(253),	NAME(254),	NAME(255)
};














