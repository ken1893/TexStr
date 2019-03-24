/****************************************Copyright (c)****************************************************                                    
**                                 http://www.pgcc.top
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_Blink.c
** Descriptions:            The moving_Blink application function
**--------------------------------------------------------------------------------------------------------
** Created by:              ken
** Created date:            2019-3-9
** Version:                 v2.0
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/

#define BLINK_GLOBALS
#include <includes.h>            
//#include "GLCD.h"

/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void uctsk_Blink            (void);	  // Task

static void LED_GPIO_Configuration (void);	  // LED��ʾIO��ʼ��
static void PWM_GPIO_Configuration(void);     // �������IO��ʼ��
static void Tim3_Configuration(void);         // ʱ��3��ʼ�� PA6  TIM3_CH1
//static void Tim5_Configuration(void);       // TIM5_CH2
static void UART1_Configuration(INT32U baud_rate);		  //  

extern void RxIntEn (void);
void RunProcess(unsigned long AllRunStep,unsigned char StepW);

void  App_BlinkTaskCreate (void)
{
  CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task MOVING Blink", &os_err);
	#endif
}

//-----------------------------------------------------------------------------------------------------
//        Task for Motion
//
//-----------------------------------------------------------------------------------------------------
static void uctsk_Blink (void) 
{	
	uint8_t led_state;
	led_state = 1;
	
	allmove = 0;             
	Action_Flag = WAITING;            // ��ǰAction��־
	
  LED_GPIO_Configuration();
	PWM_GPIO_Configuration(); 
	
	Tim3_Configuration();
	UART1_Configuration(57600);
	
   	for(;;)
   	{
			if(led_state)
			{
				led_state = 0;
				GPIO_SetBits(GPIOE , GPIO_Pin_13);
			}
			else 
			{
				led_state = 1; 
				GPIO_ResetBits(GPIOE , GPIO_Pin_13);
			}

			// MOVING �˶�����
			if(Action_Flag != WAITING)
			{
				switch(Action_Flag)
				{
					case MOVEING_1:           // �������
						allmove = 0;
					  Current_Fre = 0;
				    TIM_SetAutoreload(TIM3, FreTab[0]);
					  GPIO_SetBits(GPIODIRA , DIRPORT);       
						ControlMove(ENABLE);
					  RecordAction = MOVEING_1;
						break;
					
					case MOVEING_2:           // ������㶯��2
						RunProcess(ZeroMove,100);
						allmove = 0;
					  Current_Fre = 0;
				    TIM_SetAutoreload(TIM3, FreTab[0]);
						GPIO_ResetBits(GPIODIRA , DIRPORT);
						ControlMove(ENABLE);
					  RecordAction = MOVEING_2;
						break;
					
					case MOTORDOWN:
						allmove = 0;
					  Current_Fre = 0;
				    TIM_SetAutoreload(TIM3, FreTab[0]);
					  GPIO_ResetBits(GPIODIRA , DIRPORT);       
						ControlMove(ENABLE);
					  RecordAction = MOTORDOWN;
						break;
				
					case STOP:                // ���ֹͣ
						ControlMove(DISABLE);
					  allmove = 0;
					  Current_Fre = 0;
					  TIM_SetAutoreload(TIM3, FreTab[0]);
					  RecordAction = STOP;
						break;
					
					// �Ƿ���Ҫ���ԣ��Ƿ񱾴β��Ի�δ���
					case EXAM_START:          // �յ���ʼ��������ж��Ƿ���ȷ���ò�������
						if(Holding.RegS.GuanCount != 0)
						{
							if(Holding.RegS.GuanTimes != 0)
							{
								Holding.RegS.GuanTimes--;
								if(Holding.RegS.GuanTimes == 0)
								{
									Holding.RegS.GuanCount--;
									if(Holding.RegS.GuanCount != 0)Holding.RegS.GuanTimes = GuanTimes_Cache;
								}
								Action_Flag = EXAM_1;   // �������
								RecordAction = EXAM_START;
							}
						}
						allmove = 0;
						break;
					
					case EXAM_1:                  // down��������
						TexMove = 0;
						
					  allmove = 0;
					  ExamS_Flag = UP;
					  Current_Fre = 0;
					
					  adcMax = 0;
					  Input.RegS.BreakingForce = 0;      
					  Input.RegS.BreakTime = 0;    // ����ʱ��
					  Input.RegS.Elongation = 0;
					  Input.RegS.Length = 0;
					
						TIM_SetAutoreload(TIM3, FreTab[0]);
						GPIO_ResetBits(GPIODIRA , DIRPORT);
						ControlMove(ENABLE);
					  RecordAction = EXAM_1;      // ��ʼ����
						break;
					
					case EXAM_2:
					  //ControlMove(DISABLE);       // ���Ӽ��ٹ���
					
						//backmove = allmove;         // ������ɣ��س����г���
					  //stepmove = 0;
					  //Action_Flag = EXAM_3;       // �������
					  RecordAction = EXAM_2;      
						break;
					
					case EXAM_3:                  // up���ϻص��������λ��
						RunProcess(backmove,100);
						allmove = 0;
					  Current_Fre = 0;
				    TIM_SetAutoreload(TIM3, FreTab[0]);
						GPIO_SetBits(GPIODIRA , DIRPORT);
						ControlMove(ENABLE);
					  RecordAction = EXAM_3;
						break;
					
					case ZEROSCALE:
						Holding.RegS.ZeroScaleAD = Input.RegS.STR_Result;
					  strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // ����У׼��ֵ��Ӧ����ֵ
					  Flag_Save = 1;
					  RecordAction = ZEROSCALE;
						break;
					
					case FULLSCALE:
						Holding.RegS.FullScaleAD = Input.RegS.STR_Result;
					  strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // ����У׼��ֵ��Ӧ����ֵ
					  Flag_Save = 1;
					  RecordAction = FULLSCALE;
						break;
				
					default:break;
				}
				
				if(RecordAction != EXAM_2 && RecordAction != EXAM_START)
				{
					Action_Flag = WAITING;
					
					if(RecordAction == MOVEING_1)     // �ж����
					{
						if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == 0)
	          {
							if(RecordAction != MOVEING_2)
		          {
								ControlMove(DISABLE);
		          }
		          if(RecordAction == MOVEING_1) // ���㶯��
		          {
								Action_Flag = MOVEING_2;
								allmove = 0;
		          }
	          }
					}
				}
			}
		
		OSTimeDlyHMSM(0, 0, 0, 50);	 /* 50 MS  */
   }
}

/*******************************************************************************
* Function Name  : PWM_GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void PWM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);					
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		

   // X��
  //  TIM3_CH1->PA6,TIM3_CH2->PA7   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // TIM3_CH3->PB0,TIM3_CH4->PB1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Tim3_Configuration
* Description    : Configure Timer
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void Tim3_Configuration(void)		   // PWM
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 						 
  
/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = FreTab[0];//�Զ���װ�ؼĴ������ڵ�ֵ  ����PWM���� fre 600
  TIM_TimeBaseStructure.TIM_Prescaler = 1; // 2��Ƶ    36M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

  // PWM2 Mode configuration: Channel1 
  TIM_OCInitStructure.TIM_Pulse = 50;    //CCR1_Val=
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  /*
  // PWM2 Mode configuration: Channel2 
  TIM_OCInitStructure.TIM_Pulse = 50;    //CCR2_Val=
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  // PWM2 Mode configuration: Channel3 
  TIM_OCInitStructure.TIM_Pulse = 50;    //CCR3_Val=
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  // PWM2 Mode configuration: Channel4 
  TIM_OCInitStructure.TIM_Pulse = 50;    //CCR4_Val=
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
*/
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
  //TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
  //TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
  //TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );  // ����TIM3 �����ж�

  //�ж����ȼ� NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            // TIM3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         // 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQ enable
  NVIC_Init(&NVIC_InitStructure);                            // NVIC
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);	 // ʹ�ܻ���ʧ��TIM3��ARR�ϵ�Ԥװ�ؼĴ���
	
	//TIM_CCxCmd(TIM3,TIM_Channel_1, ENABLE);
	//TIM_CCxCmd(TIM3,TIM_Channel_2, ENABLE);
	//TIM_CCxCmd(TIM3,TIM_Channel_3, ENABLE);
	//TIM_CCxCmd(TIM3,TIM_Channel_4, ENABLE);
	/* TIM3 enable counter */
  //TIM_Cmd(TIM3, ENABLE);
}



//------------------------------------------------------------------------------
//
// STart  OR STOP   MOVE 
//
//------------------------------------------------------------------------------
void ControlMove(FunctionalState C)
{
	TIM_CCxCmd(TIM3,TIM_Channel_1, C);
	//TIM_CCxCmd(TIM3,TIM_Channel_2, C);
	//TIM_CCxCmd(TIM3,TIM_Channel_3, C);
	//TIM_CCxCmd(TIM3,TIM_Channel_4, C);
		/* TIM3 enable/disable counter */
	TIM_Cmd(TIM3, C);
}


//------------------------------------------------------------------------------
//
// RunProcess(unsigned long AllRunStep,unsigned char StepW) 
//
//------------------------------------------------------------------------------
void RunProcess(unsigned long AllRunStep,unsigned char StepW)
{	
	//if(AllRunStep < StepW * RunSteps * 2)     // 
	if(AllRunStep < StepW * 86 * 2)
	{
		Target_Fre = AllRunStep / (StepW * 2);  //  Ŀ��̨����
		
		Mark_Pos = (AllRunStep >> 1) + AllRunStep % (StepW * 2);     //  ת�۵�
	}
	else
	{
			Target_Fre = 85;      //
			Mark_Pos = AllRunStep - StepW * 86;
	}
}

/*******************************************************************************
* Function Name  : LED_GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LED_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);    // dirA 
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE); 
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);		 // PC 6 7 dir		
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);	   // PE13 LED

  //  LED1 -> PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			     // ���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//  DIRA ->  PA7
	GPIO_InitStructure.GPIO_Pin = DIRPORT;			     // ���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIODIRA, &GPIO_InitStructure);
		
	
	// NOT USED
	
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	 // ����
  // GPIO_Mode_IPU   ��������  		GPIO_Mode_IPD  ��������
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	 // ��������
  //GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* PD.13(RS), PD.14(WR), PD.15(RD) */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*-------------------------------------------------------------------
   tim2    ����
--------------------------------------------------------------------*/


/*-------------------------------------------------------------------
   UART    ����
--------------------------------------------------------------------*/
void UART1_Configuration(INT32U baud_rate)
{
	  GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;	

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure USARTx_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USARTx_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = baud_rate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

	  RxIntEn();

    USART_Cmd(USART1, ENABLE);

    /// Configure the NVIC Preemption Priority Bits   
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);            //ѡ�����ȼ��� 
    /// Enable the USARTy Interrupt 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          //ѡ��USART1�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //�����ȼ�
 	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	           //ʹ���ж�
	            
    NVIC_Init(&NVIC_InitStructure);	
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval : None
  */



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
