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

#define TEST

#define ZEROFRE			  0x0003
#define ACTFRE	      0x0000 
#define ADD_TARGET		0x0026       // Ŀ��������

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
	  MOTORDOWN       // DOWN                 10
};

// ��λѡ��
enum UnitType {
	Newton = 1,          // N  1
	cNewton,             // cN
	Kilogram,            // kg
	Pound                // lb
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
		u16 TEX;              // ���ܶ� density                      0		R
    u16 ConstantElongation; // ���쳤   			   		             1		R
	  //-------------------------------------------------------------------------
    u16 Action;           //  Aciton                             2		R/W
		
		// ------------------------------------------------------------------------
		u16 FreS;        //  �����Ƶ��			  0-85  default  600      3		R/W     
		u16 ConstantForce;    //  ��ǿ��                              4		R/W
    u16 FreH9;       //  ����Ƶ��       0-85                      5		R/W
		u16 Steps;       //  �ٶ�̨��    Ŀǰ�̶� 50    Null          6		R/W
		u16 StepLong;    //  ϸ��			       400 - 50000              7		R/W
		
		u16 Distance;       //  ���ȣ��г־���    Null                8		R/W
		
		u16 CycleNum;       //  ������ֹ����/ 1000   0.01-9999.9    9		R/W      ����
		u16 ForceDrop;      //  ����                                  A		R/W
		
		// ���Բ���
		u16 GuanCount;       //  ���Թ���				                   B	  R/W
		u16 GuanTimes;       //  ���Դ���					                 C		R/W
	  //-------------------------------------------------------------------------	
		u16 Standard;        //  У׼ֵ������ֵ           				 D		R/W
		u16 Unit;            //  ��λ   N��cN 				             E    R/W   
		
		u16 tDisplay;		      //  ǿ����ʾ				                   F		R
		u16 FullScaleAD;     //  ����У׼ֵ						            10		R
		u16 ZeroScaleAD;     //  ���У׼ֵ                       11   R
		
		u16 End_ID;           //  ϵͳ                            12		R/W
		u16 tErrorCode;       //  �������			                  13
		
		// for motion 
		// u16 ADres[10];		      //             		       20 - 29   R
		
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
		
		unsigned int ErrorCode;           //  9  error code
		
		unsigned int Power;              //  A
		unsigned int Null3;              //  B
		unsigned int PF;                 //  C

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
BLINK_EXT __IO uint8_t stepmove;     // ��ǰƵ��pulse,̨��W

BLINK_EXT uint32_t ZeroMove;         // �гֳ��Ȼ���Ϊ������в���
BLINK_EXT uint8_t GuanTimes_Cache;   

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
COLLECT_EXT uint8_t adcBuf[2];    //  
COLLECT_EXT uint16_t adcVal;      //  
COLLECT_EXT uint16_t adcMax;      //  
COLLECT_EXT uint32_t TexMove;     //  �����쳤

COLLECT_EXT uint8_t Flag_Save;    // ��Ҫ�洢��־
COLLECT_EXT uint16_t strDisSta;   // ��������ֵ У׼����ֵfull��zero��ֵ

COLLECT_EXT  uint16_t calcStand(uint16_t selAD);            // �����׼ֵ

#ifdef TEST

COLLECT_EXT uint8_t countad;      // 
#endif 

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


