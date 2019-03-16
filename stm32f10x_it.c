/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include "stm32f10x_it.h"


#if ( OS_VIEW_MODULE == DEF_ENABLED )
extern	void OSView_RxTxISRHandler(void);
#endif

extern void  RxISRHandler(void);


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	CPU_SR         cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                     */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR        */
}


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USART1_IRQHandler(void)
{
	#if ( OS_VIEW_MODULE == DEF_ENABLED )
		OSView_RxTxISRHandler();
	#endif

	RxISRHandler();
}

//----------------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------------
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)			 //  TIM_interrupt_sources
	{
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);		   //  ���ж� TIM_Flags
	}	
}

//��ʱ��3 �жϷ������
void TIM3_IRQHandler(void) //TIM3 
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���TIM3�ж��Ƿ���
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );     // ���TIM3 �����жϱ�־ 
		
		// MOVE CODE 
		stepmove++;        // �������̨�׼���
		allmove++;         // ������������
		
		// �ж϶���
		switch(RecordAction)
		{
			case MOVEING_1:
				if(stepmove >= 100)        // steps
				{
					stepmove = 0;
					if(Current_Fre <= Holding.RegS.FreS)TIM_SetAutoreload(TIM3, FreTab[Current_Fre++]);     // �����ٶ�
				}
				break;
			
			case MOVEING_2:              // ���ص����Լг־���ֹͣ�����ж�
				if(stepmove >= 100)        // steps
				{
					stepmove = 0;
					if(allmove < Mark_Pos)   // speed up
					{
						if(Current_Fre < Target_Fre)TIM_SetAutoreload(TIM3, FreTab[Current_Fre++]);
				  }
					else                     // speed down
					{
						if(Current_Fre > 0)TIM_SetAutoreload(TIM3, FreTab[Current_Fre--]);
					}
				}
				if(allmove >= ZeroMove)    // �г־��룬ָ����λ�����Ե����
				{
					ControlMove(DISABLE);
					allmove = 0;
					stepmove = 0;
				}
				break;
				
			case EXAM_1:                 // �������������мӼ��ٿ���
				if(stepmove >= 100)        // steps
				{
					stepmove = 0;
					if(ExamS_Flag == UP)     // speed up
					{
						if(Current_Fre < Holding.RegS.FreH9)TIM_SetAutoreload(TIM3, FreTab[Current_Fre++]);    // �����ٶ�
				  }
					else                     // speed down
					{
						if(Current_Fre > 0)TIM_SetAutoreload(TIM3, FreTab[Current_Fre--]);
					}
				}
				break;
				
			case EXAM_2:
				if(stepmove >= 100)        // steps
				{
					stepmove = 0;
					if(Current_Fre > 0)
					{
						TIM_SetAutoreload(TIM3, FreTab[Current_Fre--]);
					}
					else 
					{
						ControlMove(DISABLE);       // ���Ӽ��ٹ���
						backmove = allmove;         // ������ɣ��س����г���
					  Action_Flag = EXAM_3;       // �������
					}
				}
				break;
			
			case EXAM_3:                 // ���Խ���step3���ص����Լг־���ֹͣ�����ж�
				if(stepmove >= 100)        // steps
				{
					stepmove = 0;
					if(allmove < Mark_Pos)   // speed up
					{
						if(Current_Fre < Target_Fre)TIM_SetAutoreload(TIM3, FreTab[Current_Fre++]);
				  }
					else                     // speed down
					{
						if(Current_Fre > 0)TIM_SetAutoreload(TIM3, FreTab[Current_Fre--]);
					}
				}
				if(allmove >= backmove)            // test stop�����ɲ��Թ����вɼ��õ�,ÿ�β��Զ��в��ÿ�ν�������
				{
					Action_Flag = WAITING;           // ֹͣ
					ControlMove(DISABLE);
				  allmove = 0;
					backmove = 0;
					stepmove = 0; 
				}
				break;
			
			default:
				break;
		}
		
		// for�˶����߿���
	}
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles EXTI15_10_IRQ global interrupt request. // �ⲿ�ж�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == 0)
	{
		if(RecordAction != MOVEING_2)
		{
			// user code 
			ControlMove(DISABLE);
		}
		
		if(RecordAction == MOVEING_1)     // ���㶯��
		{
			Action_Flag = MOVEING_2;
			allmove = 0; 
		}
	}

	EXTI_ClearITPendingBit(EXTI_Line4);   	//�������жϹ���λ
}

/*
void EXTI5_10_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == 0)
	{
		if(RecordAction != MOVEING_2)
		{
			// user code 
			ControlMove(DISABLE);
		}
		
		if(RecordAction == MOVEING_1)     // ���㶯��
		{
			Action_Flag = MOVEING_2;
			allmove = 0; 
		}
	}

	//�������жϹ���λ
	EXTI_ClearITPendingBit(EXTI_Line5);
}
*/

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
