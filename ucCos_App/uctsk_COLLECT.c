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

	I2C_Read(I2C1,ADDR_24C02,0,(u8 *)Holding.RegI,40);       
	ADS1115_WriteConfig();            //ADS1115_Init();
	
	Zero_GPIO_Configuration();
	
	Flag_Save = 0;       // ���������־
	timetemp = 0;
	
	sampleCount = 0;       // sample count init
	Sum_adc = 0;
	
	//Holding.RegS.Unit = Mpa;     // 
	
	TestTimesRecord = 0;
	
	strDisSta = Holding.RegS.FullScaleAD - Holding.RegS.ZeroScaleAD;    // ����У׼��ֵ��Ӧ����ֵ

	GuanTimes_Cache = Holding.RegS.GuanTimes;
	Guan_Cache = Holding.RegS.GuanCount;
	
	// ������������гֳ��ȶ�Ӧ������в���
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

		OSTimeDlyHMSM(0, 0, 0, 10);	        // ��ʱ10ms
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
	ADS1115_ReadConversion(adcBuf);            
	
	// ����ADS1115 ʵ�ʲɼ�ǿ��������û�и�ѹ�źţ�
	adcVal = adcBuf[0];
	if((adcVal & 0x80) == 0)    // ���λΪ1�ɼ�Ϊ���������ݶ��� 
	{
		adcVal = (adcVal << 8) + adcBuf[1];
		
		adcVal = adcVal >> 3;     // ��12λ
		
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
		timetemp++;
		if(timetemp >= 10)
		{
			timetemp = 0;
			Input.RegS.BreakTime++;
		}
		
		Input.RegS.Length = (LEAD * allmove * 10 / Holding.RegS.StepLong) >> 1;          // �����쳤
	  Input.RegS.Elongation = (Input.RegS.Length * 100) / Holding.RegS.CycleNum;       // �����쳤��
		
		if(allmove >= startforcetemp && startforceADtemp == 0)   // and only can calc one times    1%
		{
			startforceADtemp = Result_adcVal;   // record 1% AD
		}
		
		if(Result_adcVal > adcMax)
		{
			adcMax = Result_adcVal;           // ��¼�������ֵ
			
			// ����ʵʱ����
			DropTemp = ((adcMax - Holding.RegS.ZeroScaleAD) / 10) * (Holding.RegS.ForceDrop / 10);    // �������� ���ֵ����ٷֱ�
		}
		else
		{			
			// ����
			if((adcMax - Result_adcVal) >= difMax && (Result_adcVal - Holding.RegS.ZeroScaleAD) < DropTemp)  // ������С�ݲ�ң���ֵ��С�������� 
			{
				// ���Խ���
				if(ExamS_Flag == UP)
				{
					ExamS_Flag = DOWN;
				}
				else if(ExamS_Flag == DOWN && Action_Flag != EXAM_2)
				{
					Action_Flag = EXAM_2;            // ֹͣ		
					//Holding.RegS.SysCode = TexTestTimes;    // һ�β��Խ�����һ�ܲ��Խ�����־
					
					// ��ͣ���� (�쳤)
					TexMove = allmove;               // �����쳤��recorde ����������¼�����쳤��,���ڼ���
					Input.RegS.Length = (LEAD * TexMove * 10 / Holding.RegS.StepLong) >> 1;          // �����쳤
					//distemp = Input.RegS.Length * 100;     // �����м�ֵ
					Input.RegS.Elongation = (Input.RegS.Length * 100) / Holding.RegS.CycleNum;       // �����쳤��
					
					
					// ���ݼ�����λ���ɼ�AD����������������ʾֵ
	        distemp = adcMax - Holding.RegS.ZeroScaleAD;  // �����м�ֵ
	        if(distemp >= 65000)
	        {
		        distemp = 0;
 	        }
	        switch(Holding.RegS.Unit)      // ���ѣ���ʾ����ǿ��������ǿ�ȣ�ͳ����Ϣ     Holding.RegS.Standard ��λcn 
	        {
		        case Newton:
			        distemp = distemp * Holding.RegS.Standard / 10;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // ����ǿ��
						  Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // ����ǿ��
			        break;
		
		        case cNewton:
			        distemp = distemp * Holding.RegS.Standard * 10;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // ����ǿ��
						  Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // ����ǿ��
			        break;
		
		        case Kilogram:
			        distemp = distemp * Holding.RegS.Standard / 100;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // ����ǿ��
							Input.RegS.BreakingTenacity = Input.RegS.BreakingForce / Holding.RegS.TEX;   // ����ǿ��
			        break;
		
		        case Pound:
			        break;
						
						case Mpa:                                                  // 1N = 100cN   N/mm2 = Mpa
							distemp = distemp * Holding.RegS.Standard ;
			        Input.RegS.BreakingForce = distemp / strDisSta;          // ����ǿ��
						
						  // ��ʼģ��
						  distemp = startforceADtemp - Holding.RegS.ZeroScaleAD;   // �����м�ֵ
	            if(distemp >= 65000)distemp = 0;
						  distemp = distemp * Holding.RegS.Standard * 10;   // ��ʼģ��
						  Input.RegS.StartForce = distemp / strDisSta;        // ��ʼģ��
						  
              Mtemp = ((uint32_t)Input.RegS.BreakingForce & 0xffff);
							Mtemp = Mtemp * 10 / (Holding.RegS.TEX >> 1);
						  Mtemp = Mtemp * 10 / (Holding.RegS.TEX >> 1);
						  Mtemp = Mtemp / 314;
						
						  Input.RegS.BreakingTenacity = Mtemp;                // ����ǿ��
			      break;
	        }
					
					if(TestTimesRecord >= (Guan_Cache * GuanTimes_Cache))TestTimesRecord = 0;
					
					ResultTEX[TestTimesRecord][0] = Input.RegS.BreakingForce;    // ����ǿ��
					ResultTEX[TestTimesRecord][1] = Input.RegS.Length;           // �����쳤
					ResultTEX[TestTimesRecord][2] = Input.RegS.BreakingTenacity; // ����ǿ��
					ResultTEX[TestTimesRecord][3] = Input.RegS.Elongation;       // �����쳤��
					ResultTEX[TestTimesRecord][4] = Input.RegS.BreakTime;        // time
					ResultTEX[TestTimesRecord][5] = Input.RegS.StartForce;       // ��ʼģ��
					
					TestTimesRecord++;
				}
			}
		}
	}	
	
	// ���ݼ�����λ���ɼ�AD����������������ʾֵ
	distemp = Result_adcVal - Holding.RegS.ZeroScaleAD;  // �����м�ֵ
	if(distemp >= 60000)
	{
		distemp = 0;
	}
	// ��ʾĬ��һλ��Ч����
	// ����ʵʱ��ʾǿ��ֵ
	switch(Holding.RegS.Unit)   // ��ʾʵʱֵ��ǿ��ֵ������ֵ
	{
		case Newton:
			distemp = distemp * Holding.RegS.Standard / 10;
			Input.RegS.Force = distemp / strDisSta;          // ʵʱǿ��ֵ
			break;
		
		case cNewton:
			distemp = distemp * Holding.RegS.Standard * 10;   
			Input.RegS.Force = distemp / strDisSta;          // ʵʱǿ��ֵ
		
		  if(Input.RegS.Force < 150)Input.RegS.Force = 0;
		  if((Input.RegS.Force - Holding.RegS.Standard * 10) < 200 || (Holding.RegS.Standard * 10 - Input.RegS.Force) < 200)Input.RegS.Force = Holding.RegS.Standard * 10;
			break;
		
		case Kilogram:
			distemp = distemp * Holding.RegS.Standard / 100;
			Input.RegS.Force = distemp / strDisSta;          // ʵʱǿ��ֵ
			break;
		
		case Pound:
			break;
		
		case Mpa:
			distemp = distemp * Holding.RegS.Standard * 10;
			Input.RegS.Force = distemp / strDisSta;          // ʵʱǿ��ֵ
			break;
	}
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	     // ��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);	


	/***************************�����ⲿ�ж����ȼ�*******************************
	*					�������ȼ���KEY0=KEY1>WK_UP
	****************************************************************************/
	//�����ж����ȼ���2
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

	//����EXTI12 EXTI14���ȼ�	  
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn ;           //�ж�ͨ��
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


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

