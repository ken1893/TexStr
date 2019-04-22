/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               includes.h
** Descriptions:            includes.h for ucos
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
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

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/* Includes ------------------------------------------------------------------*/

//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>

#include  <uCOS-II\Source\ucos_ii.h>
#include  <uCOS-II\Ports\ARM-Cortex-M3\RealView\os_cpu.h>
#include  <uC-CPU\ARM-Cortex-M3\RealView\cpu.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#if (OS_VIEW_MODULE == DEF_ENABLED)
#include    <uCOS-VIEW\Ports\ARM-Cortex-M3\Realview\os_viewc.h>
#include    <uCOS-VIEW\Source\os_view.h>
#endif

#include  <stm32f10x.h>
#include  <com.h>

#include "ads1115.h"


#define LEAD        5 
#define difMax      100       // �ݲ�
#define PWMCORECLK  36000000


// �豸����
enum ActionType {
    WAITING,        // ϵͳ�ȴ�
    MOVEING_1,      // �������,���У��    1
	  MOVEING_2,      // �������,���У��    2
	
    ZEROSCALE,      // AD���У��           3
	  FULLSCALE,      // AD������             4
	
		EXAM_START,     // ���� examination     5
	  EXAM_1,         // E1               
	  EXAM_2,         // E2   
	  EXAM_3,         // E3  
	  STOP,           // STOP                 9
	  MOTORDOWN,      // DOWN                 10
	
	  PRINT           // print                11
};

// ��λѡ��
enum UnitType {
	Newton = 1,          // N  1
	cNewton,             // cN
	Kilogram,            // kg
	Pound,               // lb
	Mpa                  // 
};

enum TimesType {
	NoTest = 0,
	Once = 1,          // once
	Last = 3           // last
};


/* Time constant for the delay caclulation allowing to have a millisecond 
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 24MHz then ADS1115_TIME_CONST should be 24. */
#define ADS1115_TIME_CONST          24 

#ifdef MAIN_GLOBALS
#define MAIN_EXT
#else
#define MAIN_EXT extern
#endif

union HOLDREG_U
{
	u16 RegI[20];
	struct HONDREG_S
	{ 
		u16 TEX;                // ���ܶ� density                    0		R
    u16 ConstantElongation; // ���쳤   			   		             1		R
	  //-------------------------------------------------------------------------
    u16 Action;             //  Aciton                           2		R/W
		
		// ------------------------------------------------------------------------
		u16 FreS;             //  �����Ƶ��			                    3		R/W     
		u16 ConstantForce;    //  ��ǿ��                              4		R/W
    u16 FreH9;       //  ����Ƶ��       0-85                      5		R/W
		u16 Steps;       //  �ٶ�̨��    Ŀǰ�̶� 50    Null          6		R/W
		u16 StepLong;    //  ϸ��			       400 - 50000              7		R/W
		
		u16 Distance;       //  ���ȣ��г־���    Null                8		R/W
		u16 CycleNum;       //  ������ֹ����/ 1000   0.01-9999.9    9		R/W      ����
		
		u16 ForceDrop;      //  ����                                  A(10)		R/W
		
		// ���Բ���
		u16 GuanCount;       //  ���Թ���				                   B(11)	  R/W
		u16 GuanTimes;       //  ���Դ���					                 C(12)		R/W
	  //-------------------------------------------------------------------------	
		u16 Standard;        //  У׼ֵ������ֵ           				 D(13)		R/W
		u16 Unit;            //  ��λ   N��cN 				             E    R/W   
		
		u16 tDisplay;		     //  ǿ����ʾ				                   F		R
		u16 FullScaleAD;     //  ����У׼ֵ						            10		R
		u16 ZeroScaleAD;     //  ���У׼ֵ                       11    R
		
		u16 End_ID;          //  ϵͳ                            12		R/W
		u16 SysCode;         //  ϵͳ����			                   13   19
		
	}RegS;
};

// 
union INPUTREG_U
{
	unsigned int RegI[13];  // 
	struct INPUTREG_S
	{
		unsigned int STR_Result;          //  0  ���������� ʵʱADֵ
		unsigned int Force;                //  1  ʵʱǿ��   force
		
		unsigned int tZeroScaleAD;         //  2
		unsigned int tFullScaleAD;         //  3
		
		unsigned int BreakingForce;       //  4  ����ǿ��   breaking force ��������� ����ǿ����Ӧ��adֵ
		unsigned int Length;              //  5  �쳤		                                  //              
		unsigned int BreakingTenacity;    //  6  ����ǿ��   breaking tenacity  ɴ�߶���ǿ���������ܶȱ�ֵ��ͨ������ţ��ÿ�ؿ˱�ʾ
		unsigned int Elongation;          //  7  �����쳤�� breaking elongation 
		unsigned int BreakTime;           //  8  ����ʱ��   
		
		unsigned int StartForce;          //  9  ��ʼģ��
		
		unsigned int ErrorCode;           //  A  error code
		unsigned int Null3;               //  B
		unsigned int PF;                  //  C

	}RegS;
};

MAIN_EXT union HOLDREG_U Holding; 
MAIN_EXT union INPUTREG_U Input;     //

#ifdef BLINK_GLOBALS
#define BLINK_EXT
#else
#define BLINK_EXT extern
#endif

#define GPIODIRA GPIOA
#define DIRPORT  GPIO_Pin_7

//--------------------------------------------------------------------------------
// for motion
BLINK_EXT u16 Action_Flag;           // ������־
BLINK_EXT u16 RecordAction;          // ��¼��һ������

BLINK_EXT __IO uint32_t allmove;     // ���������pulse
BLINK_EXT __IO uint32_t backmove;    // ����ʱ�ص�pulse
BLINK_EXT __IO uint8_t  stepmove;    // ��ǰƵ��pulse,̨��W


BLINK_EXT uint8_t TexTestTimes;      // ����״ֵ̬

BLINK_EXT uint8_t  TestTimesRecord;  // �����������¼
BLINK_EXT uint8_t  GuanTimes_Cache;  // ÿ�ܴ�������
BLINK_EXT uint8_t  Guan_Cache;       // ÿ��������Թ�������

enum SpeedType {
    UP,        // 
    DOWN       // 
};

BLINK_EXT uint8_t ExamS_Flag;   // ����ʱ�Ӽ���ת�۵��־
BLINK_EXT uint8_t Current_Fre;      
BLINK_EXT uint8_t Target_Fre;   // Ŀ��Ƶ��
BLINK_EXT uint32_t Mark_Pos;    // �Ӽ���ת�۵�

//--------------------------------------------------------------------------------
// FUNCTION
BLINK_EXT void ControlMove(FunctionalState C);      //

#ifdef COLLECT_GLOBALS
#define COLLECT_EXT
#else
#define COLLECT_EXT extern
#endif

// for moter move    

// for AD
COLLECT_EXT uint8_t  adcBuf[2];   //  
COLLECT_EXT uint16_t adcVal;      //  
COLLECT_EXT uint16_t adcMax;      //  

COLLECT_EXT uint32_t TexMove;     // �����쳤
COLLECT_EXT uint32_t ZeroMove;    // �гֳ��Ȼ���Ϊ������в���

COLLECT_EXT uint8_t  Flag_Save;   // ��Ҫ�洢��־
COLLECT_EXT uint16_t strDisSta;   // ��������ֵ У׼����ֵfull��zero��ֵ

COLLECT_EXT uint32_t Mtemp;             // �м���
COLLECT_EXT	uint16_t DropTemp;          // ��������

COLLECT_EXT uint32_t distemp;           // ��ʾ����
COLLECT_EXT uint8_t timetemp;           // ����ʱ�����

COLLECT_EXT uint16_t startforcetemp;      // ��ʼģ�����Ȼ���
COLLECT_EXT uint16_t startforceADtemp;    // ��ʼģ��AD����

COLLECT_EXT uint16_t ResultTEX[200][3];   // ���Խ���洢

#ifdef PRINT_GLOBALS
#define PRINT_EXT
#else 
#define PRINT_EXT extern
#endif 

PRINT_EXT void PrintBody(void);
PRINT_EXT void PrintHead(void);
PRINT_EXT void hangjianju(void);
PRINT_EXT void shuipingzhaobiao(void);
PRINT_EXT void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
PRINT_EXT void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
PRINT_EXT void Uart2_SendChar(uint8_t Ch);

#ifdef CYCSEND_GLOBALS
#define CYCSEND_EXT
#else 
#define CYCSEND_EXT extern
#endif 

CYCSEND_EXT u16 ComCounter_Timer;

#ifdef TABLE_GLOBALS  
#define TABLE_EXT
#else
#define TABLE_EXT extern
#endif

TABLE_EXT const uint16_t FreTab[86];

#endif




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


