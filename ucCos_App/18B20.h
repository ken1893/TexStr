/********************************************************************************************************
*
* File                : 18B20.c
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
#ifndef _18B20_H
#define _18B20_H

#include <stm32f10x.h>
#include "OneWire.h"

void convertDs18b20(void);
u8* readID(void);
u8 readTemp(void);

#endif /*_18B20_H*/

