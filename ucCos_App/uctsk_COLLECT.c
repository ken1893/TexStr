/****************************************Copyright (c)****************************************************                                     
**             KH
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_LWIP.c
** Descriptions:            The uctsk_COLLECT application function
                            
														��ֵ�ɼ������Խ���жϣ��ɼ�����100ms
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

static  void    Zero_GPIO_Configuration(void);         // ���0λ
static  void    vHandler_collect(void);

uint16_t strDisSta;    // ��������ֵ
uint8_t  sampleCount;   // ��������
uint16_t Result_adcVal; // ƽ�����ad���
uint32_t Sum_adc;       // ��ͽ��
uint16_t ArrayAdc[100]; // adc�������

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
	
	strDisSta = calcStand(Holding.RegS.Standard);    // ����У׼��ֵ��Ӧ����ֵ

	GuanTimes_Cache = Holding.RegS.GuanTimes;
	
	Holding.RegS.CycleNum  = 100;     // ����
	
	// ����гֳ��ȶ�Ӧ������в���
	ZeroMove = (Holding.RegS.CycleNum / LEAD) * Holding.RegS.StepLong * 2;
	
	//Holding.RegS.FreS      = 20;      // �����ٶ�
	//Holding.RegS.FreH9     = 20;      // �����ٶ�
	
	/**----------------------Read ID------------------------*/
	//p = readID();

	for(;;){
		vHandler_collect();
		
		if(Flag_Save == 1)
		{
			Flag_Save = 0;
			I2C_Write(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,30);
		}

		OSTimeDlyHMSM(0, 0, 0, 10);	        // ��ʱ20ms
    }
}

/*******************************************************************************
* Function Name  : vHandler_collect
* Description    : ������ѹ�ɼ�����
* Input          : none
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
static void vHandler_collect(void)
{
	uint32_t distemp;           // ��ʾ����
	
	ADS1115_ReadConversion(adcBuf);            
	
	// ����ADS1115 ʵ�ʲɼ�ǿ��������û�и�ѹ�źţ�
	adcVal = adcBuf[0];
	if((adcVal & 0x80) == 0)    // ���λΪ1�ɼ�Ϊ���������ݶ��� 
	{
		adcVal = (adcVal << 8) + adcBuf[1];
		
		Sum_adc = Sum_adc + adcVal - ArrayAdc[sampleCount];  // ���±��βɼ������Ľ����
		ArrayAdc[sampleCount] = adcVal;                      // ���±��βɼ����������
		Result_adcVal = Sum_adc / 100;
		
		if(sampleCount < 99){sampleCount++;}
		else{sampleCount = 0;}
	}
	else adcVal = 0;
	
	Input.RegS.STR_Result = Result_adcVal;
	
	// ���Թ������ж�
	if(RecordAction == EXAM_1)     // ���ڲ����У�����ɼ����ݴ����ֵ���µ��䣬����Խ������ص��������
	{
		Input.RegS.BreakTime++;
		if(Result_adcVal > adcMax)
		{
			adcMax = Result_adcVal;           // ��¼�������ֵ
			Input.RegS.STR_Max = adcMax;
		}
		else
		{			
			// ���������ֵ�뵱ǰ�ɼ�ֵ֮����Ҫ�����������ƫ��ֵ
			// ������λ�����Խ�������ֵ����25%���ж�Ϊ���Խ���
			if((adcMax - Result_adcVal) >= difMax && (adcMax - Result_adcVal) >= (difMax >> 2))   
			{
				// ���Խ���
				if(ExamS_Flag == UP)
				{
					ExamS_Flag = DOWN;
				}
				else 
				{
					Action_Flag = EXAM_2;            // ֹͣ		
					
					// ��ͣ���� (�쳤)
					TexMove = allmove;               // �����쳤��recorde ����������¼�����쳤��,���ڼ���
					Input.RegS.Length = (LEAD * TexMove * 100 / Holding.RegS.StepLong) >> 1;      // �쳤
				}
			}
		}
	}
	
	// ���ݼ�����λ���ɼ�AD����������������ʾֵ
	switch(Holding.RegS.Unit)
	{
		case Newton:
			distemp = strDisSta * Result_adcVal;
			Input.RegS.BreakingForce = distemp / Input.RegS.FullScaleAD;          // ����ǿ��
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
   Thunder GPIO    ����    PC4 5  �ⲿ�ж�
--------------------------------------------------------------------*/
static void Zero_GPIO_Configuration(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//������ʱ��  GPIOC
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//�򿪸���ʱ�� ��Ϊ�ж�����IO�ڸ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;       // PC2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	             // ��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);	


	/***************************�����ⲿ�ж����ȼ�*******************************
	*					�������ȼ���KEY0=KEY1>WK_UP
	****************************************************************************/
	//�����ж����ȼ���2
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

	//����EXTI12 EXTI14���ȼ�	  
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn ;          //�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  2;  //ǿռ���ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //�����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //ͨ���ж�ʹ��
  	NVIC_Init(&NVIC_InitStructure);                             //��ʼ���ж�
		

	/*********************�����ⲿ�ж�****************************
	*  EXTI4 	PC4 	 	�½��ش���
	* 
	*************************************************************/
	//�����ⲿ�ж��� ��EXTI0 ���ӵ�PC0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	
	
	//����EXTI0��EXTI15 �½��ش����ж�
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
			stdtemp = (selAD * 980);    // С�����2λ
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

