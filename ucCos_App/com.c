/*
*********************************************************************************************************
*                                           uC/OS-App com.c
*
*                                     (c) Copyright 2015, Ken
*                                       All Rights Reserved
*
*
* Filename   : com.C
* Version    : V1.01
* Programmer : Ken
*********************************************************************************************************
*/

#define  COM_GLOBALS
#include <includes.h>
#include "24C16.h"

/* CRC 高位字节值表 */ 
const unsigned char auchCRCHi[256] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 

/* CRC低位字节值表*/ 
const unsigned char auchCRCLo[256] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
};


void  RxISRHandler(void);                  /* C code to process character received           */
void  RxHandler (INT8U rx_data);
static  void  RxBufClr (void);
static  void  RxStoINT8U (INT8U rx_data);

void  RxIntDis (void);
void  RxIntEn (void);
struct crcstruct crc16(unsigned char *puchMsg, unsigned int usDataLen);
void UartDeal(void);
void ModbusPro(unsigned char com);

// MODBUS CMD MODE 
void Read_InputRegister(void);     // 读输入寄存器
void Read_HoldingRegister(void);   // 讀保持寄存器
void Write_SingleHolding(void);    // 写单个保持寄存器   0x06
void Write_MultiHolding(void);	   // 写多个保持寄存器   

void Send_CommonReg(u8* pBuffer, u8 Num);	   // 发送装载数据并计算校验



/************************************************************************************
** 函数名称: crc16
** 功能描述: 对接收数据进行crc校验
** 输　入: 地址指针 *puchMsg, 数据长度 usDataLen
** 输　出: crcstruct (crc结构体)
** 全局变量: 无
** 调用模块:
*************************************************************************************/
struct crcstruct crc16(unsigned char *puchMsg, unsigned int usDataLen) 
{ 
	unsigned char uchCRCHi = 0xFF;   //高CRC字节初始化  
	unsigned char uchCRCLo = 0xFF;   //低CRC字节初始化  
	unsigned int uIndex;             //CRC循环中的索引 
	struct crcstruct temp;   //返回值
	 
	while(usDataLen--)       // 传输消息缓冲区  
	{ 
		uIndex = uchCRCHi ^ *puchMsg++;             // 计算CRC 
		uchCRCHi = uchCRCLo ^ (auchCRCHi[uIndex]); 
		uchCRCLo = auchCRCLo[uIndex]; 
	} 	

	temp.crcH = uchCRCHi;
	temp.crcL = uchCRCLo;

	return(temp);
}

/********************************************************************************
*  UartDeal(void)
*  函数功能: 处理串口接收
*********************************************************************************/
void UartDeal(void)
{
	unsigned char numTemp = 0;    // 数据个数
	struct crcstruct temp;
	
	RxIntDis();			  // 关接收中断
		
	if(RxBuf[1] == 0x01 || RxBuf[1] == 0x02 || 
		RxBuf[1] == 0x04 || RxBuf[1] == 0x05 ||
		RxBuf[1] == 0x03 || RxBuf[1] == 0x06){
			numTemp = 6;
	}
	else if(RxBuf[1] == 0x10){
			numTemp = MulLen;
			MulLen = 0;	
	}
		
	temp = crc16(RxBuf,numTemp);
	if(temp.crcH == RxBuf[numTemp] && temp.crcL == RxBuf[numTemp + 1]){
			ModbusPro(RxBuf[1]);    // 处理modbus命令
	}
		
	RxBufClr();		   
	memset(RxBuf,0,sizeof(RxBuf));
	RxIntEn();                  // 开接收中断

}


/********************************************************************************
*  ModbusPro(unsigned char com)
*  函数功能: 处理串行modbus协议
*********************************************************************************/
void ModbusPro(unsigned char com)
{
	switch(com)
		{
			case 0x04:
				Read_InputRegister();      // 读输入寄存器
			break;
			
			//case 0x02:
				//Read_DiscreteInputs();   // 读离散量输入
			//break;
			
			//case 0x01:
				//Read_Contoctor();        // 读接触器状态
			//break;
			
			//case 0x05:
				//Write_SingleBit();   // 写一个位
			//break;
			
			//case 0x0f:
				//Write_MultipleContactor(); // 写多个接触器
			//break;
			
			case 0x03:
				Read_HoldingRegister();    // 讀保持寄存器
			break;
			
			case 0x06:                   // 寫單個保持寄存器
				Write_SingleHolding();
			break; 

			case 0x10:                   // 写多个寄存器
				Write_MultiHolding();	
			break;
				
			default:break;
		}
}

/********************************************************************************
*  Read_InputRegister(void)
*  函数功能: 讀輸入寄存器
*********************************************************************************/
void Read_InputRegister(void)
{
	unsigned char i;  
	unsigned char starttemp;
	unsigned char counttemp;  // 寄存器數量
	struct crcstruct crcTemp;
	
	starttemp = (RxBuf[2] << 8) + RxBuf[3];    // 起始地址
	counttemp = (RxBuf[4] << 8) + RxBuf[5];    // 寄存器數量
	
	TxBuf[0] = RxBuf[0];     // ID
	TxBuf[1] = RxBuf[1];     // modbus function code 
	TxBuf[2] = counttemp * 2;   // 字节个数
	
	for(i=0;i<counttemp;i++)
	{
		TxBuf[(i * 2) + 3] = Input.RegI[starttemp + i] >> 8;
		TxBuf[(i * 2) + 4] = Input.RegI[starttemp + i];
	}
	crcTemp = crc16(TxBuf,(3 + TxBuf[2]));
	
	TxBuf[3 + TxBuf[2]] = crcTemp.crcH;
	TxBuf[4 + TxBuf[2]] = crcTemp.crcL;
	
	TxLen = TxBuf[2] + 5;    // 发送个数
	TxEnable_Flag = 1;    // 允许发送
}


/********************************************************************************
*  Read_HoldingRegister(void)
*  函数功能: 讀保持寄存器
*********************************************************************************/
void Read_HoldingRegister()
{
	unsigned char i;
	unsigned char starttemp;   // 起始地址
	unsigned char counttemp;   // 输出线圈数量
	struct crcstruct crcTemp;
	
	starttemp = (RxBuf[2] << 8) + RxBuf[3];    // 起始地址
	counttemp = (RxBuf[4] << 8) + RxBuf[5];    // 寄存器數量
   	
	TxBuf[0] = RxBuf[0];     // ID
	if(counttemp < 1 || counttemp > 0x7d){     // 0x0001≤寄存器的数量≤0x007D
		TxBuf[1] = 0x84; 	 // 差错码	  0x84 
		TxBuf[2] = 0x03;	 // 0x0001≤寄存器的数量≤0x007D

		crcTemp = crc16(TxBuf,3);
	
		TxBuf[3] = crcTemp.crcH;
		TxBuf[4] = crcTemp.crcL;

		TxLen = 5;    // 发送个数
	}
	else if(starttemp > 0x23 || (starttemp + counttemp - 1) > 0x23){
		TxBuf[1] = 0x84; 	 // 差错码	
		TxBuf[2] = 0x02;	 // 起始地址错误，起始地址加寄存器数量错误

		crcTemp = crc16(TxBuf,3);
	
		TxBuf[3] = crcTemp.crcH;
		TxBuf[4] = crcTemp.crcL;

		TxLen = 5;    // 发送个数
	}
	else{ 	
		TxBuf[1] = RxBuf[1];     // modbus function code 
		TxBuf[2] = counttemp * 2;   // 字節數
	
		for(i=0;i<counttemp;i++)
		{
			TxBuf[(i * 2) + 3] = Holding.RegI[starttemp + i] >> 8;
			TxBuf[(i * 2) + 4] = Holding.RegI[starttemp + i];
		}
		crcTemp = crc16(TxBuf,(3 + TxBuf[2]));
	
		TxBuf[3 + TxBuf[2]] = crcTemp.crcH;
		TxBuf[4 + TxBuf[2]] = crcTemp.crcL;
	
		TxLen = TxBuf[2] + 5;    // 发送个数
	}

	TxEnable_Flag = 1;       // 允许发送
}

/********************************************************************************
*  Write_SingleHolding(void)
*  函数功能: 写單個保持寄存器
*********************************************************************************/
void Write_SingleHolding()
{
	uint8_t add;              // 寄存器地址
	struct crcstruct temp;    // temp for CRC
	
	add = (RxBuf[2] << 8) + RxBuf[3];    // 寄存器地址	

	if(add <= 18)
	{
		Holding.RegI[add] = RxBuf[4];
		Holding.RegI[add] = (Holding.RegI[add] << 8) + RxBuf[5];
		cpytex(TxBuf,RxBuf);
		TxLen = 8;                // 发送个数
		
		Flag_Save = 1;            // save para
	
		switch(add)
		{
			case 2:     // Action
			  Action_Flag = Holding.RegI[add]; 
				break;
			
			case 11:    // 测试管数
	
				break;
		
			case 12:    // 测试次数
				GuanTimes_Cache = Holding.RegI[add];
				break;
			
			case 7:     // 细分
				ZeroMove = (Holding.RegS.CycleNum / LEAD) * Holding.RegS.StepLong * 2;
				break;
			
			case 9:     // 夹持距离
				ZeroMove = (Holding.RegS.CycleNum / LEAD) * Holding.RegS.StepLong * 2;
				break;
		}
	}    // 差错
	else if(add >= 19){	  // 寄存器地址 ！= OK
		TxBuf[0] = RxBuf[0];     // ID
		TxBuf[1] = 0x86;         // 差错码 0x86
		TxBuf[2] = 0x02;
		
		temp = crc16(TxBuf,3);
	
		TxBuf[3] = temp.crcH;
		TxBuf[4] = temp.crcL;

		TxLen = 5;    // 发送个数	
	}
	/*
	else if(add >= 0x0a && add <= 0x23){   //  小于0x0a地址未允许写入
	
		Holding.RegI[add] = RxBuf[4];
		Holding.RegI[add] = (Holding.RegI[add] << 8) + RxBuf[5];

		cpytex(TxBuf,RxBuf);
		TxLen = 8;                // 发送个数
	}*/

	TxEnable_Flag = 1;    // 允许发送
}

/********************************************************************************
*  Write_MultiHolding(void)
*  函数功能: 写多个保持寄存器
*********************************************************************************/
void Write_MultiHolding(void)	   // 写多个保持寄存器
{
	unsigned char i;
	unsigned char starttemp;   // 起始地址
	unsigned char counttemp;   // 输出线圈数量
	struct crcstruct temp;
	//u16 *pa;
	//unsigned char add;   // 寄存器地址
	
	//增加处理
	starttemp = (RxBuf[2] << 8) + RxBuf[3];    // 起始地址
	counttemp = (RxBuf[4] << 8) + RxBuf[5];    // 寄存器數量

	if(starttemp >= 0x13 && (starttemp + counttemp - 1) <= 0x23){   //  调光	0x13/19, 0x23/35
		for(i = 0;i < counttemp;i++){
			Holding.RegI[starttemp + i] = RxBuf[7 + i * 2];
			Holding.RegI[starttemp + i] = (Holding.RegI[starttemp + i] << 8) + RxBuf[8 + i * 2];

			if(starttemp + i != 0x13){ 		 // 0x13目标调光，不存储
			}
		}
		//I2C_Write(I2C1,ADDR_24LC16,0,(u8 *)Holding.RegI,sizeof(Holding.RegI));
	}
	else if(starttemp == 13 && counttemp == 3){     // 写唯一地址，最后一个寄存器为特殊码
		if(RxBuf[11] == 0x12 && RxBuf[12] == 0x34){		// 特殊确认码
			
			//I2C_Write(I2C1,ADDR_24LC16,0,(u8 *)Holding.RegI,sizeof(Holding.RegI));		
		}	
	}

	TxBuf[0] = RxBuf[0];     // ID
	TxBuf[1] = RxBuf[1];     // modbus function code 
	TxBuf[2] = RxBuf[2];     // 起始地址H
	TxBuf[3] = RxBuf[3];     // 起始地址L
	TxBuf[4] = RxBuf[4];     // 寄存器数量H
	TxBuf[5] = RxBuf[5];     // 寄存器数量L

	temp = crc16(TxBuf,6);
	
	TxBuf[6] = temp.crcH;
	TxBuf[7] = temp.crcL;
	
	TxLen = 8;    // 发送个数
	TxEnable_Flag = 1;       // 允许发送			
}

/********************************************************************************
*  Send_CommonReg(u8* pBuffer, u8 Num)
*  函数功能: 计算并装载校验数据
*********************************************************************************/
void Send_CommonReg(u8* pBuffer, u8 Num)	   // 计算并装载校验数据
{
	struct crcstruct temp;

	TxBuf[0] = Holding.RegS.End_ID;
	TxBuf[1] = 0x42;                   // 命令码
	TxBuf[2] = 12;                     // 字节数
	
	temp = crc16(TxBuf,(3 + TxBuf[2]));
	
	TxBuf[3 + TxBuf[2]] = temp.crcH;
	TxBuf[4 + TxBuf[2]] = temp.crcL;
	
	TxLen = TxBuf[2] + 5;    // 发送个数  
	
	TxEnable_Flag = 1;       // 允许发送
}



/*
*********************************************************************************************************
*                                Rx/Tx Communication handler for uC/OS-View
*
* Description: This function is  called in stm32f10x_it.c 
*********************************************************************************************************
*/ 																				 
void  RxISRHandler (void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
                      /* Read one byte from the receive data register         */
        RxHandler( USART_ReceiveData(USART1) & 0xFF );       // call RxHandler

		//USART_SendData(USART1,USART_ReceiveData(USART1) & 0xFF );		       // for debug test

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);          /* Clear the USART Receive interrupt                   */

    }

    OSIntExit();                                                 /* Tell uC/OS-II that we are leaving the ISR           */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              Rx Handler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/
void  RxHandler (INT8U rx_data)
{
	switch(RxState){
		case RX_STATE_ID:
			if(rx_data == Holding.RegS.End_ID){
				RxTimeCnt = 1;                        // set rec timer
				RxState = RX_STATE_LEN;	
				RxBufClr();
				RxStoINT8U(rx_data);     // 
			}
			break;

		case RX_STATE_LEN:
			if(rx_data == 0x03 || rx_data == 0x04 || rx_data == 0x06){
				RxState = RX_STATE_DATA;
				RxStoINT8U(rx_data);
				RxRemainLen = 6;		
			}else if(rx_data == 0x10){
				RxState = RX_STATE_DATA1;
				RxStoINT8U(rx_data);
				RxRemainLen = 5;	 // 先接收5个
			}
			else{
				RxState = RX_STATE_ID;	 // can't handle the CMD
			}
			break;

		case RX_STATE_DATA:
				RxStoINT8U(rx_data);
				if (--RxRemainLen == 0) {
					pkt_Flag = 1;			 // 有数据包需要处理,在下一个 uctsk_CYCSend中解析数据			
                 	RxState = RX_STATE_ID;	 // rec end
            }
			break;

		case RX_STATE_DATA1:
				RxStoINT8U(rx_data);
				if (--RxRemainLen == 0) {
					RxRemainLen = rx_data + 2;   // 剩余寄存器数量和两个校验位
					MulLen = rx_data + 7;		 	
                 	RxState = RX_STATE_DATA;;	 // end
				}	
			break;	
	}
}

//-----------------------------------------------------------------------------------------
// copy rec char
void cpytex(uint8_t *dst, const uint8_t *src)
{
	uint8_t i;
	
	for(i=0;i<8;i++){
			dst[i] = src[i];
		}
}


/*
*********************************************************************************************************
*                                           CLEAR Rx BUFFER
*********************************************************************************************************
*/
static  void  RxBufClr (void)
{
    RxBufCnt = 0;
    RxRdIx   = 0;
}

/*
*********************************************************************************************************
*                                        STORE DATA IN Rx BUFFER
*
* Description: This routine is called whenever a valid byte has been received.
*********************************************************************************************************
*/

static  void  RxStoINT8U (INT8U rx_data)
{
    if (RxBufCnt < RX_BUF_SIZE) {
        RxBuf[RxBufCnt++] = rx_data;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       Disable Rx Interrupts
*********************************************************************************************************
*/

void  RxIntDis (void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Rx Interrupts
*********************************************************************************************************
*/

void  RxIntEn (void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
