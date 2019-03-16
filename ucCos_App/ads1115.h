
/**
  ******************************************************************************
  * @file    xxx.h
  * @author  
  * @version 
  * @date    xx-xx-2018
  * @brief   xxxxx.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _ADS1115_H_
#define _ADS1115_H_


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "MacroAndConst.h"
#include <stm32f10x.h>


/* Select which EEPROM will be used with this driver */
#define ADS1115

/* Uncomment the following line to use the default ADS1115_TIMEOUT_UserCallback() 
   function implemented in 
   ADS1115_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */   
/* #define USE_DEFAULT_TIMEOUT_CALLBACK */

  
#define ADS1115_HW_ADDRESS         0x90        /* E0 = E1 = E2 = 0 */ 

#define ADS1115_I2C_TIMING         0x00210507

   
/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define ADS1115_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define ADS1115_LONG_TIMEOUT         ((uint32_t)(10 * ADS1115_FLAG_TIMEOUT))

/* Maximum number of trials for ADS1115_WaitEepromStandbyState() function */
#define ADS1115_MAX_TRIALS_NUMBER     300
      
#define ADS1115_OK                    0
#define ADS1115_FAIL                  1   

#define ADS1115_I2C                   I2C1 


void     ADS1115_DeInit(void);
void     ADS1115_Init(void);

uint32_t     ADS1115_WriteConfig(void);
uint32_t     ADS1115_ReadConversion(uint8_t* pBuffer); 


/* USER Callbacks: These are functions for which prototypes only are declared in
   EEPROM driver and that should be implemented into user applicaiton. */  
/* ADS1115_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm320518_evel_i2c_ee.h file.
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t ADS1115_TIMEOUT_UserCallback(void);

#ifdef __cplusplus
}
#endif

#endif 

/*-------------------------------END OF FILE-------------------------------*/




