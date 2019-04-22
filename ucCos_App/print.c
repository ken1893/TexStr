

#define PRINT_GLOBALS
#include "includes.h"

//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*************************�м������************************************/
void PrintHead(void)
{
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);   //��ʼ����ӡ��
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//����16*16�����֣�24����Ϊ24*24
	

	UsartPrintf(USART2,"���ӵ���άǿ�������鱨��: \n");
	//Uart2_SendChar(0x0c);   // change page
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10); 
	//UsartPrintf(USART2,"01234567890123456789012345678901234567890123456789012345678901234567890123456789");
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(13);
	//UsartPrintf(USART2,"a1234567890123456789012345678901234567890123456789012345678901234567890123456789z");
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('C');Uart2_SendChar(0);Uart2_SendChar(11);   // ����Ϊ��λ�趨ҳ����Ӣ��
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	UsartPrintf(USART2,"����:00/01/01  	ʱ��:00:00:00  	�¶�:25  	ʪ��\n");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	UsartPrintf(USART2,"�гֳ���:mm  	�����ٶ�:mm/min  	�������:25  	����Ա\n");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	UsartPrintf(USART2,"����ֱ��:mm  	����:%  	���鵥λ\n");//��ӡ�ִ�
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10);     // �������
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//����
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(10);
	//Uart2_SendChar(0x1b);Uart2_SendChar('Q');Uart2_SendChar(100);
	
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(0);
	//UsartPrintf(USART2,"01234567890123456789012345678901234567890123456789012345678901234567890123456789");											 //��ӡ�ַ���
	//Uart2_SendChar(0x1b);Uart2_SendChar('I');Uart2_SendChar(136);
	//UsartPrintf(USART2,"LONG              SPEED              TIMES");

}


/*************************ˮƽ�������***************************************/
void PrintBody(void)
{
	uint8_t i,j;
	
	//Guan_Cache;        // ����
	//GuanTimes_Cache;   // ����
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					   //��ʼ����ӡ��
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//����16*16�����֣�24����Ϊ24*24
	
	
	UsartPrintf(USART2,"�������	����ǿ��	�����쳤	����ǿ��	�����쳤��	����ʱ��	��ʼģ��\n");
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//����
	
	UsartPrintf(USART2,"	cN	mm	Mpa	%	S	Mpa");//���
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x51);Uart2_SendChar(0x12);          //ESC  Q �������Ϊ6
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	
	for(i=0;i<Guan_Cache;i++)    // ����
	{
		for(j = 0;j<GuanTimes_Cache;j++)
		{
			uint8_t ttemp = i * Guan_Cache + j;
			UsartPrintf(USART2,"%d  %d	%d	%d	%d\n",i+1,j+1,ResultTEX[ttemp][0],ResultTEX[ttemp][1],ResultTEX[ttemp][2]);//��ӡ�ִ�
		}
		UsartPrintf(USART2,"ƽ��ֵ b 12345678901234567890123456789012 z\n");//��ӡ�ִ�
		UsartPrintf(USART2,"CVֵ b 12345678901234567890123456789012 z\n");//��ӡ�ִ�
		UsartPrintf(USART2,"\n");//�س�
	}
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"b 12345678901234567890123456789012 z");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"c 12345678901234567890123456789012 zaaaaaaaaaaaaaaaaa");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"d 12345678901234567890123456789012 zddddddddddddddddd");//��ӡ�ִ�
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10);								  //�س�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					  //��ʼ����ӡ��
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//����16*16�����֣�24����Ϊ24*24
	//UsartPrintf(USART2,"���ӵ���άǿ�������鱨��:\n");
	//UsartPrintf(USART2,"12345678901234567890123456789012");//���
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x6c);Uart2_SendChar(0x06);		  //ESC I �������Ϊ6
	//UsartPrintf(USART2,"12345678901234567890123456789012");//��ӡ�ִ�
	//Uart2_SendChar(0x0d);								  //�س�
}

/*************************�м������************************************/
void hangjianju(void)
{
	uint8_t i;
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	// ����16*16�����֣�24����Ϊ24*24
	UsartPrintf(USART2,"�������	����ǿ��	�����쳤	����ǿ��	�����쳤��	����ʱ��	��ʼģ��\n");
	//print_chs("�м����ԣ�\n");
	for(i=0;i<11;i=i+2)			 //ѭ����ӡ
	{
		Uart2_SendChar(0x1b);Uart2_SendChar(0x31);Uart2_SendChar(i);		  // ESC 1�������м��
		UsartPrintf(USART2,"012345678901234567890123456789012345678901234567890123456789");
		//print_chs("LINE SPACING");
	//	print(0x0a);   //��ӡ�ַ����ͻ���
	}
}

/*************************ˮƽ�������***************************************/
void shuipingzhaobiao(void)
{
	uint8_t i;
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					   //��ʼ����ӡ��
	Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//����16*16�����֣�24����Ϊ24*24
	
	
	UsartPrintf(USART2,"�������	����ǿ��	�����쳤	����ǿ��	�����쳤��	����ʱ��	��ʼģ��\n");
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//����
	
	UsartPrintf(USART2,"	cN	mm	Mpa	%	S	Mpa");//���
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x51);Uart2_SendChar(0x12);          //ESC  Q �������Ϊ6
	
	Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	for(i=0;i<10;i++)
	{
	  UsartPrintf(USART2,"%d b 12345678901234567890123456789012 z\n",i);//��ӡ�ִ�
	}
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"b 12345678901234567890123456789012 z");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"c 12345678901234567890123456789012 zaaaaaaaaaaaaaaaaa");//��ӡ�ִ�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x66);Uart2_SendChar(0x01);Uart2_SendChar(0x0a);			//��6��
	//UsartPrintf(USART2,"d 12345678901234567890123456789012 zddddddddddddddddd");//��ӡ�ִ�
	
	//Uart2_SendChar(27);Uart2_SendChar(64);Uart2_SendChar(10);								  //�س�
	
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x40);					  //��ʼ����ӡ��
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x38);Uart2_SendChar(0x00);   	//����16*16�����֣�24����Ϊ24*24
	//UsartPrintf(USART2,"���ӵ���άǿ�������鱨��:\n");
	//UsartPrintf(USART2,"12345678901234567890123456789012");//���
	//Uart2_SendChar(0x1b);Uart2_SendChar(0x6c);Uart2_SendChar(0x06);		  //ESC I �������Ϊ6
	//UsartPrintf(USART2,"12345678901234567890123456789012");//��ӡ�ִ�
	//Uart2_SendChar(0x0d);								  //�س�
}

void Uart2_SendChar(uint8_t Ch)
{
	USART_SendData(USART2,Ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

//-------------------------------------------------------------------------
//	�������ƣ�	Usart_SendString
//	�������ܣ�	�������ݷ���
//	��ڲ�����	USARTx��������
//				str��Ҫ���͵�����
//				len�����ݳ���
//	���ز�����	��
//	˵����		
//-------------------------------------------------------------------------
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//��������
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
    }
}

//-------------------------------------------------------------------------
//	�������ƣ�	UsartPrintf
//	�������ܣ�	��ʽ����ӡ
//	��ڲ�����	USARTx��������
//				fmt����������
//	���ز�����	��
//	˵����		
//-------------------------------------------------------------------------
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

/*************************��ӡ�ո��������**********************************/
/*void konggehuanhang(void)
{
	print(0x1b);print(0x40);   //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("���л�ո���ԣ�\n");
	print_chs("TE");		   //��ӡ�ַ�����TE��
	print(0x1b);print(0x66);print(0);print(0x06);				//������
	print_chs("ST");		   //��ӡ�ַ�����ST��
	print(0x0d);			   //�س�
	print_chs("TE");		   //��ӡ�ַ�����TE��
	print(0x1b);print(0x66);print(0x01);print(0x06);			//��6��
	print_chs("ST");		   //��ӡ�ַ�����ST��
	print(0x0d);			   //�س�
	
}

/*************************��������**********************************/
/*void youxian(void)
{
	print(0x1b);print(0x40);					  //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("���޲��ԣ�\n");
	print_chs("12345678901234567890123456789012");//���
	print(0x1b);print(0x51);print(0x06);          //ESC  Q �������Ϊ6
	print_chs("12345678901234567890123456789012");//��ӡ�ִ�
	print(0x0d);								  //�س�
}
/*************************��������**********************************/
/*void zhuoxian(void)
{
	print(0x1b);print(0x40);					  //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("���޲��ԣ�\n");
	print_chs("12345678901234567890123456789012");//���
	print(0x1b);print(0x6c);print(0x06);		  //ESC I �������Ϊ6
	print_chs("12345678901234567890123456789012");//��ӡ�ִ�
	print(0x0d);								  //�س�
								
}
/*************************�Ŵ�ָ������**********************************/
/*void fangda(void)
{
	uchar i;
	print(0x1b);print(0x40);				   //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("�Ŵ���ԣ�\n");
	for(i=1;i<4;i++)                           //��1��3��
	{
		print (0x1b);print(0x55);print(i);     //����Ŵ�
		print_chs("RD");					   //��ӡ�ַ�
	}
	print(0x0d); 							   //�س�����
	print(0x1b);print(0x40);				   //��ʼ����ӡ��
	for(i=1;i<4;i++)                           //��1��3��
	{
		print (0x1b);print(0x56);print(i);	   //�ݷ�����
		print_chs("RD");					   //��ӡ�ַ�
		print(0x0d);						   //�س�
	}
	print(0x1b);print(0x40);				   //��ʼ����ӡ��
	for(i=1;i<4;i++)						   //��1��3��
	{
	    print (0x1b);print(0x57);print(i);     //������ͬʱ�Ŵ�
		print_chs("RD");                       //��ӡ�ַ�
		print(0x0d);                           //�س�
	
	}		

}
/*************************����ָ������**********************************/
/*void huaxian(void)
{
	print(0x1b);print(0x40);		 //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("���߲��ԣ�\n");
	print(0x1b);print(0x57);print(2);//�Ŵ�������ӡ�����ڹ۲컮��
	print_chs("RD");				 //��ӡ�ִ�
	print(0x1b);print(0x2d);print(1);//�����»��ߴ�ӡ
	print_chs("UPT");				 //UPT��ӡ�»���
	print(0x1b);print(0x2d);print(0);//��ֹ�»��ߴ�ӡ
	print_chs("RD");	 			 //��ӡ�ִ�
	print(0x0d);					 //�س�
	print(0x1b);print(0x40);		 //��ʼ����ӡ��
	print(0x1b);print(0x57);print(2);//�Ŵ�������ӡ�����ڹ۲컮��
	print_chs("RD");				 //��ӡ�ִ�
	print(0x1b);print(0x2b);print(1);	//�����ϴ�ӡ�ߴ�ӡ
	print_chs("UPT");				 	//UPT�ϴ�ӡ�ϻ���
	print(0x1b);print(0x2b);print(0);	//��ֹ�ϻ���        
	print_chs("RD");					//��ӡ�ִ�
	print(0x0d);						//��ӡ�س�
	
}
/*************************���±�ָ������**********************************/
/*void shangxiabiao(void)
{
	print(0x1b);print(0x40);			  //��ʼ����ӡ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("���±���ԣ�\n");
	print(0x1b);print(0x57);print(0x02);  //�Ŵ�����
	print('M');                      	  //��ӡ�ַ�'M'
	print(0x1b);print(0x57);print(0x01);  //ԭ��
	print(0x1c);print(0x72);print(0);     //�ϱ�����
	print('2');                       	  //��ӡ�ַ�'2'
	print(0x0d);                          //�س�
	
	print(0x1b);print(0x57);print(0x01);  //ԭ��
	print(0x1b);print(0x57);print(0x02);  //�Ŵ�����
	print('M');                           //��ӡ�ַ�'M'
	print(0x1c);print(0x72);print(1);     //�ϱ�����
	print(0x1b);print(0x57);print(0x01);  //ԭ��
	print('2');                       	  //��ӡ�ַ�'2'
	print(0x0d);                      	  //�س�
}
/*************************������ָ������**********************************/
/*void hengzhong(void)
{
	print(0x1b);print(0x40);
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("��������ԣ�\n");
	print(0x1c);print(0x4a);                //���������ӡ
	print_chs("ABCDEFGHI");           		//��ӡ�ַ�
	print(0x0d);                        	//�س�   
	
	
	print(0x1b);print(0x40);
	print(0x1C);print(0x4B);                //���ú����ӡ
	print_chs("ABCDEFGHI");           		//��ӡ�ַ�
	print(0x0d);                        	//�س�   
}
/*************************�Զ���ָ������**********************************/
/*void zidingyi(void)
{
	print(0x1b);print(0x40);				  //��ʼ��ָ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("�Զ����ַ����ԣ�\n");
	print(0x1b);print(0x36);				  //����24������������Զ����ַ�ǰ��Ҫ��һ��1B 36 ָ�����
	print(0x1b);print(0x57);print(2);         //��������Ŵ�2��
	print(0x1b);print(0x26);print(65);        //ESC & ����
	print(0x02);print(0x7C);print(0x40);      //��ģ����
	print(0xC0);print(0x40);print(0x00);      //��ģ����
	print(0x1b);print(0x25);print(65);print(65);print(0); //ESC %
	print(65);            								  //��ӡ�����ַ�
	print(0x1b);print(0x3a);                              //ESC�����
	print(65);											  //�������е��ַ�
	print(0x0d);
}
/*************************ͼ��ָ������**********************************/
/*void tuxing(void)
{
	uchar code tch[ ]={0x7c,0x44,0x44,0xff,0x44,0x44,0x7c,0x00,0x41,0x62,0x54,0xc8,0x54,0x62,0x41};
	uchar i;	
	print(0x1b);print(0x40);							//��ʼ��ָ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("ͼ�β��ԣ�\n");
	//			print(0x1b);print(0x57);print(2);		//��������Ŵ�2��
	print(0x1b);print(0x4b);print(15);print(0);		  	//ESC K����
	for(i=0;i<15;i++)									//����15���ֽ�����
		print(tch[i]);
	print(0x0d);										//����س�
	
}
/*************************����IIָ������**********************************/
//��������������ӡ��
/*void quxian_2(void)
{
 	float  X;
	unsigned int  Y,YY;
	//			Y1=50+40*abs��-0.01*X��*sin��X/10��
	//			Y2=50-40*abs��-0.01*X��*sin��X/10��
	//			Y3=50
	//			Y4=50+40*abs(-0.1*X)
	//			Y5=50-40*abs(-0.01*X)
	//			�������C���Ա�д���µĳ���
	//			#include <math.h>
	//			unsigned int Y,YY,X;
	print(0x1b);print(0x40);							//��ʼ��ָ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("����1���ԣ�\n");
	for(X=0;X<150;X++)									//��ӡ150����
	{
	  Y= 40*exp(-0.01*X);
	  YY= Y*sin(X/10);
	  print(0x1b);print(39);print(5);print(0);		    //ESC����,M=5,0
	  print(50+YY);print(0);print(50-YY);print(0);print(50);print(0);
	  print(50+Y);print(0);print(50-Y);print(0);print(13);
	  }
}
/*************************����Iָ������**********************************/
//����������ʽ��ӡ��
/*void quxian_1(void)
{
 	float  X;
	unsigned int  Y,YY;
	//			Y1=50+40*abs��-0.01*X��*sin��X/10��
	//			Y2=50-40*abs��-0.01*X��*sin��X/10��
	//			Y3=50
	//			Y4=50+40*abs(-0.1*X)
	//			Y5=50-40*abs(-0.01*X)
	//			�������C���Ա�д���µĳ���
	//			#include <math.h>
	//			unsigned int Y,YY,X;
	print(0x1b);print(0x40);							//��ʼ��ָ��
	print(0x1b);print(0x38);print(0x00);   	//����16*16�����֣�24����Ϊ24*24
	print_chs("����2���ԣ�\n");
	for(X=0;X<150;X++)									//��ӡ150����
	{
	  Y= 40*exp(-0.01*X);
	  YY= Y*sin(X/10);
	  print(0x1b);print(39);print(5);		    //ESC����,M=5
	  print(50+YY);;print(50-YY);print(50);;
	  print(50+Y);print(50-Y);print(13);
	  }
}

/************************��תָ������**********************************/
/*void xuanzhuan(void)
{
	uchar i;
	print(0x1b);print(0x40);							//��ʼ��ָ��
	for(i=0;i<4;i++)						 //0,90,180,270��ת
	{

		print(0x1c);print(0x49);print(i);				    //��תָ��
		print(0x1b);print(0x38);print(0);					//���ֵ���ָ��
		print_chs("��ת����");								//��ӡ����
	}	
	print(0x0d);										    //�س�
}

/************************����ָ������**********************************/
/*void fanbai(void)
{
	uchar i;
	print(0x1b);print(0x40);							//��ʼ��ָ��
	for(i=0;i<2;i++)						 //iΪ1ʱ�����״�ӡ
	{
		print(0x1b);print(0x69);print(i);
		print(0x1b);print(0x38);print(0);
		print_chs("���ײ���");	
	}	
	print(0x0d);

}
/************************����ָ������**********************************/
/*void fanxiang(void)
{
	uchar i;   
	print(0x1b);print(0x40);							//��ʼ��ָ��
	for(i=0;i<2;i++)						//iΪ1ʱ��������,������
	{
		print(0x1b);print(0x63);print(i);
		print(0x1b);print(0x38);print(0);
		print_chs("�������");	
		print(0x0d);
	}	
}
/************************����ָ������**********************************/
//ֻ�в����ͺŵĴ�ӡ��֧�ִ�ָ��
/*void tiaoma(void)
{  
	print(0x1b);print(0x40);							//��ʼ��ָ��
	print(0x1d);print(0x72);print(0x01);				//��ӡ�����ַ�
	print(0x1d);print(0x68);print(0x60);				//��������߶�Ϊ96��
	print(0x1d);print(0x6B);print(0x45);print(8);		//��ӡ����Ϊ8��CODE39��	
	print(0x2a);print(0x31);print(0x32);print(0x34);print(0x35);print(0x37);print(0x36);print(0x2a); //CODE39������
	print(0x0d);										//�س�
	print(0x1d);print(0x6B);print(3); 					//��ӡEAN13��	
	print(0x32);print(0x37);print(0x30);print(0x38);print(0x35);print(0x38);print(0x35);print(0x30); //EAN13������
	print(0x30);print(0x39);print(0x32);print(0x34);print(0x38);print(0x00);
	print(0x0d);										//�س�

}*/		 


















