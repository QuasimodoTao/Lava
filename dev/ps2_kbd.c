/*
	dev/ps2_kbd.c
	Copyright (C) 2020  Quasimodo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stddef.h>
#include <keyboard.h>
#include <asm.h>
#include <int.h>
#include <semaphore.h>
#include <fs.h>
#include <kernel.h>
#include <gst.h>
#include <mm.h>
#include <eisa.h>
#include <fctrl.h>

#define KEY_CAPS_LOCK_DOWN		1
#define KEY_NUM_LOCK_DOWN		2
#define KEY_SCROLL_LOCK_DOWN	4

#define FIFO_SIZE	128


static u8 kbd_sc2asc_ns[] = {
	0,KEY_ESC,'1','2','3','4','5','6','7','8','9','0','-','=',KEY_BACK_SPACE,
	'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
	KEY_LEFT_CTRL,'a','s','d','f','g','h','j','k','l',';','\'','`',
	KEY_LEFT_SHIFT,'\\','z','x','c','v','b','n','m',',','.','/',
	KEY_RIGHT_SHIFT,'*',KEY_LEFT_ALT,' ',KEY_CAPS_LOCK,KEY_F1,KEY_F2,KEY_F3,
	KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NUM_LOCK,
	KEY_SCROLL_LOCK,'7','8','9','-','4','5','6','+','1','2','3','0','.',
	KEY_F11,KEY_F12
};
static u8 kbd_sc2asc_s[] = {
	0,KEY_ESC,'!','@','#','$','%','^','&','*','(',')','_','+',KEY_BACK_SPACE,
	'\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
	KEY_LEFT_CTRL,'A','S','D','F','G','H','J','K','L',':','\"','~',
	KEY_LEFT_SHIFT,'|','Z','X','C','V','B','N','M','<','>','?',
	KEY_RIGHT_SHIFT,'*',KEY_LEFT_ALT,' ',KEY_CAPS_LOCK,KEY_F1,KEY_F2,KEY_F3,
	KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NUM_LOCK,
	KEY_SCROLL_LOCK,'7','8','9','-','4','5','6','+','1','2','3','0','.',
	KEY_F11,KEY_F12
};
static u8 kbd_sc2asc_e[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	KEY_PREV_TRACK,0,0,0,0,0,0,0,0,KEY_NEXT_TRACK,0,
	'\n',KEY_RIGHT_CTRL,0,0,
	KEY_MUTE,KEY_CACULATOR,KEY_PLAY,0,KEY_STOP,0,0,0,0,0,0,0,0,0,
	KEY_VOL_DOWN,0,KEY_VOL_UP,0,KEY_WWW_HOME,0,0,'/',0,0,KEY_RIGHT_ALT,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,KEY_HOME,KEY_UP,KEY_PAGE_UP,0,
	KEY_LEFT,0,KEY_RIGHT,0,KEY_END,KEY_DOWN,KEY_PAGE_DOWN,KEY_INSERT,
	KEY_DELETE,0,0,0,0,0,0,0,KEY_LEFT_GUI,KEY_RIGHT_GUI,KEY_APP,KEY_POWER,
	KEY_SLEEP,0,0,0,KEY_WAKE,0,KEY_WWW_SEARCH,KEY_WWW_FAVORATE,
	KEY_WWW_REFRESH,KEY_WWW_STOP,KEY_WWW_FORWARD,KEY_WWW_BACK,KEY_MY_COMPUTER,
	KEY_EMAIL,KEY_MEDIA_SLELECT
};
static u16 status;

static u16 fifo[FIFO_SIZE];
static u8 head;
static u8 tail;
static SEMAPHORE semaphore;
static LPTHREAD wait_thread;
static int int_arise;
static wchar_t * path = L".dev/ps2_kbd.dev";

static void istream(int code){
	u64 rf;

	SFI(rf);
	fifo[tail] = code;
	tail++;
	tail %= FIFO_SIZE;
	LF(rf);
	release_semaphore(1,&semaphore,0);
}
static int ps2_kbd_handle(int irq){
	u8 tmp;
	u8 loosen = 0;
	u16 code;
	static int pos = 0;
	static u8 scan_code[8];
	static u8 status2 = 0;
	
	tmp = inb(EISA_KBD_DATA_PORT);
	
	if(!pos){
		if(tmp == 0xe0 || tmp == 0xe1){
			scan_code[0] = tmp;
			pos++;
			return 0;
		}
		loosen = tmp & 0x80;
		tmp &= 0x7f;
		if(tmp > 0x58) return 0;
		if(status & KEY_S_SHIFT_DOWN){
			code = kbd_sc2asc_s[tmp];
			if(code >= 'A' && code <= 'Z' && status & KEY_S_CAPS_LOCK)
				code = kbd_sc2asc_ns[tmp];
		}
		else{
			code = kbd_sc2asc_ns[tmp];
			if(code >= 'a' && code <= 'z' && status & KEY_S_CAPS_LOCK)
				code = kbd_sc2asc_s[tmp];
		}
		if(!loosen){
			if(code == KEY_LEFT_CTRL) status |= KEY_S_CTRL_DOWN;
			else if(code == KEY_LEFT_SHIFT)	status |= KEY_S_SHIFT_DOWN;
			else if(code == KEY_RIGHT_SHIFT) status |= KEY_S_SHIFT_DOWN;
			else if(code == KEY_LEFT_ALT) status |= KEY_S_ALT_DOWN;
			else if(code == KEY_CAPS_LOCK && !(status2 & KEY_CAPS_LOCK_DOWN)) {
				status ^= KEY_S_CAPS_LOCK;
				status2 |= KEY_CAPS_LOCK_DOWN;
			}
			else if(code == KEY_NUM_LOCK && !(status2 & KEY_NUM_LOCK_DOWN)) {
				status ^= KEY_S_NUM_LOCK;
				status2 |= KEY_NUM_LOCK_DOWN;
			}
			else if(code == KEY_SCROLL_LOCK && !(status2 & KEY_SCROLL_LOCK_DOWN)) {
				status ^= KEY_S_SCROL_LOCK;
				status2 |= KEY_SCROLL_LOCK_DOWN;
			}
		}
		else{
			if(code == KEY_LEFT_CTRL) status &= ~KEY_S_CTRL_DOWN;
			else if(code == KEY_LEFT_SHIFT)	status &= ~KEY_S_SHIFT_DOWN;
			else if(code == KEY_RIGHT_SHIFT) status &= ~KEY_S_SHIFT_DOWN;
			else if(code == KEY_LEFT_ALT) status &= ~KEY_S_ALT_DOWN;
			else if(code == KEY_CAPS_LOCK) status2 &= ~KEY_CAPS_LOCK_DOWN;
			else if(code == KEY_NUM_LOCK) status2 &= ~KEY_NUM_LOCK_DOWN;
			else if(code == KEY_SCROLL_LOCK) status2 &= ~KEY_SCROLL_LOCK;
		}
		if(loosen) code |= KEY_S_LOOSEN;
		if(tmp == 0x37 || (tmp >= 0x47 && tmp <= 0x53)) {
			if(!(status & KEY_S_NUM_LOCK))
				switch(code){
				case '7':code = KEY_HOME;break;
				case '8':code = KEY_UP;break;
				case '9':code = KEY_PAGE_UP;break;
				case '4':code = KEY_LEFT;break;
				case '6':code = KEY_RIGHT;break;
				case '1':code = KEY_END;break;
				case '2':code = KEY_DOWN;break;
				case '3':code = KEY_PAGE_DOWN;break;
				case '0':code = KEY_INSERT;break;
				case '.':code = KEY_DELETE;break;
				}
			code |= KEY_S_PAD;
		}
		code |= status;
		istream(code);
		return 0;
	}
	if(pos == 1){
		if(scan_code[0] == 0xe0){
			if(tmp & 0x7f == 0x2a){
				scan_code[1] = tmp;
				pos++;
			}
			else {
				loosen = tmp & 0x80;
				tmp &= 0x7f;
				if(!loosen){
					if(code == KEY_RIGHT_CTRL) status |= KEY_S_CTRL_DOWN;
					else if(code == KEY_RIGHT_ALT) status |= KEY_S_ALT_DOWN;
				}
				else {
					if(code == KEY_RIGHT_CTRL) status &= ~KEY_S_CTRL_DOWN;
					else if(code == KEY_RIGHT_ALT) status &= ~KEY_S_ALT_DOWN;
				}
				code = kbd_sc2asc_e[tmp];
				if(loosen) code |= KEY_S_LOOSEN;
				if(tmp == 0x35) code |= KEY_S_PAD;
				istream(code);
				pos = 0;
			}
			return 0;
		}
		if(scan_code[0] == 0xe1){
			scan_code[1] = tmp;
			pos++;
			return 0;
		}
	}
	if(pos == 3 && scan_code[0] == 0xe0 && scan_code[2] == 0xe0 && 
		scan_code[1] & 0x7f == 0x2a && tmp & 0x7f == 0x37){
		code = status | KEY_PRINT_SCREEN;
		if(tmp & 0x80) code |= KEY_S_LOOSEN;
		istream(code);
		pos = 0;
		return 0;
	}
	if(pos == 5 && scan_code[0] == 0xe1 && scan_code[1] == 0x1d &&
		scan_code[2] == 0x45 && scan_code[3] == 0xe1 &&
		scan_code[4] == 0x9d && tmp == 0xc5){
		istream(status | KEY_PAUSE);
		pos = 0;
		return 0;
	}
	scan_code[pos] = tmp;
	pos++;
	if(pos >= 6) pos = 0;
	return 0;
}

static LPSTREAM ps2_kbd_open(wchar_t * name,u64 mode,struct _FCPEB_ * fc){
	LPSTREAM file;
	
	file = kmalloc(sizeof(STREAM),0);
	memset(file,0,sizeof(STREAM));
	file->gst = GST_FILE;
	file->fc = fc;
	return file;
}
static int ps2_kbd_close(LPSTREAM file){
	kfree(file);
	return 0;
}
static int ps2_kbd_get(LPSTREAM file){
	int res;
	
	wait_semaphore(1,&semaphore,-1);
	res = fifo[head];
	head++;
	head %= FIFO_SIZE;
	return res;
}
static int ps2_kbd_fresh(LPSTREAM file){
	while(wait_semaphore(1,&semaphore,0));
	return 0;
}
static int ps2_kbd_get_size(LPSTREAM file){
	return 2;
}

static struct _FCPEB_ ps2_kbd_fc = {
	.in = NULL,
	.out = NULL,
	.open = ps2_kbd_open,
	.close = ps2_kbd_close,
	.get = ps2_kbd_get,
	.fresh_get = ps2_kbd_fresh,
	.fresh_put = ps2_kbd_fresh
};

void ps2_kbd_init(){
	u64 rf;
	
	SFI(rf);
	while(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_IN_BUF_FULL) pause();
	outb(EISA_KBD_CMD_STATUS_PORT,EISA_8042_KDB_ENABLE);
	while(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_IN_BUF_FULL) pause();
	outb(EISA_KBD_DATA_PORT,0x00);
	while(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_IN_BUF_FULL) pause();
	outb(EISA_KBD_DATA_PORT,EISA_8042_SET_LED);
	while(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_IN_BUF_FULL) pause();
	outb(EISA_KBD_DATA_PORT,0x00);
	while(!(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_OUT_BUF_FULL)) pause();
	inb(EISA_KBD_DATA_PORT);
	request_irq(EISA_KEYBOARD_IRQ,ps2_kbd_handle);
	while(!(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_OUT_BUF_FULL)) pause();
	inb(EISA_KBD_DATA_PORT);
	while(!(inb(EISA_KBD_CMD_STATUS_PORT) & EISA_8042_OUT_BUF_FULL)) pause();
	inb(EISA_KBD_DATA_PORT);
	LF(rf);
	head = tail = 0;
	create_semaphore_ex(FIFO_SIZE,0,&semaphore);
	fs_map(path,&ps2_kbd_fc,NULL);
	irq_enable(EISA_KEYBOARD_IRQ);
}

