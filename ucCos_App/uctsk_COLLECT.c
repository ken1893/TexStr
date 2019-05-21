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

	I2C_Read(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,40);       
	ADS1115_WriteConfig();            //ADS1115_Init();
	
	Zero_GPIO_Configuration();
	
	Flag_Save = 0;       // 保存参数标志
	timetemp = 0;
	
	sampleCount = 0;       // sample count init
	Sum_adc = 0;
	
	//Holding.RegS.Unit = Mpa;     // 
	
	TestTimesRecord = 0;
	
	strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // 计算校准力值对应计算值

	GuanTimes_Cache = Holding.RegS.GuanTimes;
	Guan_Cache = Holding.RegS.GuanCount;
	
	// 计算回零距离减夹持长度对应电机运行步数
	if(Holding.RegS.Distance != 0)
	{
		ZeroMove = ((Holding.RegS.CycleNum - (Holding.RegS.Distance - Holding.RegS.CycleNum))) * Holding.RegS.StepLong * 2  / LEAD;
	}
	else 
	{
		ZeroMove = ((Holding.RegS.CycleNum)) * Holding.RegS.StepLong * 2  / LEAD;
	}
	
	startforcetemp = ZeroMove / 100;

	for(;;){
		vHandler_collect();
		
		if(Flag_Save == 1)
		{
			Flag_Save = 0;
			I2C_Write(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,40);
		}

		OSTimeDlyHMSM(0, 0, 0, 10);	        // 延时10ms
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
	ADS1115_ReadConversion(adcBuf);            
	
	// 采用ADS1115 实际采集强力传感器没有负压信号，
	adcVal = adcBuf[0];
	if((adcVal & 0x80) == 0)    // 最高位为1采集为负数，数据丢弃 
	{
		adcVal = (adcVal << 8) + adcBuf[1];
		
		adcVal = adcVal >> 3;     // 变12位
		
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
		timetemp++;
		if(timetemp >= 10)
		{
			timetemp = 0;
			Input.RegS.BreakTime++;
		}
		
		Input.RegS.Length = (LEAD * allmove * 10 / Holding.RegS.StepLong) >> 1;          // 断裂伸长
	  Input.RegS.Elongation = (Input.RegS.Length * 100) / Holding.RegS.CycleNum;       // 断裂伸长率
		
		if(allmove >= startforcetemp && startforceADtemp == 0)   // and only can calc one times    1%
		{
			startforceADtemp = Result_adcVal;   // record 1% AD
		}
		
		if(Result_adcVal > adcMax)
		{
			adcMax = Result_adcVal;           // 记录单次最大值
			
			// 力降实时更新
			DropTemp = ((adcMax - Holding.RegS.ZeroScaleAD) / 10) * (Holding.RegS.ForceDrop / 10);    // 计算力降 最大值跌落百分比
		}
		else
		{			
			// 力降
			if((adcMax - Result_adcVal) >= difMax && (Result_adcVal - Holding.RegS.ZeroScaleAD) < DropTemp)  // 大于最小容差并且，力值减小大于力降 
			{
				// 测试结束
				if(ExamS_Flag == UP)
				{
					ExamS_Flag = DOWN;
				}
				else if(ExamS_Flag == DOWN && Action_Flag != EXAM_2)
				{
					Action_Flag = EXAM_2;            // 停止		
					//Holding.RegS.SysCode = TexTestTimes;    // 一次测试结束或一管测试结束标志
					
					// 拉停长度 (伸长)
					TexMove = allmove;               // 拉断伸长，recorde 脉冲数，记录测试伸长量,用于计算
					Input.RegS.Length = (LEAD * TexMove * 10 / Holding.RegS.StepLong) >> 1;          // 断裂伸长
					//distemp = Input.RegS.Length * 100;     // 计算中间值
					Input.RegS.Elongation = (Input.RegS.Length * 100) / Holding.RegS.CycleNum;       // 断裂伸长率
					
					
					// 根据计量单位，采集AD结果，计算出重力表示值
	        distemp = adcMax - Holding.RegS.ZeroScaleAD;  // 计算中间值
	        if(distemp >= 65000)
	        {
		        distemp = 0;
 	        }
	        switch(Holding.RegS.Unit)      // 断裂，显示断裂强力，断裂强度，统计信息     Holding.RegS.Standard 单位cn 
	        {
		        case Newton:
			        distemp = distemp * Holding.RegS.Standard / 10;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // 断裂强力
						  Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // 断裂强度
			        break;
		
		        case cNewton:
			        distemp = distemp * Holding.RegS.Standard * 10;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // 断裂强力
						  Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // 断裂强度
			        break;
		
		        case Kilogram:
			        distemp = distemp * Holding.RegS.Standard / 100;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // 断裂强力
							Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // 断裂强度
			        break;
		
		        case Pound:
			        break;
						
						case Mpa:                                                  // 1N = 100cN   N/mm2 = Mpa
							distemp = distemp * Holding.RegS.Standard ;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // 断裂强力
						
						  // 初始模量
						  distemp = startforceADtemp - Holding.RegS.ZeroScaleAD;   // 计算中间值
	            if(distemp >= 65000)distemp = 0;
						  distemp = distemp * Holding.RegS.Standard * 10;   // 初始模量
						  Input.RegS.StartForce = distemp / strDisSta;        // 初始模量
						  
              Mtemp = ((uint32_t)Input.RegS.BreakingForce & 0xffff);
							Mtemp = Mtemp * 10 / (Holding.RegS.TEX >> 1);
						  Mtemp = Mtemp * 10 / (Holding.RegS.TEX >> 1);
						  Mtemp = Mtemp / 314;
						
						  Input.RegS.BreakingTenacity = Mtemp;                // 断裂强度
			      break;
	        }
					
					if(TestTimesRecord >= (Guan_Cache * GuanTimes_Cache))TestTimesRecord = 0;
					
					ResultTEX[TestTimesRecord][0] = Input.RegS.BreakingForce;    // 断裂强力
					ResultTEX[TestTimesRecord][1] = Input.RegS.Length;           // 断裂伸长
					ResultTEX[TestTimesRecord][2] = Input.RegS.BreakingTenacity; // 断裂强度
					ResultTEX[TestTimesRecord][3] = Input.RegS.Elongation;       // 断裂伸长率
					ResultTEX[TestTimesRecord][4] = Input.RegS.BreakTime;        // time
					ResultTEX[TestTimesRecord][5] = Input.RegS.StartForce;       // 初始模量
					
					TestTimesRecord++;
				}
			}
		}
	}	
	
	// 根据计量单位，采集AD结果，计算出重力表示值
	distemp = Result_adcVal - Holding.RegS.ZeroScaleAD;  // 计算中间值
	if(distemp >= 60000)
	{
		distemp = 0;
	}
	// 显示默认一位有效数字
	// 以下实时显示强力值
	switch(Holding.RegS.Unit)   // 显示实时值，强力值，拉伸值
	{
		case Newton:
			distemp = distemp * Holding.RegS.Standard / 10;
			Input.RegS.Force = distemp / strDisSta;          // 实时强力值
			break;
		
		case cNewton:
			distemp = distemp * Holding.RegS.Standard * 10;   
			Input.RegS.Force = distemp / strDisSta;          // 实时强力值
		
		  if(Input.RegS.Force < 150)Input.RegS.Force = 0;
		  if((Input.RegS.Force - Holding.RegS.Standard * 10) < 200 || (Holding.RegS.Standard * 10 - Input.RegS.Force) < 200)Input.RegS.Force = Holding.RegS.Standard * 10;
			break;
		
		case Kilogram:
			distemp = distemp * Holding.RegS.Standard / 100;
			Input.RegS.Force = distemp / strDisSta;          // 实时强力值
			break;
		
		case Pound:
			break;
		
		case Mpa:
			distemp = distemp * Holding.RegS.Standard * 10;
			Input.RegS.Force = distemp / strDisSta;          // 实时强力值
			break;
	}
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	     // 上拉输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);	


	/***************************配置外部中断优先级*******************************
	*					按键优先级：KEY0=KEY1>WK_UP
	****************************************************************************/
	//配置中断优先级组2
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

	//设置EXTI12 EXTI14优先级	  
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn ;           //中断通道
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


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

