/*********************************************************************************************************
*
* File                : 24c02.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef __24C16_H
#define __24C16_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
					  
/* Private define ------------------------------------------------------------*/
#define AT24C01A
//#define AT24C01

#define ADDR_24C02		0xA0
#define I2C_PAGESIZE	8

/* Private function prototypes -----------------------------------------------*/
void I2C_Configuration(void);
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);

#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

