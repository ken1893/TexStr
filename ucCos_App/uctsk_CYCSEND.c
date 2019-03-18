/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_CYCSEND.c
** Descriptions:            The uctsk_CYCSEND application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-10-29
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#define CYCSEND_GLOBALS
/* Includes ------------------------------------------------------------------*/

#include <includes.h> 

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/* Private variables ---------------------------------------------------------*/
static  OS_STK  AppCYCSENDTaskStk[APP_TASK_CYCSEND_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_CYCSEND  (void);

void usartsend_pros(void);    // send common 串口发送
void stuff_TxBuf(void);       // 装载窗口发送数据，主动发送状态装包
extern void UartDeal(void);   // 接收完成，处理数据包
extern void Send_CommonReg(u8* pBuffer, u8 Num);     // 发送寄存器

void  App_CYCSENDTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_CYCSEND,				
                          (void          * ) 0,							
                          (OS_STK        * )&AppCYCSENDTaskStk[APP_TASK_CYCSEND_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_CYCSEND_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_CYCSEND_PRIO, "Task CYCSEND", &os_err);
	#endif
}

static void uctsk_CYCSEND(void)
{
	Holding.RegS.End_ID = 1;
  
	for(;;){
		if(RxTimeCnt >= 1){    // 接收到第一个合法字节，开始计时
		
			RxTimeCnt++;
			if(RxTimeCnt >= 3){	  // 超时			
				RxTimeCnt = 0;
				RxState = 0;	
			}
		}
 
		// 串口数据包接收完成处理，并发送数据
		if(pkt_Flag == 1){    // 数据接收完毕，处理数据包	(解析)
			UartDeal();         // 解析
			usartsend_pros();   // 解析完成发送数据
			
			pkt_Flag = 0;       // 处理结束
		}
		
		// 定时主动发送 控制软件端发送启动命令
		ComCounter_Timer++;
		if(ComCounter_Timer >= 10){     // 定时发送,正在接收状态则等待处理接收完毕   && RxState == RX_STATE_ID
			ComCounter_Timer = 0;
			//stuff_TxBuf();				
		}
		OSTimeDlyHMSM(0, 0, 0, 250);	        // 延时250ms
    }
}

// 装载窗口发送数据，主动发送状态
void stuff_TxBuf(void)
{
	TxBuf[5] = (Input.RegS.STR_Result >> 8) & 0xff;
	TxBuf[6] = Input.RegS.STR_Result & 0xff;
	

	// 未发送
	Send_CommonReg(TxBuf,19);	  // 计算校验
			
	usartsend_pros();           // send common 串口发送
}

//------------------------------------串口发送---------------------------------------
//   调用主动发送
//
//
/////////////////////////////////////////////////////////////////////////////////////
void usartsend_pros(void)    
{
	uint8_t i;

	if(TxEnable_Flag == 1){
		for(i=0;i< TxLen;i++){
			USART_SendData(USART1,TxBuf[i]);	
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		}
			
		TxEnable_Flag = 0;       // 发送完毕，关闭发送使能
	}	
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

