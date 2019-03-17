/****************************************Copyright (c)****************************************************                                     
**             KH
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_LWIP.c
** Descriptions:            The uctsk_COLLECT application function
                            
														力值采集，测试结果判断，采集周期100ms
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2019-3-2
** Version:                 v1.0
** Descriptions:            The original version
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
*********************************************************************************************************/
#define COLLECT_GLOBALS
/* Includes ------------------------------------------------------------------*/

#include <includes.h> 
#include "ads1115.h"
#include "24C16.h"

/* Private variables ---------------------------------------------------------*/
static  OS_STK  AppCOLLECTTaskStk[APP_TASK_COLLECT_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_COLLECT  (void *pdata);

static  void    Zero_GPIO_Configuration(void);         // 检测0位
static  void    vHandler_collect(void);

uint16_t strDisSta;    // 计量计算值
uint8_t  sampleCount;   // 采样次数
uint16_t Result_adcVal; // 平均后的ad结果
uint32_t Sum_adc;       // 求和结果
uint16_t ArrayAdc[100]; // adc结果数组

void  App_COLLECTTaskCreate (void)
{
  CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_COLLECT,				
                          (void          * ) 0,							
                          (OS_STK        * )&AppCOLLECTTaskStk[APP_TASK_COLLECT_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_COLLECT_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_COLLECT_PRIO, "Task COLLECT", &os_err);
	#endif
}

static  void    uctsk_COLLECT(void *pdata)
{
		/* Initialize the I2C EEPROM driver ----------------------------------------*/
	I2C_Configuration();
	
	I2C_Read(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,30);
	//ADS1115_Init();

	Zero_GPIO_Configuration();       

	ADS1115_WriteConfig();
	//ADC_Configuration();
	
	Flag_Save = 0;
	
	sampleCount = 0;       // sample count init
	Sum_adc = 0;
	#ifdef TEST
  countad = 0;   // 
  #endif 
	
	Holding.RegS.Unit = Newton;     // 
	
	strDisSta = calcStand(Holding.RegS.Standard);    // 计算校准力值对应计算值

	GuanTimes_Cache = Holding.RegS.GuanTimes;
	
	Holding.RegS.CycleNum  = 100;     // 距离
	
	// 计算夹持长度对应电机运行步数
	ZeroMove = (Holding.RegS.CycleNum / LEAD) * Holding.RegS.StepLong * 2;
	
	//Holding.RegS.FreS      = 20;      // 回零速度
	//Holding.RegS.FreH9     = 20;      // 测试速度
	
	/**----------------------Read ID------------------------*/
	//p = readID();

	for(;;){
		vHandler_collect();
		
		if(Flag_Save == 1)
		{
			Flag_Save = 0;
			I2C_Write(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,30);
		}

		OSTimeDlyHMSM(0, 0, 0, 10);	        // 延时20ms
    }
}

/*******************************************************************************
* Function Name  : vHandler_collect
* Description    : 电流电压采集处理
* Input          : none
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
static void vHandler_collect(void)
{
	uint32_t distemp;           // 显示缓存
	
	ADS1115_ReadConversion(adcBuf);            
	
	// 采用ADS1115 实际采集强力传感器没有负压信号，
	adcVal = adcBuf[0];
	if((adcVal & 0x80) == 0)    // 最高位为1采集为负数，数据丢弃 
	{
		adcVal = (adcVal << 8) + adcBuf[1];
		
		Sum_adc = Sum_adc + adcVal - ArrayAdc[sampleCount];  // 更新本次采集结果后的结果和
		ArrayAdc[sampleCount] = adcVal;                      // 更新本次采集结果到数组
		Result_adcVal = Sum_adc / 100;
		
		if(sampleCount < 99){sampleCount++;}
		else{sampleCount = 0;}
	}
	else adcVal = 0;
	
	Input.RegS.STR_Result = Result_adcVal;
	
	// 测试过程中判断
	if(RecordAction == EXAM_1)     // 正在测试中，如果采集数据从最大值向下跌落，则测试结束，回到测试起点
	{
		Input.RegS.BreakTime++;
		if(Result_adcVal > adcMax)
		{
			adcMax = Result_adcVal;           // 记录单次最大值
			Input.RegS.STR_Max = adcMax;
		}
		else
		{			
			// 并且最大车辆值与当前采集值之差需要大于最大允许偏差值
			// 右移两位，测试结果比最大值跌落25%，判断为测试结束
			if((adcMax - Result_adcVal) >= difMax && (adcMax - Result_adcVal) >= (difMax >> 2))   
			{
				// 测试结束
				if(ExamS_Flag == UP)
				{
					ExamS_Flag = DOWN;
				}
				else 
				{
					Action_Flag = EXAM_2;            // 停止		
					
					// 拉停长度 (伸长)
					TexMove = allmove;               // 拉断伸长，recorde 脉冲数，记录测试伸长量,用于计算
					Input.RegS.Length = (LEAD * TexMove * 100 / Holding.RegS.StepLong) >> 1;      // 伸长
				}
			}
		}
	}
	
	// 根据计量单位，采集AD结果，计算出重力表示值
	switch(Holding.RegS.Unit)
	{
		case Newton:
			distemp = strDisSta * Result_adcVal;
			Input.RegS.BreakingForce = distemp / Input.RegS.FullScaleAD;          // 断裂强力
			break;
		
		case cNewton:
			break;
		
		case Kilogram:
			break;
		
		case Pound:
			break;
	}
	
	#ifdef TEST
	if(RecordAction == EXAM_1)
	{
		if(countad < 10)
		{
			//Holding.RegS.ADres[countad] = adcVal; //
			countad++;   // 
		}
		else 
		{
			//printf("D0:%d,D1:%d,D2:%d,D2:%d,D4:%d,D5:%d,D6:%d,D7:%d,D8:%d,D9:%d\r\n", CoAD[0],CoAD[1],CoAD[2],CoAD[3],CoAD[4],CoAD[5],CoAD[6],CoAD[7],CoAD[8],CoAD[9]);
			countad = 0;
			//Holding.RegS.ADres[countad] = adcVal; //
		}
	}
  #endif 
}





/*-------------------------------------------------------------------
   Thunder GPIO    配置    PC4 5  外部中断
--------------------------------------------------------------------*/
static void Zero_GPIO_Configuration(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//打开外设时钟  GPIOC
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//打开复用时钟 因为中断线与IO口复用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;       // PC2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	             // 上拉输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);	


	/***************************配置外部中断优先级*******************************
	*					按键优先级：KEY0=KEY1>WK_UP
	****************************************************************************/
	//配置中断优先级组2
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

	//设置EXTI12 EXTI14优先级	  
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn ;          //中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  2;  //强占优先级
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //次优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //通道中断使能
  	NVIC_Init(&NVIC_InitStructure);                             //初始化中断
		

	/*********************配置外部中断****************************
	*  EXTI4 	PC4 	 	下降沿触发
	* 
	*************************************************************/
	//配置外部中断线 把EXTI0 连接到PC0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	
	
	//配置EXTI0和EXTI15 下降沿触发中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
}

//--------------------------------------------------------------------
uint16_t calcStand(uint16_t selAD)
{	
	uint16_t stdtemp;
	
	switch(Holding.RegS.Unit)
	{
		case Newton:
			stdtemp = (selAD * 980);    // 小数点后2位
		  break;
		
		case cNewton:
			break;
		
		case Kilogram:
			break;
		
		case Pound:
			break;
	}
	
	return stdtemp;
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

