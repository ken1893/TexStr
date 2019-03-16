//****************************************Copyright (c)**************************************************

// оƬ�ͺ�          : STM32F030
// ����Ƶ��          : 8.000000 MHz
//
//--------------�ļ���Ϣ--------------------------------------------------------------------------------
//��   ��   ��: i2c.c
//����޸�����: 2018��12��21��
//��        ��: ��ʼ��I2C1,����д

//********************************************************************************************************

// Includes ------------------------------------------------------------------

#include "includes.h"
#include "ads1115.h"

#define USE_DEFAULT_TIMEOUT_CALLBACK

/** @defgroup STM320518_EVAL_I2C_EE_Private_Variables
  * @{
  */
__IO uint16_t  ADS1115Address = 0;   
__IO uint32_t  ADS1115Timeout = ADS1115_LONG_TIMEOUT;
__IO uint16_t  ADS1115DataNum;
__IO uint16_t* ADS1115DataReadPointer;   

__IO uint16_t  sEEAddress = 0;     

__IO uint8_t*  sEEDataWritePointer;  
__IO uint8_t   sEEDataNum;

uint16_t  adtemp;

void ADS1115_DeInit(void)
{
  //ADS1115_LowLevel_DeInit(); 
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void ADS1115_Init(void)
{ 
   I2C_InitTypeDef  I2C_InitStructure;
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//����ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

   /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	   /*!< I2C configuration */
  /* ADS1115_I2C configuration */
	I2C_DeInit(ADS1115_I2C);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  //I2C_InitStructure.I2C_OwnAddress1 = ADS1115_HW_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;
  
  /* ADS1115_I2C Peripheral Enable */
  I2C_Cmd(ADS1115_I2C, ENABLE);
  /* Apply ADS1115_I2C configuration after enabling it */
  I2C_Init(ADS1115_I2C, &I2C_InitStructure);
  
  ADS1115Address = ADS1115_HW_ADDRESS;
}

//---------------------------------------------------------------------------------------------
//
//    ADS1115_ReadConversion
//
//---------------------------------------------------------------------------------------------
uint32_t ADS1115_ReadConversion(uint8_t* pBuffer)
{
	//uint8_t DataNum;

	/*����д�����������豸��ַд�������ҵ��豸Ӧ���ˣ�д��Ҫ�����ݵĵ�ַ*/
   //IIC_Wait_EEprom(); 
   I2C_GenerateSTART(ADS1115_I2C, ENABLE);
	
	 //EV5�¼�
	ADS1115Timeout = ADS1115_LONG_TIMEOUT;
   while(!I2C_CheckEvent(ADS1115_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	 {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }
   I2C_Send7bitAddress(ADS1115_I2C, ADS1115Address, I2C_Direction_Transmitter); 
	
	 //EV6 EV8_1�¼������¼��ж�ͬʱ�ж���EV8_1�¼���
	ADS1115Timeout = ADS1115_LONG_TIMEOUT;
	 while(!I2C_CheckEvent(ADS1115_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	 {
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
	 //I2C_SendData(I2C1,Word_Address); 
	 I2C_SendData(ADS1115_I2C, 0x00);      // points to Conversion  0b00000000
	 ADS1115Timeout = ADS1115_LONG_TIMEOUT;
	 while(!I2C_CheckEvent(ADS1115_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //����EV8_2���ж�������Ӧ��ʱ�Ѿ��յ�ACK��Word_Address������ɡ�
	 {
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
	 I2C_GenerateSTOP(ADS1115_I2C,ENABLE);
	 
		 
	/*��Start�����������豸��ַ���������ҵ��豸Ӧ���ˣ������ݣ�������Ӧ����ֹ*/
	 I2C_GenerateSTART(ADS1115_I2C, ENABLE);
	//EV5�¼�
	 ADS1115Timeout = ADS1115_LONG_TIMEOUT;
   while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
		 {
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
   
	 I2C_Send7bitAddress(ADS1115_I2C, ADS1115Address, I2C_Direction_Receiver ); 
  //EV6�¼�
	 ADS1115Timeout = ADS1115_LONG_TIMEOUT;
	 while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
		 
	 //for(DataNum=0;DataNum<1;DataNum++)
	// {
		 I2C_AcknowledgeConfig(ADS1115_I2C,ENABLE);
		 ADS1115Timeout = ADS1115_LONG_TIMEOUT;
		 while(I2C_GetFlagStatus(ADS1115_I2C,I2C_FLAG_RXNE) == RESET)
			 {
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
		 *pBuffer = I2C_ReceiveData(ADS1115_I2C);
		 pBuffer++;
	 //}
	 
	 I2C_AcknowledgeConfig(ADS1115_I2C,DISABLE);
	 ADS1115Timeout = ADS1115_LONG_TIMEOUT;
	 while(I2C_GetFlagStatus(ADS1115_I2C,I2C_FLAG_RXNE) == RESET)
		 {
	   if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
	 }
	 *pBuffer = I2C_ReceiveData(ADS1115_I2C);
	//EV6_1�¼���û�б�־λ��Ҫ����ACKʧ�ܺ�ֹͣλ����
	 //I2C1->CR1 &= 0xFBFF ;   // ʧ��ACK
	 //I2C1->CR1 |= 0x0020 ;   // ʹ��Stop
	
	//EV7�¼�����DR�Ĵ���
	 //while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	 //* pBuffer = I2C1->DR;		
		
	 I2C_GenerateSTOP(ADS1115_I2C,ENABLE);
	 
  return ADS1115_OK;
}


//---------------------------------------------------------------------------------------------
//
//    ADS1115_WriteConfig
//
//---------------------------------------------------------------------------------------------

uint32_t ADS1115_WriteConfig(void)
{  
	ADS1115Address = ADS1115_HW_ADDRESS;
  /*!< While the bus is busy */
  ADS1115Timeout = ADS1115_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(ADS1115_I2C, I2C_FLAG_BUSY))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }
   
  /*!< Send START condition */
  I2C_GenerateSTART(ADS1115_I2C, ENABLE);
  /*!< Test on EV5 and clear it */
  ADS1115Timeout = ADS1115_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(ADS1115_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }

  /*!< Send ADS1115Address for write */
  I2C_Send7bitAddress(ADS1115_I2C, ADS1115Address, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  ADS1115Timeout = ADS1115_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(ADS1115_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }
	
	/// 2\  Send points to Config register /

  I2C_SendData(ADS1115_I2C,0x01);     // 0b00000001       
  ADS1115Timeout = ADS1115_LONG_TIMEOUT;
	while(!I2C_CheckEvent(ADS1115_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }

  ///  3\ Send MSB of the Config register to be written  /

  I2C_SendData(ADS1115_I2C,0x82);        // msb   0b10000010

  /*!< Test on EV8 and clear it */
  ADS1115Timeout = ADS1115_FLAG_TIMEOUT;  
  while(!I2C_CheckEvent(ADS1115_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }  
  
  ///   4\ Send LSB of the Config register to be written  /
  I2C_SendData(ADS1115_I2C,0x83);        // Lsb   0b10000011

  /*!< Test on EV8 and clear it */
  ADS1115Timeout = ADS1115_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(ADS1115_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((ADS1115Timeout--) == 0) return ADS1115_TIMEOUT_UserCallback();
  }  
  
 I2C_GenerateSTOP(ADS1115_I2C, ENABLE);
  
  /* If all operations OK, return ADS1115_OK (0) */
  return ADS1115_OK;
}

#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t ADS1115_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
		Input.RegS.ErrorCode = 1;
		return Input.RegS.ErrorCode;
  }
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */














