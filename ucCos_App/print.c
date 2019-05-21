

#define PRINT_GLOBALS
#include "includes.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*************************行间距例程************************************/
void PrintHead(void)
{
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);   //初始化打印机
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	

	UsartPrintf(USART2,"		电子单纤维强力机试验报告: \n");
	//Uart2_SendChar(0x0c);   // change page
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10); 
	//UsartPrintf(USART2,"01234567890123456789012345678901234567890123456789012345678901234567890123456789");
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(13);
	//UsartPrintf(USART2,"a1234567890123456789012345678901234567890123456789012345678901234567890123456789z");
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('C');Uart2_SendChar(0);Uart2_SendChar(11);   // 以行为单位设定页长，英寸
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	UsartPrintf(USART2,"日期:00/01/01  	时间:00:00:00  	温度:25  	湿度\n");//打印字串
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	UsartPrintf(USART2,"夹持长度:%dmm  	拉伸速度:mm/min  	试验次数:%d  	试验员\n",Holding.RegS.CycleNum,GuanTimes_Cache*Guan_Cache);//打印字串
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	UsartPrintf(USART2,"采样直径:%dmm  	力降:%d%%  	试验单位\n",Holding.RegS.TEX,Holding.RegS.ForceDrop);//打印字串
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10);     // 断落大间距
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换行
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(10);
	//Uart2_SendChar(0x1b);Uart2_SendChar('Q');Uart2_SendChar(100);
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(0);
	//UsartPrintf(USART2,"01234567890123456789012345678901234567890123456789012345678901234567890123456789");											 //打印字符串
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(136);
	//UsartPrintf(USART2,"LONG              SPEED              TIMES");

}


/*************************打印测试中间值***************************************/
void PrintBody(void)
{
	uint8_t i,j;
	
	//Guan_Cache;        // 管数
	//GuanTimes_Cache;   // 次数
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					   //初始化打印机
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	
	
	UsartPrintf(USART2,"试验次数	断裂强力	断裂伸长	断裂强度	断裂伸长率	断裂时间	初始模量\n");
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换行
	
	UsartPrintf(USART2,"	cN	mm	Mpa	%%	S	Mpa");//标尺
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x51);Uart2_SendChar(0x12);          //ESC  Q 命令，右限为6
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	
	for(i=0;i<Guan_Cache;i++)    // 管数
	{
		uint32_t sumtemp[6];
		uint32_t CVgemp[6];
		double temp;
		
		sumtemp[0] = 0;
		sumtemp[1] = 0;
		sumtemp[2] = 0;
		sumtemp[3] = 0;
		sumtemp[4] = 0;
		sumtemp[5] = 0;
		
		CVgemp[0] = 0;
		CVgemp[1] = 0;
		CVgemp[2] = 0;
		CVgemp[3] = 0;
		CVgemp[4] = 0;
		CVgemp[5] = 0;
		
		for(j = 0;j<GuanTimes_Cache;j++)
		{
			uint8_t ttemp = i * GuanTimes_Cache + j;
			
			sumtemp[0] += ResultTEX[ttemp][0];
			sumtemp[1] += ResultTEX[ttemp][1];
			sumtemp[2] += ResultTEX[ttemp][2];
			sumtemp[3] += ResultTEX[ttemp][3];
			sumtemp[4] += ResultTEX[ttemp][4];
			sumtemp[5] += ResultTEX[ttemp][5];
			
			UsartPrintf(USART2,"%d  %d	%d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",i+1,j+1,ResultTEX[ttemp][0]/PC,ResultTEX[ttemp][0]%PC,ResultTEX[ttemp][1]/PC,ResultTEX[ttemp][1]%PC,ResultTEX[ttemp][2]/PC,ResultTEX[ttemp][2]%PC,ResultTEX[ttemp][3],ResultTEX[ttemp][4]/PC,ResultTEX[ttemp][4]%PC,ResultTEX[ttemp][5]/PC,ResultTEX[ttemp][5]%PC);//打印字串
		}
		
		sumtemp[0] = sumtemp[0] / GuanTimes_Cache;    // average
		sumtemp[1] = sumtemp[1] / GuanTimes_Cache;
		sumtemp[2] = sumtemp[2] / GuanTimes_Cache;
		sumtemp[3] = sumtemp[3] / GuanTimes_Cache;
		sumtemp[4] = sumtemp[4] / GuanTimes_Cache;
		sumtemp[5] = sumtemp[5] / GuanTimes_Cache;
		
		// CV 1、求与平均值之差的二次方的和
		for(j = 0;j<GuanTimes_Cache;j++)
		{
			uint8_t ttemp = i * GuanTimes_Cache + j;
			
			CVgemp[0] += (ResultTEX[ttemp][0] - sumtemp[0]) * (ResultTEX[ttemp][0] - sumtemp[0]);   // (x - average)2次方
			CVgemp[1] += (ResultTEX[ttemp][1] - sumtemp[1]) * (ResultTEX[ttemp][1] - sumtemp[1]);
			CVgemp[2] += (ResultTEX[ttemp][2] - sumtemp[2]) * (ResultTEX[ttemp][2] - sumtemp[2]);
			CVgemp[3] += (ResultTEX[ttemp][3] - sumtemp[3]) * (ResultTEX[ttemp][3] - sumtemp[3]);
			CVgemp[4] += (ResultTEX[ttemp][4] - sumtemp[4]) * (ResultTEX[ttemp][4] - sumtemp[4]);
			CVgemp[5] += (ResultTEX[ttemp][5] - sumtemp[5]) * (ResultTEX[ttemp][5] - sumtemp[5]);
		}

		CVgemp[0] = sqrt(CVgemp[0] / (GuanTimes_Cache - 1));
		CVgemp[1] = sqrt(CVgemp[1] / (GuanTimes_Cache - 1));
		CVgemp[2] = sqrt(CVgemp[2] / (GuanTimes_Cache - 1));
		CVgemp[3] = sqrt(CVgemp[3] / (GuanTimes_Cache - 1));
		CVgemp[4] = sqrt(CVgemp[4] / (GuanTimes_Cache - 1));
		CVgemp[5] = sqrt(CVgemp[5] / (GuanTimes_Cache - 1));
		
		UsartPrintf(USART2,"平均值 	%d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",sumtemp[0]/PC,sumtemp[0]%PC,sumtemp[1]/PC,sumtemp[1]%PC,sumtemp[2]/PC,sumtemp[2]%PC,sumtemp[3],sumtemp[4]/PC,sumtemp[4]%PC,sumtemp[5]/PC,sumtemp[5]%PC);//打印字串
		UsartPrintf(USART2,"CV值 %d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",CVgemp[0]/PC,CVgemp[0]%PC,CVgemp[1]/PC,CVgemp[1]%PC,CVgemp[2]/PC,CVgemp[2]%PC,CVgemp[3],CVgemp[4]/PC,CVgemp[4]%PC,CVgemp[5]/PC,CVgemp[5]%PC);//打印字串
		UsartPrintf(USART2,"\n");//回车
	}
}

//--------------------------------------------------------------------------------
 uint16_t MAXSel(uint8_t A,uint8_t C)    // Array  Count
{
	uint16_t Mtemp;
	uint8_t i;
	
	for(i=0;i<C;i++)
	{
		if(ResultTEX[i][A] > Mtemp)Mtemp = ResultTEX[i][A];
	}
	
	return Mtemp;
}

//--------------------------------------------------------------------------------
 uint16_t MINSel(uint8_t A,uint8_t C)    // Array  Count
{
	uint16_t Mtemp;
	uint8_t i;
	
	Mtemp = ResultTEX[0][A];
	
	for(i=0;i<C;i++)
	{
		if(ResultTEX[i][A] < Mtemp)Mtemp = ResultTEX[i][A];
	}
	
	return Mtemp;
}

//--------------------------------------------------------------------------------
 uint16_t AVerSel(uint8_t A,uint8_t C)    // Array  Count
{
	uint32_t Mtemp;
	uint8_t i;
	
	for(i=0;i<C;i++)
	{
		Mtemp += ResultTEX[i][A];
	}
	
	return Mtemp/C;
}


/*************************打印测试中间值***************************************/
void PrintBottom(void)
{
	uint8_t i;
	uint16_t sumtemp[6];
		sumtemp[0] = 0;
		sumtemp[1] = 0;
		sumtemp[2] = 0;
		sumtemp[3] = 0;
		sumtemp[4] = 0;
		sumtemp[5] = 0;
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					   //初始化打印机
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	
	UsartPrintf(USART2,"	**统计报表**\n");
	UsartPrintf(USART2," 	断裂强力	断裂伸长	断裂强度	断裂伸长率	断裂时间	初始模量\n");
	
	UsartPrintf(USART2,"	cN	mm	Mpa	%%	S	Mpa");//标尺
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	
	for(i=0;i<6;i++)
	{
		sumtemp[i] = MAXSel(i,GuanTimes_Cache * Guan_Cache);
	}
	UsartPrintf(USART2,"最大值 	%d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",sumtemp[0]/PC,sumtemp[0]%PC,sumtemp[1]/PC,sumtemp[1]%PC,sumtemp[2]/PC,sumtemp[2]%PC,sumtemp[3],sumtemp[4]/PC,sumtemp[4]%PC,sumtemp[5]/PC,sumtemp[5]%PC);//打印字串

	for(i=0;i<6;i++)
	{
		sumtemp[i] = MINSel(i,GuanTimes_Cache * Guan_Cache);
	}
	UsartPrintf(USART2,"最小值 	%d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",sumtemp[0]/PC,sumtemp[0]%PC,sumtemp[1]/PC,sumtemp[1]%PC,sumtemp[2]/PC,sumtemp[2]%PC,sumtemp[3],sumtemp[4]/PC,sumtemp[4]%PC,sumtemp[5]/PC,sumtemp[5]%PC);//打印字串

	for(i=0;i<6;i++)
	{
		sumtemp[i] = AVerSel(i,GuanTimes_Cache * Guan_Cache);
	}
	
	UsartPrintf(USART2,"平均值 	%d.%d	%d.%d	%d.%d	%d	%d.%d	%d.%d\n",sumtemp[0]/PC,sumtemp[0]%PC,sumtemp[1]/PC,sumtemp[1]%PC,sumtemp[2]/PC,sumtemp[2]%PC,sumtemp[3],sumtemp[4]/PC,sumtemp[4]%PC,sumtemp[5]/PC,sumtemp[5]%PC);//打印字串

		UsartPrintf(USART2,"\n");//回车

}

/*************************行间距例程************************************/
void hangjianju(void)
{
	uint8_t i;
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	// 调用16*16点阵汉字，24点阵为24*24
	UsartPrintf(USART2,"试验次数	断裂强力	断裂伸长	断裂强度	断裂伸长率	断裂时间	初始模量\n");
	//print_chs("行间距测试：\n");
	for(i=0;i<11;i=i+2)			 //循环打印
	{
		Uart2_SendChar(0x1b);Uart2_SendChar(0x31);Uart2_SendChar(i);		  // ESC 1，设置行间距
		UsartPrintf(USART2,"012345678901234567890123456789012345678901234567890123456789");
		//print_chs("LINE SPACING");
	//	print(0x0a);   //打印字符串和换行
	}
}

/*************************水平造表例程***************************************/
void shuipingzhaobiao(void)
{
	uint8_t i;
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					   //初始化打印机
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	
	
	UsartPrintf(USART2,"试验次数	断裂强力	断裂伸长	断裂强度	断裂伸长率	断裂时间	初始模量\n");
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换行
	
	UsartPrintf(USART2,"	cN	mm	Mpa	\%	S	Mpa");//标尺
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x51);Uart2_SendChar(0x12);          //ESC  Q 命令，右限为6
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	for(i=0;i<10;i++)
	{
	  UsartPrintf(USART2,"%d b 12345678901234567890123456789012 z\n",i);//打印字串
	}
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	//UsartPrintf(USART2,"b 12345678901234567890123456789012 z");//打印字串
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	//UsartPrintf(USART2,"c 12345678901234567890123456789012 zaaaaaaaaaaaaaaaaa");//打印字串
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//换6行
	//UsartPrintf(USART2,"d 12345678901234567890123456789012 zddddddddddddddddd");//打印字串
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10);								  //回车
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					  //初始化打印机
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	//UsartPrintf(USART2,"电子单纤维强力机试验报告:\n");
	//UsartPrintf(USART2,"12345678901234567890123456789012");//标尺
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x6c);Uart2_SendChar(0x06);		  //ESC I 命令，左限为6
	//UsartPrintf(USART2,"12345678901234567890123456789012");//打印字串
	//Uart2_SendChar(0x0d);								  //回车
}

void Uart2_SendChar(uint8_t Ch)
{
	USART_SendData(USART2,Ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

//-------------------------------------------------------------------------
//	函数名称：	Usart_SendString
//	函数功能：	串口数据发送
//	入口参数：	USARTx：串口组
//				str：要发送的数据
//				len：数据长度
//	返回参数：	无
//	说明：		
//-------------------------------------------------------------------------
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}

//-------------------------------------------------------------------------
//	函数名称：	UsartPrintf
//	函数功能：	格式化打印
//	入口参数：	USARTx：串口组
//				fmt：不定长参
//	返回参数：	无
//	说明：		
//-------------------------------------------------------------------------
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

/*************************打印空格或换行例程**********************************/
/*void konggehuanhang(void)
{
	print(0x1b);print(0x40);   //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("换行或空格测试：\n");
	print_chs("TE");		   //打印字符串“TE”
	print(0x1b);print(0x66);print(0);print(0x06);				//空六格
	print_chs("ST");		   //打印字符串“ST”
	print(0x0d);			   //回车
	print_chs("TE");		   //打印字符串“TE”
	print(0x1b);print(0x66);print(0x01);print(0x06);			//换6行
	print_chs("ST");		   //打印字符串“ST”
	print(0x0d);			   //回车
	
}

/*************************右限例程**********************************/
/*void youxian(void)
{
	print(0x1b);print(0x40);					  //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("右限测试：\n");
	print_chs("12345678901234567890123456789012");//标尺
	print(0x1b);print(0x51);print(0x06);          //ESC  Q 命令，右限为6
	print_chs("12345678901234567890123456789012");//打印字串
	print(0x0d);								  //回车
}
/*************************左限例程**********************************/
/*void zhuoxian(void)
{
	print(0x1b);print(0x40);					  //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("左限测试：\n");
	print_chs("12345678901234567890123456789012");//标尺
	print(0x1b);print(0x6c);print(0x06);		  //ESC I 命令，左限为6
	print_chs("12345678901234567890123456789012");//打印字串
	print(0x0d);								  //回车
								
}
/*************************放大指令例程**********************************/
/*void fangda(void)
{
	uchar i;
	print(0x1b);print(0x40);				   //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("放大测试：\n");
	for(i=1;i<4;i++)                           //从1到3倍
	{
		print (0x1b);print(0x55);print(i);     //横向放大
		print_chs("RD");					   //打印字符
	}
	print(0x0d); 							   //回车命令
	print(0x1b);print(0x40);				   //初始化打印机
	for(i=1;i<4;i++)                           //从1到3倍
	{
		print (0x1b);print(0x56);print(i);	   //纵放命令
		print_chs("RD");					   //打印字符
		print(0x0d);						   //回车
	}
	print(0x1b);print(0x40);				   //初始化打印机
	for(i=1;i<4;i++)						   //从1到3倍
	{
	    print (0x1b);print(0x57);print(i);     //横纵向同时放大
		print_chs("RD");                       //打印字符
		print(0x0d);                           //回车
	
	}		

}
/*************************划线指令例程**********************************/
/*void huaxian(void)
{
	print(0x1b);print(0x40);		 //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("划线测试：\n");
	print(0x1b);print(0x57);print(2);//放大两倍打印，便于观察划线
	print_chs("RD");				 //打印字串
	print(0x1b);print(0x2d);print(1);//允许下划线打印
	print_chs("UPT");				 //UPT打印下划线
	print(0x1b);print(0x2d);print(0);//禁止下划线打印
	print_chs("RD");	 			 //打印字串
	print(0x0d);					 //回车
	print(0x1b);print(0x40);		 //初始化打印机
	print(0x1b);print(0x57);print(2);//放大两倍打印，便于观察划线
	print_chs("RD");				 //打印字串
	print(0x1b);print(0x2b);print(1);	//允许上打印线打印
	print_chs("UPT");				 	//UPT上打印上划线
	print(0x1b);print(0x2b);print(0);	//禁止上划线        
	print_chs("RD");					//打印字串
	print(0x0d);						//打印回车
	
}
/*************************上下标指令例程**********************************/
/*void shangxiabiao(void)
{
	print(0x1b);print(0x40);			  //初始化打印机
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("上下标测试：\n");
	print(0x1b);print(0x57);print(0x02);  //放大两倍
	print('M');                      	  //打印字符'M'
	print(0x1b);print(0x57);print(0x01);  //原倍
	print(0x1c);print(0x72);print(0);     //上标命令
	print('2');                       	  //打印字符'2'
	print(0x0d);                          //回车
	
	print(0x1b);print(0x57);print(0x01);  //原倍
	print(0x1b);print(0x57);print(0x02);  //放大两倍
	print('M');                           //打印字符'M'
	print(0x1c);print(0x72);print(1);     //上标命令
	print(0x1b);print(0x57);print(0x01);  //原倍
	print('2');                       	  //打印字符'2'
	print(0x0d);                      	  //回车
}
/*************************横纵向指令例程**********************************/
/*void hengzhong(void)
{
	print(0x1b);print(0x40);
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("横纵向测试：\n");
	print(0x1c);print(0x4a);                //设置纵向打印
	print_chs("ABCDEFGHI");           		//打印字符
	print(0x0d);                        	//回车   
	
	
	print(0x1b);print(0x40);
	print(0x1C);print(0x4B);                //设置横向打印
	print_chs("ABCDEFGHI");           		//打印字符
	print(0x0d);                        	//回车   
}
/*************************自定义指令例程**********************************/
/*void zidingyi(void)
{
	print(0x1b);print(0x40);				  //初始化指令
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("自定义字符测试：\n");
	print(0x1b);print(0x36);				  //如是24点阵机器，在自定义字符前面要加一个1B 36 指令才行
	print(0x1b);print(0x57);print(2);         //横向纵向放大2倍
	print(0x1b);print(0x26);print(65);        //ESC & 命令
	print(0x02);print(0x7C);print(0x40);      //字模数据
	print(0xC0);print(0x40);print(0x00);      //字模数据
	print(0x1b);print(0x25);print(65);print(65);print(0); //ESC %
	print(65);            								  //打印定义字符
	print(0x1b);print(0x3a);                              //ESC：命令　
	print(65);											  //复符集中的字符
	print(0x0d);
}
/*************************图形指令例程**********************************/
/*void tuxing(void)
{
	uchar code tch[ ]={0x7c,0x44,0x44,0xff,0x44,0x44,0x7c,0x00,0x41,0x62,0x54,0xc8,0x54,0x62,0x41};
	uchar i;	
	print(0x1b);print(0x40);							//初始化指令
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("图形测试：\n");
	//			print(0x1b);print(0x57);print(2);		//横向纵向放大2倍
	print(0x1b);print(0x4b);print(15);print(0);		  	//ESC K命令
	for(i=0;i<15;i++)									//输入15个字节数据
		print(tch[i]);
	print(0x0d);										//输入回车
	
}
/*************************曲线II指令例程**********************************/
//仅适用于热敏打印机
/*void quxian_2(void)
{
 	float  X;
	unsigned int  Y,YY;
	//			Y1=50+40*abs（-0.01*X）*sin（X/10）
	//			Y2=50-40*abs（-0.01*X）*sin（X/10）
	//			Y3=50
	//			Y4=50+40*abs(-0.1*X)
	//			Y5=50-40*abs(-0.01*X)
	//			你可以用C语言编写如下的程序：
	//			#include <math.h>
	//			unsigned int Y,YY,X;
	print(0x1b);print(0x40);							//初始化指令
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("曲线1测试：\n");
	for(X=0;X<150;X++)									//打印150点行
	{
	  Y= 40*exp(-0.01*X);
	  YY= Y*sin(X/10);
	  print(0x1b);print(39);print(5);print(0);		    //ESC命令,M=5,0
	  print(50+YY);print(0);print(50-YY);print(0);print(50);print(0);
	  print(50+Y);print(0);print(50-Y);print(0);print(13);
	  }
}
/*************************曲线I指令例程**********************************/
//仅适用于针式打印机
/*void quxian_1(void)
{
 	float  X;
	unsigned int  Y,YY;
	//			Y1=50+40*abs（-0.01*X）*sin（X/10）
	//			Y2=50-40*abs（-0.01*X）*sin（X/10）
	//			Y3=50
	//			Y4=50+40*abs(-0.1*X)
	//			Y5=50-40*abs(-0.01*X)
	//			你可以用C语言编写如下的程序：
	//			#include <math.h>
	//			unsigned int Y,YY,X;
	print(0x1b);print(0x40);							//初始化指令
	print(0x1b);print(0x38);print(0x00);   	//调用16*16点阵汉字，24点阵为24*24
	print_chs("曲线2测试：\n");
	for(X=0;X<150;X++)									//打印150点行
	{
	  Y= 40*exp(-0.01*X);
	  YY= Y*sin(X/10);
	  print(0x1b);print(39);print(5);		    //ESC命令,M=5
	  print(50+YY);;print(50-YY);print(50);;
	  print(50+Y);print(50-Y);print(13);
	  }
}

/************************旋转指令例程**********************************/
/*void xuanzhuan(void)
{
	uchar i;
	print(0x1b);print(0x40);							//初始化指令
	for(i=0;i<4;i++)						 //0,90,180,270旋转
	{

		print(0x1c);print(0x49);print(i);				    //旋转指令
		print(0x1b);print(0x38);print(0);					//汉字调出指令
		print_chs("旋转测试");								//打印汉字
	}	
	print(0x0d);										    //回车
}

/************************反白指令例程**********************************/
/*void fanbai(void)
{
	uchar i;
	print(0x1b);print(0x40);							//初始化指令
	for(i=0;i<2;i++)						 //i为1时，反白打印
	{
		print(0x1b);print(0x69);print(i);
		print(0x1b);print(0x38);print(0);
		print_chs("反白测试");	
	}	
	print(0x0d);

}
/************************反向指令例程**********************************/
/*void fanxiang(void)
{
	uchar i;   
	print(0x1b);print(0x40);							//初始化指令
	for(i=0;i<2;i++)						//i为1时，允许反向,由左到右
	{
		print(0x1b);print(0x63);print(i);
		print(0x1b);print(0x38);print(0);
		print_chs("反向测试");	
		print(0x0d);
	}	
}
/************************条码指令例程**********************************/
//只有部分型号的打印机支持此指令
/*void tiaoma(void)
{  
	print(0x1b);print(0x40);							//初始化指令
	print(0x1d);print(0x72);print(0x01);				//打印条码字符
	print(0x1d);print(0x68);print(0x60);				//设置条码高度为96点
	print(0x1d);print(0x6B);print(0x45);print(8);		//打印长度为8的CODE39码	
	print(0x2a);print(0x31);print(0x32);print(0x34);print(0x35);print(0x37);print(0x36);print(0x2a); //CODE39码数据
	print(0x0d);										//回车
	print(0x1d);print(0x6B);print(3); 					//打印EAN13码	
	print(0x32);print(0x37);print(0x30);print(0x38);print(0x35);print(0x38);print(0x35);print(0x30); //EAN13码数据
	print(0x30);print(0x39);print(0x32);print(0x34);print(0x38);print(0x00);
	print(0x0d);										//回车

}*/		 


















