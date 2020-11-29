//Lava OS
//PageFault
//20-04-11-15-05

#include "lm.h"

struct _PCI_DEV_ {
	uint8_t Bus;
	uint8_t Dev;
	uint8_t Fun;
	uint8_t Class;
	uint8_t SubClass;
	uint8_t ProgIF;
	uint8_t RevID;
};
static uint8_t (*__PCIReadByte)(uint8_t,uint8_t,uint8_t,uint8_t);
static uint16_t (*__PCIReadWord)(uint8_t,uint8_t,uint8_t,uint8_t);
static uint32_t (*__PCIReadDword)(uint8_t,uint8_t,uint8_t,uint8_t);
static int (*__PCIWriteByte)(uint8_t,uint8_t,uint8_t,uint8_t,int8_t);
static int (*__PCIWriteWord)(uint8_t,uint8_t,uint8_t,uint8_t,int16_t);
static int (*__PCIWriteDword)(uint8_t,uint8_t,uint8_t,uint8_t,int32_t);
static int DevCount;
static struct _PCI_DEV_ * DevArray;


static uint8_t PCIReadByteM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int32_t Addr = 0x80000000;
	int _O;
	int32_t Data;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	Data = ind(PCI_CONFIG_DATA);
	return (Data >> (_O << 3)) & 0x0ff;
}
static uint16_t PCIReadWordM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int32_t Addr = 0x80000000;
	int _O;
	int32_t Data;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	Data = ind(PCI_CONFIG_DATA);
	return (Data >> (_O << 3)) & 0x0ffff;
}
static uint32_t PCIReadDwordM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int32_t Addr = 0x80000000;
	int _O;
	int32_t Data;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	Data = ind(PCI_CONFIG_DATA);
	return Data;
}
static int PCIWriteByteM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint8_t v){
	int32_t Addr = 0x80000000;
	int _O;
	int32_t Data;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	Data = ind(PCI_CONFIG_DATA);
	Data &= ~(0x0ff << (_O << 3));
	Data |= v << (_O << 3);
	outd(PCI_CONFIG_ADDRESS,Addr);
	outd(PCI_CONFIG_DATA,Data);
	return 0;
}
static int PCIWriteWordM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint16_t v){
	int32_t Addr = 0x80000000;
	int _O;
	int32_t Data;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	Data = ind(PCI_CONFIG_DATA);
	Data &= ~(0x0ffff << (_O << 3));
	Data |= v << (_O << 3);
	outd(PCI_CONFIG_ADDRESS,Addr);
	outd(PCI_CONFIG_DATA,Data);
	return 0;
}
static int PCIWriteDwordM1(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint32_t v){
	int32_t Addr = 0x80000000;
	int _O;
	
	_O = Off & 0x03;
	Addr |= ((int32_t)Off) & 0xfc;
	Addr |= ((int32_t)Fun) << 8;
	Addr |= ((int32_t)Dev) << 11;
	Addr |= ((int32_t)Bus) << 16;
	outd(PCI_CONFIG_ADDRESS,Addr);
	outd(PCI_CONFIG_DATA,v);
	return 0;
}
static uint8_t PCIReadByteM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int32_t Data;
	int _O;
	
	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	Data = ind(Port);
	return (Data >> (_O << 3)) & 0x0ff;
}
static uint16_t PCIReadWordM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int32_t Data;
	int _O;
	
	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	Data = ind(Port);
	return (Data >> (_O << 3)) & 0x0ffff;
}
static uint32_t PCIReadDwordM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int32_t Data;
	int _O;
	
	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	Data = ind(Port);
	return Data;
}
static int PCIWriteByteM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint8_t v){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int32_t Data;
	int _O;
	
	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	Data = ind(Port);
	Data &= ~(0x0ff << (_O << 3));
	Data |= v << (_O << 3);
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	outd(Port,Data);
	return 0;
}
static int PCIWriteWordM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint16_t v){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int32_t Data;
	int _O;
	
	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	Data = ind(Port);
	Data &= ~(0x0ffff << (_O << 3));
	Data |= v << (_O << 3);
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	outd(Port,Data);
	return 0;
}
static int PCIWriteDwordM2(uint8_t Bus,uint8_t Dev,uint8_t Fun,uint8_t Off,uint32_t v){
	int16_t Port = 0xc000;
	int8_t Addr = 0xf0;
	int _O;

	_O = Off & 0x03;
	Addr |= Fun << 1;
	Port |= ((int16_t)Dev) << 8;
	Port |= ((int16_t)Off) & 0xfc;
	outb(0xcf8,Addr);
	outb(0xcfa,Bus);
	outd(Port,v);
	return 0;
}
uint8_t lm_PCIReadByte(HANDLE _Dev,uint8_t Reg){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIReadByte(Dev->Bus,Dev->Dev,Dev->Fun,Reg);
}
uint16_t lm_PCIReadWord(HANDLE _Dev,uint8_t Reg){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIReadWord(Dev->Bus,Dev->Dev,Dev->Fun,Reg);
}
uint32_t lm_PCIReadDword(HANDLE _Dev,uint8_t Reg){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIReadDword(Dev->Bus,Dev->Dev,Dev->Fun,Reg);
}
int lm_PCIWriteByte(HANDLE _Dev,uint8_t Reg,uint8_t v){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIWriteByte(Dev->Bus,Dev->Dev,Dev->Fun,Reg,v);
}
int lm_PCIWriteWord(HANDLE _Dev,uint8_t Reg,uint16_t v){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIWriteWord(Dev->Bus,Dev->Dev,Dev->Fun,Reg,v);
}
int lm_PCIWriteDword(HANDLE _Dev,uint8_t Reg,uint32_t v){
	struct _PCI_DEV_ * Dev = (struct _PCI_DEV_ *)_Dev;
	return __PCIWriteDword(Dev->Bus,Dev->Dev,Dev->Fun,Reg,v);
}

HANDLE lm_PCISearchClass(HANDLE _Dev,uint32_t Class,uint32_t Subclass,uint32_t ProgIF){
	struct _PCI_DEV_ * Dev;

	if(Class >= 0x100) return NULL;
	if(_Dev) {
		Dev = (struct _PCI_DEV_ *)_Dev;
		Dev++;
		if(Dev >= DevArray + DevCount || Dev < DevArray) return NULL;
	}
	else Dev = DevArray;
	for(;Dev < DevArray + DevCount;Dev++){
		if(Dev->Class != Class) continue;
		if(Subclass >= 0x100) return Dev;
		if(Dev->SubClass != Subclass) continue;
		if(ProgIF >= 0x100) return Dev;
		if(Dev->ProgIF == ProgIF) return Dev;
	}
	return NULL;
}
int64_t lm_CfgPCI(int64_t MemoryStart, int32_t PCIBIOSEnt, uint16_t PCIax, uint16_t PCIbx, uint16_t PCIcx){
	int Bus,Device,Function,DevicePerBus;
	struct _PCI_DEV_ * Dev;
	struct _PCI_DEV_ * _IntLine[4] = {NULL,NULL,NULL,NULL};
	uint32_t IntLine;
	int i;
	
	DevArray = Dev = (struct _PCI_DEV_ *)MemoryStart;
	DevCount = 0;
	
	if(PCIax & 0x01){
		__PCIReadByte = PCIReadByteM1;
		__PCIReadWord = PCIReadWordM1;
		__PCIReadDword = PCIReadDwordM1;
		__PCIWriteByte = PCIWriteByteM1;
		__PCIWriteWord = PCIWriteWordM1;
		__PCIWriteDword = PCIWriteDwordM1;
		DevicePerBus = 32;
	}
	else if(PCIax & 0x02){
		__PCIReadByte = PCIReadByteM2;
		__PCIReadWord = PCIReadWordM2;
		__PCIReadDword = PCIReadDwordM2;
		__PCIWriteByte = PCIWriteByteM2;
		__PCIWriteWord = PCIWriteWordM2;
		__PCIWriteDword = PCIWriteDwordM2;
		DevicePerBus = 16;
	}
	else{
		
		
	}
	PCIcx &= 0xff;
	for(Bus = 0;Bus <= PCIcx;Bus++){
		for(Device = 0;Device < DevicePerBus;Device++){
			for(Function = 0;Function < 8;Function++){
				if(__PCIReadWord(Bus,Device,Function,PCIVndID) == 0xffff) continue;
				Dev->Bus = Bus;
				Dev->Dev = Device;
				Dev->Fun = Function;
				DevCount++;
				Dev->Class = __PCIReadByte(Bus,Device,Function,PCIClass);
				Dev->SubClass = __PCIReadByte(Bus,Device,Function,PCISubClass);
				Dev->ProgIF = __PCIReadByte(Bus,Device,Function,PCIProgIF);
				Dev->RevID = __PCIReadByte(Bus,Device,Function,PCIRevID);
				Dev++;			
			}
		}
	}
	Dev++;
	MemoryStart = (int64_t)Dev;
	MemoryStart += 0x0f;
	MemoryStart &= 0xfffffffffffffff0;
	return MemoryStart;
}