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

//C库
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
#define difMax      100       // 容差
#define PWMCORECLK  36000000


// 设备动作
enum ActionType {
    WAITING,        // 系统等待
    MOVEING_1,      // 电机动作,零点校正    1
	  MOVEING_2,      // 电机动作,零点校正    2
	
    ZEROSCALE,      // AD零点校正           3
	  FULLSCALE,      // AD满量程             4
	
		EXAM_START,     // 测试 examination     5
	  EXAM_1,         // E1               
	  EXAM_2,         // E2   
	  EXAM_3,         // E3  
	  STOP,           // STOP                 9
	  MOTORDOWN,      // DOWN                 10
	
	  PRINT           // print                11
};

// 单位选择
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
		u16 TEX;                // 线密度 density                    0		R
    u16 ConstantElongation; // 定伸长   			   		             1		R
	  //-------------------------------------------------------------------------
    u16 Action;             //  Aciton                           2		R/W
		
		// ------------------------------------------------------------------------
		u16 FreS;             //  回零点频率			                    3		R/W     
		u16 ConstantForce;    //  定强力                              4		R/W
    u16 FreH9;       //  测试频率       0-85                      5		R/W
		u16 Steps;       //  速度台阶    目前固定 50    Null          6		R/W
		u16 StepLong;    //  细分			       400 - 50000              7		R/W
		
		u16 Distance;       //  长度，夹持距离    Null                8		R/W
		u16 CycleNum;       //  从零点回止距离/ 1000   0.01-9999.9    9		R/W      长度
		
		u16 ForceDrop;      //  力降                                  A(10)		R/W
		
		// 测试参数
		u16 GuanCount;       //  测试管数				                   B(11)	  R/W
		u16 GuanTimes;       //  测试次数					                 C(12)		R/W
	  //-------------------------------------------------------------------------	
		u16 Standard;        //  校准值，砝码值           				 D(13)		R/W
		u16 Unit;            //  单位   N，cN 				             E    R/W   
		
		u16 tDisplay;		     //  强力显示				                   F		R
		u16 FullScaleAD;     //  满度校准值						            10		R
		u16 ZeroScaleAD;     //  零度校准值                       11    R
		
		u16 End_ID;          //  系统                            12		R/W
		u16 SysCode;         //  系统代码			                   13   19
		
	}RegS;
};

// 
union INPUTREG_U
{
	unsigned int RegI[13];  // 
	struct INPUTREG_S
	{
		unsigned int STR_Result;          //  0  拉力计算结果 实时AD值
		unsigned int Force;                //  1  实时强力   force
		
		unsigned int tZeroScaleAD;         //  2
		unsigned int tFullScaleAD;         //  3
		
		unsigned int BreakingForce;       //  4  断裂强力   breaking force 拉伸最大结果 断裂强力对应的ad值
		unsigned int Length;              //  5  伸长		                                  //              
		unsigned int BreakingTenacity;    //  6  断裂强度   breaking tenacity  纱线断裂强力与其线密度比值，通常以厘牛顿每特克表示
		unsigned int Elongation;          //  7  断裂伸长率 breaking elongation 
		unsigned int BreakTime;           //  8  断裂时间   
		
		unsigned int StartForce;          //  9  初始模量
		
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
BLINK_EXT u16 Action_Flag;           // 动作标志
BLINK_EXT u16 RecordAction;          // 记录上一个动作

BLINK_EXT __IO uint32_t allmove;     // 电机运行总pulse
BLINK_EXT __IO uint32_t backmove;    // 测试时回的pulse
BLINK_EXT __IO uint8_t  stepmove;    // 当前频率pulse,台阶W


BLINK_EXT uint8_t TexTestTimes;      // 测试状态值

BLINK_EXT uint8_t  TestTimesRecord;  // 总试验次数记录
BLINK_EXT uint8_t  GuanTimes_Cache;  // 每管次数缓存
BLINK_EXT uint8_t  Guan_Cache;       // 每次试验测试管数缓存

enum SpeedType {
    UP,        // 
    DOWN       // 
};

BLINK_EXT uint8_t ExamS_Flag;   // 测试时加减速转折点标志
BLINK_EXT uint8_t Current_Fre;      
BLINK_EXT uint8_t Target_Fre;   // 目标频率
BLINK_EXT uint32_t Mark_Pos;    // 加减速转折点

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

COLLECT_EXT uint32_t TexMove;     // 测试伸长
COLLECT_EXT uint32_t ZeroMove;    // 夹持长度换算为电机运行步数

COLLECT_EXT uint8_t  Flag_Save;   // 需要存储标志
COLLECT_EXT uint16_t strDisSta;   // 计量计算值 校准满度值full与zero差值

COLLECT_EXT uint32_t Mtemp;             // 中间结果
COLLECT_EXT	uint16_t DropTemp;          // 力降缓存

COLLECT_EXT uint32_t distemp;           // 显示缓存
COLLECT_EXT uint8_t timetemp;           // 试验时间计数

COLLECT_EXT uint16_t startforcetemp;      // 初始模量长度缓存
COLLECT_EXT uint16_t startforceADtemp;    // 初始模量AD缓存

COLLECT_EXT uint16_t ResultTEX[200][3];   // 测试结果存储

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


