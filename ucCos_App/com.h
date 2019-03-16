/*
***********************************************************************************************
*                                           uC/OS-View
*
*                            (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : OS_VIEW.H
* Version    : V1.33
* Programmer : KEN
***********************************************************************************************
*/

#ifdef   COM_GLOBALS
#define  COM_EXT
#else
#define  COM_EXT  extern
#endif

#include  <stm32f10x.h>

#ifndef  COM_H
#define  COM_H

#define  RX_BUF_SIZE        128u          // ���ڽ����������

//#define  RX_STATE_SD0              0                /* waiting for start first  start delimiter (SD0)  */
//#define  RX_STATE_SD1              1                /* waiting for start second start delimiter (SD1)  */
#define  RX_STATE_ID               0                /* waiting for End ID (End ID)  */
#define  RX_STATE_LEN              1                /* waiting for len byte  ���ݹ�����ȷ������        */
#define  RX_STATE_DATA             4                /* waiting for data                                */
#define  RX_STATE_CHKSUM           5                /* waiting for checksum                            */
#define  RX_STATE_ED               6                /* waiting for end delimiter                       */
#define  RX_STATE_DATA1            7                /* waiting for data                                */

#define  PROTOCOL_RX_SD0        0xED                /* Start delimiters                                */
#define  PROTOCOL_RX_SD1        0x8C
#define  PROTOCOL_RX_ED         0x8D                /* End   delimiter                                 */
                               /* Outbound packets (to PC)                        */
#define  PROTOCOL_TX_SD0        0x8C                /* Start delimiters                                */
#define  PROTOCOL_TX_SD1        0xED
#define  PROTOCOL_TX_ED         0x8D                /* End   delimiter                                 */

COM_EXT  uint8_t              pkt_Flag;

COM_EXT  uint8_t              RxState;        // ���մ�������״̬
COM_EXT  uint8_t              RxRemainLen;
COM_EXT  uint8_t              RxChkSum;
COM_EXT  uint8_t              RxBuf[RX_BUF_SIZE];  // ���ջ����ַ���

COM_EXT  uint8_t              RxBufCnt;
COM_EXT  uint8_t              RxRdIx;
COM_EXT  uint32_t             RxCtr;
COM_EXT  uint16_t             RxPktCtr;

COM_EXT  uint8_t    			    TxLen; 
COM_EXT  uint8_t				      TxEnable_Flag;	// �����ͱ�־λ

COM_EXT  uint8_t              TxBuf[50];      // MAX 50 BYTE

COM_EXT  uint8_t              MulLen;
COM_EXT  uint16_t             RxTimeCnt;      // ���ռ�ʱ timer
COM_EXT  uint8_t               ComStart_Flag;    // �ȴ�һ֡������ʼ�ֽ�

COM_EXT struct crcstruct
{
	unsigned char crcH;
	unsigned char crcL;
}crcs;

void cpytex(uint8_t *dst, const uint8_t *src);    // 


#endif


