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

static void LED_GPIO_Configuration (void);	  // LED显示IO初始化
static void PWM_GPIO_Configuration(void);     // 脉冲输出IO初始化
static void Tim3_Configuration(void);         // 时钟3初始化 PA6  TIM3_CH1
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
	Action_Flag = WAITING;            // 当前Action标志
	
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

			// MOVING 运动流程
			if(Action_Flag != WAITING)
			{
				switch(Action_Flag)
				{
					case MOVEING_1:           // 电机动作
						allmove = 0;
					  Current_Fre = 0;
				    TIM_SetAutoreload(TIM3, FreTab[0]);
					  GPIO_SetBits(GPIODIRA , DIRPORT);       
						ControlMove(ENABLE);
					  RecordAction = MOVEING_1;
						break;
					
					case MOVEING_2:           // 电机回零动作2
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
				
					case STOP:                // 电机停止
						ControlMove(DISABLE);
					  allmove = 0;
					  Current_Fre = 0;
					  TIM_SetAutoreload(TIM3, FreTab[0]);
					  RecordAction = STOP;
						break;
					
					// 是否需要测试，是否本次测试还未完成
					case EXAM_START:          // 收到开始测试命令，判断是否正确设置测试数据
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
								Action_Flag = EXAM_1;   // 进入测试
								RecordAction = EXAM_START;
							}
						}
						allmove = 0;
						break;
					
					case EXAM_1:                  // down向下拉伸
						TexMove = 0;
						
					  allmove = 0;
					  ExamS_Flag = UP;
					  Current_Fre = 0;
					
					  adcMax = 0;
					  Input.RegS.BreakingForce = 0;      
					  Input.RegS.BreakTime = 0;    // 断裂时间
					  Input.RegS.Elongation = 0;
					  Input.RegS.Length = 0;
					
						TIM_SetAutoreload(TIM3, FreTab[0]);
						GPIO_ResetBits(GPIODIRA , DIRPORT);
						ControlMove(ENABLE);
					  RecordAction = EXAM_1;      // 开始测试
						break;
					
					case EXAM_2:
					  //ControlMove(DISABLE);       // 增加减速过程
					
						//backmove = allmove;         // 测试完成，回程运行长度
					  //stepmove = 0;
					  //Action_Flag = EXAM_3;       // 进入测试
					  RecordAction = EXAM_2;      
						break;
					
					case EXAM_3:                  // up向上回到拉伸起点位置
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
					  strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // 计算校准力值对应计算值
					  Flag_Save = 1;
					  RecordAction = ZEROSCALE;
						break;
					
					case FULLSCALE:
						Holding.RegS.FullScaleAD = Input.RegS.STR_Result;
					  strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // 计算校准力值对应计算值
					  Flag_Save = 1;
					  RecordAction = FULLSCALE;
						break;
				
					default:break;
				}
				
				if(RecordAction != EXAM_2 && RecordAction != EXAM_START)
				{
					Action_Flag = WAITING;
					
					if(RecordAction == MOVEING_1)     // 判断零点
					{
						if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == 0)
	          {
							if(RecordAction != MOVEING_2)
		          {
								ControlMove(DISABLE);
		          }
		          if(RecordAction == MOVEING_1) // 回零动作
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

   // X轴
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
  TIM_TimeBaseStructure.TIM_Period = FreTab[0];//自动重装载寄存器周期的值  即是PWM方波 fre 600
  TIM_TimeBaseStructure.TIM_Prescaler = 1; // 2分频    36M
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
  
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );  // 允许TIM3 更新中断

  //中断优先级 NVIC 设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            // TIM3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         // 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQ enable
  NVIC_Init(&NVIC_InitStructure);                            // NVIC
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);	 // 使能或者失能TIM3在ARR上的预装载寄存器
	
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
		Target_Fre = AllRunStep / (StepW * 2);  //  目标台阶数
		
		Mark_Pos = (AllRunStep >> 1) + AllRunStep % (StepW * 2);     //  转折点
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			     // 输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//  DIRA ->  PA7
	GPIO_InitStructure.GPIO_Pin = DIRPORT;			     // 输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIODIRA, &GPIO_InitStructure);
		
	
	// NOT USED
	
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	 // 输入
  // GPIO_Mode_IPU   输入上拉  		GPIO_Mode_IPD  输入下拉
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	 // 浮空输入
  //GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* PD.13(RS), PD.14(WR), PD.15(RD) */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*-------------------------------------------------------------------
   tim2    配置
--------------------------------------------------------------------*/


/*-------------------------------------------------------------------
   UART    配置
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
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);            //选择优先级组 
    /// Enable the USARTy Interrupt 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          //选择USART1中断
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //主优先级
 	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         //次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	           //使能中断
	            
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
