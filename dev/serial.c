/*
	dev/serial.c
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

#include <int.h>
#include <lava.h>
#include <stddef.h>
#include <asm.h>
#include <kernel.h>
#include <mm.h>
#include <gst.h>
#include <semaphore.h>
#include <error.h>
#include <spinlock.h>
#include <stdio.h>
/*

115200 Hz
Div	Bps
1	115200
2	57600
3	38400
4	28800
5	23040
6	19200
8	14400
9	12800
10	11520
12	9600
15	7680
16	7200
18	6400
20	5760
24	4800
25	4608
30	3840
32	3600
26	3200
40	2880
48	2400
50	2304

COM1	0x3f8
COM2	0x2f8
COM3	0x3e8
COM4	0x2e8

reg0:data register/Low byte
reg1:interrupt enable//High byte
	0:Enable Receiver Buffer Full Interrupt
	1:Enable Transmitter Buffer Empty Interrupt
	2:Enable Line status Interrupt
	3:Enable Delta status Signals Interrupt
reg2:Interrupt identification(ro)
	0:Pending
	3-1:Interrupt IDentification
	7-6:FIFO enable
reg2:FIFO control(wo)
	0:Enable
	1:Receiver FIFO reset
	2:Transmitter FIFO reset
	3:DMA mode select
	7-6:RX FIFO trigger level select
reg3:Line control reg
	1-0:word length
		0:5 bits
		1:6 bits
		2:7 bits
		3:8 bits
	2:Stop bits
		0:1 bits
		1:1.5/2 bits
	3:Parity enable
	5-4:Parity
		0:ODD
		1:EVEN
		2:MARK
		3:SPACE
	6:Set break
	7:Divisor Latch Access
reg4:Modem control
	0:DTR
	1:RTS
	2:OUT1
	3:OUT2
	4:Loopback
reg5:Line status
	0:Receiver Buffer Full (Data Available)
	1:Overrun Error
	2:Parity Error
	3:Framing Error
	4:Broken line detected
	5:Transmitter Holding Register Empty (new data can be written to THR)
	6:Transmitter Empty (last word has been sent)
	7:At least one error is pending in the RX FIFO chain
reg6:Modem status
	0:Delta Clear To Send
	1:Delta Data Set Ready
	2:Trailing Edge Ring Indicator
	3:Delta Data Carrier Detect
	4:Clear To Send
	5:Data Set Ready
	6:Ring Indicator
	7:Data Carrier Detect
reg7:scratch


*/

#define SERIAL_DATA			0
#define SERIAL_DIV_LOW		0
#define SERIAL_IE			1
#define SERIAL_DIV_HIGH		1
#define SERIAL_INT_ID		2
#define SERIAL_FIFO_CTRL	2
#define SERIAL_LINE_CTRL	3
#define SERIAL_MODEM_CTRL	4
#define SERIAL_LINE_STATUS	5
#define SERIAL_MODEM_STATUS	6

#define SERIAL_WORD_LEN_5	0x00
#define SERIAL_WORD_LEN_6	0x01
#define SERIAL_WORD_LEN_7	0x02
#define SERIAL_WORD_LEN_8	0x03
#define SERIAL_STOP_LEN_1	0x00
#define SERIAL_STOP_LEN_2	0x04
#define SERIAL_PARITY_EN	0x08
#define SERIAL_PARITY_ODD	0x00
#define SERIAL_PARITY_EVEN	0x10
#define SERIAL_PARITY_MARK	0x20
#define SERIAL_PARITY_SPACE	0x30
#define SERIAL_SET_BACK		0x40
#define SERIAL_DLA			0x80

#define SERIAL_FIFO_SIZE	0x800
#define SERIAL_COM_PATH_LEN	24
#define SERIAL_COM_WAIT_TIME	1000

#define PORT_SENDING_BIT	1
#define PORT_BUSY_BIT		0

#define PORT_BUSY			(1 << PORT_BUSY_BIT)
#define PORT_SENDING		(1 << PORT_SENDING_BIT)

struct _SERIAL_PORT_ {
	u8 ie;
	u8 flags;
	u16 base;
	struct _SERIAL_PORT_DATA_ * data;
};
struct _SERIAL_PORT_DATA_ {
	u16 head;
	u16 tail;
	SEMAPHORE semaphore;
	u32 write_remind;
	u8 * write_buf;
	FCPEB fc;
	u32 count;
	wchar_t path[SERIAL_COM_PATH_LEN];//L".dev/serial/COM%d.dev"
	u8 read_buf[SERIAL_FIFO_SIZE];
};

static struct _SERIAL_PORT_ port[4];
static int test_irq_arise(struct _SERIAL_PORT_ * _port){
	u8 status;
	u8 int_id;
	int count;
	
	if(!_port->base) return -1;
	int_id = inb(_port->base + SERIAL_INT_ID);
	if(int_id & 0x01) return 0;
	status = inb(_port->base + SERIAL_LINE_STATUS);
	//printk("COM%d port arise interrupt,%02X.\n",_port - port + 1,int_id);
	
	int_id &= 0x06;
	
	switch(int_id){
	case 0:
		//Priority:Fourth
		//Interrupt Type:MODEM status
		//Interrupt Source:Clear to send or data set ready or ring indicator or data carrier detect
		//Interrupt Reset Control:Reading the MODEM status register
		status = inb(_port->base + SERIAL_MODEM_STATUS);
		//printk("status:%02X.\n",status);
		break;
	case 2:
		//Third
		//Transmitter Hilding register empty
		//Transmitter holding register empty
		//Reading thr IRR Register(if source of interrupt) or writing int the transmitter holding register
		if(_port->ie & 0x01) {
			if(_port->data->write_buf){
				outb(_port->base + SERIAL_DATA,*_port->data->write_buf);
				_port->data->write_buf++;
				_port->data->write_remind--;
				if(!_port->data->write_remind) _port->data->write_buf = NULL;
			}
			else _port->flags &= ~PORT_SENDING;
		}
		//printk("transmitter available.\n");
		break;
	case 4:
		//Second
		//Recived data available
		//Recived data available
		//Reading the receiver buffer register
		if(_port->flags & PORT_BUSY){//discard data if serial isn't open or buffer overflow.
			count = 0;
			do{
				_port->data->read_buf[_port->data->tail] = inb(_port->base + SERIAL_DATA);
				_port->data->tail++;
				_port->data->tail %= SERIAL_FIFO_SIZE;
				count++;
				if(xaddd(&(_port->data->count),1) >= SERIAL_FIFO_SIZE) {
					do inb(_port->base + SERIAL_DATA);
					while(inb(_port->base + SERIAL_LINE_STATUS) & 1);
					break;
				}
			} while(inb(_port->base + SERIAL_LINE_STATUS) & 1);
			release_semaphore(count,&(_port->data->semaphore),0);
		}
		else {
			do inb(_port->base + SERIAL_DATA);
			while(inb(_port->base + SERIAL_LINE_STATUS) & 1);
		}
		break;
	case 6:
		//Highest
		//Reciver Line Status
		//Overrun error or patity error or framing error or break interrupt
		//Reading the line status register
		status = inb(_port->base + SERIAL_LINE_STATUS);
		//printk("status:%02X.\n",status);
		break;
	}
	return 0;
}
static int serial_handle(int IRQ){
	if(IRQ == SERIAL1_IRQ){
		test_irq_arise(port);
		test_irq_arise(port + 2);
	}
	else{
		test_irq_arise(port + 1);
		test_irq_arise(port + 3);
	}
	return 0;
}
static int serial_read(LPSTREAM file,size_t count,void * buf){
	struct _SERIAL_PORT_ * _port;
	u8 * read_buf;
	int buf_size;
	
	_port = file->fc->data;
	while(count >= SERIAL_FIFO_SIZE){
		count -= SERIAL_FIFO_SIZE/2;
		if(wait_semaphore(SERIAL_FIFO_SIZE/2,&(_port->data->semaphore),SERIAL_COM_WAIT_TIME + 16 * SERIAL_FIFO_SIZE)) return -1;
		read_buf = _port->data->read_buf + _port->data->head;
		buf_size = SERIAL_FIFO_SIZE - _port->data->head;
		if(buf_size < SERIAL_FIFO_SIZE/2){
			memcpy(buf,read_buf,buf_size);
			buf = ((u8*)buf) + buf_size;
			buf_size = SERIAL_FIFO_SIZE/2 - buf_size;
			_port->data->head = buf_size;
			memcpy(buf,_port->data->read_buf,buf_size);
			buf = ((u8*)buf) + buf_size;
		}
		else{
			memcpy(buf,read_buf,SERIAL_FIFO_SIZE/2);
			_port->data->head += SERIAL_FIFO_SIZE/2;
			buf = ((u8*)buf) + SERIAL_FIFO_SIZE/2;
		}
		xaddd(&(_port->data->count),SERIAL_FIFO_SIZE/2);
	}
	if(wait_semaphore(count,&(_port->data->semaphore),SERIAL_COM_WAIT_TIME + 32 * count)) return -1;
	read_buf = _port->data->read_buf + _port->data->head;
	buf_size = SERIAL_FIFO_SIZE - _port->data->head;
	if(buf_size > count){
		memcpy(buf,read_buf,count);
		_port->data->head += count;
		
	}
	else{
		memcpy(buf,read_buf,buf_size);
		buf = ((u8*)buf) + buf_size;
		count -= buf_size;
		_port->data->head = count;
		memcpy(buf,_port->data->read_buf,count);
	}
	xaddd(&(_port->data->count),count);
	return 0;
}
static int serial_write(LPSTREAM file,size_t count,void * buf){
	struct _SERIAL_PORT_ * _port;
	
	_port = file->fc->data;
	ID();
	if(spin_try_lock_bit(&(_port->flags),PORT_SENDING_BIT)) {
		IE();
		return ERR_RESOURCE_BUSY;
	}
	outb(_port->base + SERIAL_DATA,*(u8*)buf);
	count--;
	if(count){
		_port->data->write_buf = ((u8*)buf) + 1;
		_port->data->write_remind = count;
	}
	IE();
	return 0;
}
static LPSTREAM open_serial(wchar_t * name,u64 mode,struct _FCPEB_ * fc){
	struct _SERIAL_PORT_ * _port;
	LPSTREAM file;
	LPPROCESS process;
	
	ID();
	_port = fc->data;
	if(_port->flags & PORT_BUSY) return NULL;
	_port->flags = PORT_BUSY;
	_port->data->count = 0;
	file = kmalloc(sizeof(STREAM),0);
	file->gst = GST_FILE;
	file->flags = 0;
	file->read_pos = 0;
	file->write_pos = 0;
	file->data = NULL;
	file->fc = fc;
	IE();
	return file;
}
static int close_serial(LPSTREAM file){
	struct _SERIAL_PORT_ * _port;
	
	_port = file->fc->data;
	_port->flags = 0;
	kfree(file);
	return 0;
}
static int serial_get(LPSTREAM file){
	struct _SERIAL_PORT_ * _port;
	int element;
	
	if(wait_semaphore(1,&(_port->data->semaphore),SERIAL_COM_WAIT_TIME)) return -1;
	element = _port->data->read_buf[_port->data->head];
	_port->data->head++;
	_port->data->head %= SERIAL_FIFO_SIZE;
	xaddd(&(_port->data->count),-1);
	return element;
}
static int serial_put(LPSTREAM file,int element){
	struct _SERIAL_PORT_ * _port;
	
	_port = file->fc->data;
	ID();
	if(spin_try_lock_bit(&(_port->flags),PORT_SENDING_BIT)) {
		IE();
		return ERR_RESOURCE_BUSY;
	}
	_port->flags |= PORT_SENDING;
	outb(_port->base + SERIAL_DATA,element);
	IE();
	return 0;
}
static int serial_fresh(LPSTREAM file){
	struct _SERIAL_PORT_ * _port;
	
	_port = file->fc->data;
	while(!wait_semaphore(1,&(_port->data->semaphore),0));
	return 0;
}
static FCPEB serial_fc = {
	.in = NULL,
	.out = NULL,
	.open = open_serial,
	.close = close_serial,
	.read = serial_read,
	.write = serial_write,
	.seek_get = NULL,
	.seek_put = NULL,
	.tell_get = NULL,
	.tell_put = NULL,
	.get = serial_get,
	.put = serial_put,
	.read_block = NULL,
	.write_block = NULL,
	.fresh_get = serial_fresh,
	.fresh_put = serial_fresh,
	.get_size = NULL,
	.set_size = NULL,
	.get_count = NULL,
	.set_count = NULL,
	.data = NULL
};
void serial_init(){
	int i;
	unsigned cur_data,prev_data = 0;
	
	memset(port,0,sizeof(port));
	port[0].base = *(u16*)ADDRP2V(0x400);
	port[1].base = *(u16*)ADDRP2V(0x402);
	port[2].base = *(u16*)ADDRP2V(0x404);
	port[3].base = *(u16*)ADDRP2V(0x406);
	
	request_irq(SERIAL1_IRQ,serial_handle);
	request_irq(SERIAL2_IRQ,serial_handle);
	
	for(i = 0;i < 4;i++){
		if(!port[i].base) continue;
		outb(port[i].base + SERIAL_IE,0x00);
		outb(port[i].base + SERIAL_LINE_CTRL,SERIAL_DLA);
		outb(port[i].base + SERIAL_DIV_LOW,0x01);
		outb(port[i].base + SERIAL_DIV_HIGH,0x00);
		outb(port[i].base + SERIAL_LINE_CTRL,SERIAL_WORD_LEN_8 | SERIAL_STOP_LEN_1);
		outb(port[i].base + SERIAL_FIFO_CTRL,0xc7);
		outb(port[i].base + SERIAL_IE,0x0f);
		port[i].ie = 0x0f;
		port[i].flags = 0;
		port[i].data = kmalloc(sizeof(struct _SERIAL_PORT_DATA_),0);
		port[i].data->head = 0;
		port[i].data->tail = 0;
		port[i].data->write_buf = NULL;
		memcpy(&(port[i].data->fc),&serial_fc,sizeof(FCPEB));
		port[i].data->fc.data = &port[i];
		create_semaphore_ex(SERIAL_FIFO_SIZE,0,&(port[i].data->semaphore));
		wsprintf(port[i].data->path,SERIAL_COM_PATH_LEN,L".dev/serial/COM%d.dev",i + 1);
		fs_map(port[i].data->path,&(port[i].data->fc));
	}
	irq_enable(SERIAL1_IRQ);
	irq_enable(SERIAL2_IRQ);
}