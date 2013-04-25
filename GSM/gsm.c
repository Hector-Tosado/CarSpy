/*
 * gsm.c
 *
 *  Created on: Mar 26, 2013
 *      Author: Hector
 */
#include "inc/hw_memmap.h"//
#include "inc/hw_types.h"//
#include "inc/hw_ints.h"//
#include "inc/hw_gpio.h"//

#include "driverlib/gpio.h"//
#include "driverlib/uart.h"//

#include "driverlib/interrupt.h"//

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#define Camera		UART1_BASE
#define GSM 		UART4_BASE
//#define GPS			UART4_BASE

#define packetSize		1024
extern void UARTSend(unsigned long ulBase, const unsigned char *pucBuffer, unsigned long ulCount);

void
makeCall()
{

	volatile long int i=1000000;
	volatile long int seg=1000000;
	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "ATD7876725920;", 14);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);

	SysCtlDelay(100000000); //12 seconds. 100M
	//	i=seg;
	//	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "ATH", 3);
	UARTCharPut(GSM, 13);
}

void
sendTextMessage(){

	volatile long int i=1000000;
	volatile long int seg=1000000;
	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "AT+CMGF=1;", 9);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);

	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "AT+CMGS=7874218220;", 18);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);
	SysCtlDelay(25000000); //3 seconds. 100M

	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "HH2 Test ATH$$1b", 16);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);

	SysCtlDelay(100000000); //12 seconds. 100M

}

void
checkConnection()
{
	volatile long int i=1000000;
	volatile long int seg=1000000;
	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "AT+CREG?", 8);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);

	SysCtlDelay(10000000);//~1 second delayW
}

void
SendData(const unsigned char *pucBuffer)
{
	volatile long int i=1000000;
	volatile long int seg=1000000;

	//Data size seria el tamaño de la toda la info guardada en el SD card?

	char subBuff[packetSize];

	int s;
	for(s = 0; (s*packetSize) <= 13500; s++)
	{
		if(((s*packetSize) % 4096) == 0){
			i=seg;
			while(i--){}
		}
//
		if(((s*packetSize) % 9216) == 0){
			i=seg;
			while(i--){}
		}
			memcpy(subBuff, &pucBuffer[packetSize*s], packetSize);

			i=seg*2.1;
			while(i--){}
			UARTSend((unsigned long)GSM, (const unsigned char*)subBuff, packetSize);
			//		UARTCharPut(GSM, 13);
			//		UARTCharPut(GSM, 26);
			//		memset(subBuff, 0, sizeof(subBuff) );

		}
		//		int l = strlen((const char*)pucBuffer);
		//Change to photo after program is integrated.
		//		i=seg;
		//		while(i--){}
		//		UARTSend((unsigned long)GSM, (const unsigned char*)pucBuffer, l);
		//		UARTCharPut(GSM, 13);
		//		UARTCharPut(GSM, 26);
		//	}


		//	subbuff[4] = '\0'; //And end of string.



		SysCtlDelay(25000000);//3 second delay
	}

	void
	TCPConnect()
	{
		volatile long int i=1000000;
		volatile long int seg=1000000;
		i=seg;
		while(i--){}
		UARTSend((unsigned long)GSM, (unsigned char *) "AT#SD=1,0,6102,carspy.ece.uprm.edu", 34);
		UARTCharPut(GSM, 13);

	}
	void
	contextActivation()
	{
		volatile long int i=1000000;
		volatile long int seg=1000000;
		i=seg;
		while(i--){}
		UARTSend((unsigned long)GSM, (unsigned char *) "AT#SGACT=1,1", 12);
		UARTCharPut(GSM, 13);
		UARTCharPut(GSM, 26);
		//Delay to acknowledge IP registration successfully.
		SysCtlDelay(25000000);//3 second delay
	}
