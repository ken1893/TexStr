/********************************************************************************************************
*
* File                : 18B20.h
* Hardware Environment: Open103V
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : WaveShare
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#include "18B20.h"

void convertDs18b20(void) 
{ 
    resetOnewire(); 
    wOnewire(0xcc); 
    wOnewire(0x44); 
}


u8* readID(void) 
{ 
	u8 ID[8],i,*p;
	resetOnewire(); 
	wOnewire(0x33);
	for(i=0;i<8;i++)
	{ID[i]=rOnewire();}
	p=ID;
	return p;
}


u8 readTemp(void) 
{ 
	u8 temp1,temp2;
	convertDs18b20();
	resetOnewire(); 
	wOnewire(0xcc); 
	wOnewire(0xbe); 	
	temp1=rOnewire(); 
	temp2=rOnewire(); 
	temp2=temp2<<4;
	temp1=temp1>>4;
	temp2|=temp1;

	return (temp2&0x7F);
}

