#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "gsm.h"

#define Camera		UART1_BASE
#define GSM 		UART3_BASE
#define GPS			UART4_BASE

#define DeviceIdentifier		1
#define testSize		13500
volatile char response[10000];

int a = 0;

volatile long int j=1000000;
volatile long int sec=1000000;

int sFlag = 0; //Send Flag
int rFlag = 0; //Receive Flag
int ce = 0;

char cChar;
void
UARTReceive(unsigned long ulBase, volatile char pucBuffer[])
{
	//
	// Loop while there are more characters to send.
	//
	while(UARTCharsAvail(ulBase))
	{
		// Read the next character from the UART
		cChar = UARTCharGet(ulBase);
		pucBuffer[a++] = cChar;
		UARTCharPut(UART0_BASE, cChar);
	}
}

void
UART3IntHandler(void)
{
	unsigned long ulStatus;
	//tBoolean bRc;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(GSM, true);

	// Clear the asserted interrupts.
	UARTIntClear(GSM, ulStatus);

	// Mientras haya data en el UART1
	UARTReceive(GSM, response);

}
void
UARTSetup()
{
	//UART Port configuration

	//SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);//Terminal UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	//	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);


	//Pin_map HELP
	// Enable ports for the UART Rx or Tx (eg, Enable port PA0 for UART0 U0RX)
	GPIOPinConfigure(0x00000001);//GPIO_PA0_U0RX
	GPIOPinConfigure(0x00000401);//GPIO_PA1_U0TX
	//	GPIOPinConfigure(0x00010001);//GPIO_PB0_U0RX
	//	GPIOPinConfigure(0x00010401);//GPIO_PB1_U0TX
	GPIOPinConfigure(0x00021801);//GPIO_PC6_U3RX
	GPIOPinConfigure(0x00021C01);//GPIO_PC7_U3TX
	//	GPIOPinConfigure(0x00021001);//GPIO_PC4_U4RX
	//	GPIOPinConfigure(0x00021401);//GPIO_PC5_U4TX


	// Configure the GPIOs (ex. A0, A1) that are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	//	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	//	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_4);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	//	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
	//			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	//					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(GSM, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
}

void
UARTSend(unsigned long ulBase, const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	//	int i = 0;
	while(ulCount--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(ulBase, *pucBuffer++);
		//		i++;
		//
		//		if(i == 512){
		//			SysCtlDelay(25000000);
		//			i=0;
		//		}

	}
}

volatile unsigned char sendTest[testSize];
int main(void)
{

	//Set Clock for MCU to user. 50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//Setup all UART peripherals to be used.
	UARTSetup();
	// Enable processor interrupts
	IntMasterEnable();
	IntEnable(INT_UART3);
	UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');

	//Verify GSM is connected to carrier.
	//	SysCtlDelay(25000000); Removed delay.


	int s;
	int c = 49;
	for(s = 0; s < testSize; s++){
		sendTest[s] = c;

		if (s%240 == 0)
			c++;



	}
	//		"AT#SCFG=1,1,1500,90,600,50\r\n"

	//	j=sec;
	//	while(j--){}
	//	UARTSend((unsigned long)GSM, (unsigned char *) "AT#SCFG=1,1,1024,90,600,50\r\n",30);// 30);

	while(!ce)
	{
		/*
		 * Expected 0,1 if registered in network. Re-try until this is 0,1.
		 */
		checkConnection();

		if((response[a-11] == '0') && (response[a-9] == '1'))
		{
			ce = 1;
			//Response back with 0. Registered on network.
			UARTCharPut(UART0_BASE, response[a-11]);
		}
	}

	SysCtlDelay(10000000);//~1 Second Delay Can be more depending on network verification intervals.

	//	if(connEstablished == 1)
	//	{
	//	Initial Configuration of GSM is done before shipping. Instructions are set
	contextActivation();


	char *endStream = "endstream";
	memcpy((void *)(sendTest+(testSize-100)), &endStream[0], strlen(endStream) );
	while(!sFlag && !rFlag)
	{

		while(!sFlag)
		{
		TCPConnect();

		//Delay to assure socket connection.
		SysCtlDelay(25000000);//3 second //Changed this delay from 6 to 3 seconds.
		if(response[a-5] == 'E' && response[a-4] == 'C' && response[a-3] == 'T')
			sFlag = 1;
		else if(response[a-5] == 'R' && response[a-4] == 'O' && response[a-3] == 'R' && !sFlag)
			a = 0;


			}


		//	while(!rFlag)
		//	{
		if(sFlag){

			a = 0;
			SendData((unsigned char*)sendTest);
			//			IntDisable(INT_UART3);
			//		SysCtlDelay(50000000);
			//			IntEnable(INT_UART3);


			do {
				SysCtlDelay(50000000);
			} while (response[0] != '*' && response[11] != 'R' );

			SysCtlDelay(10000000);
			if(response[0] == '*')
			{
				//Testing Acknowledge
				j=sec;
				while(j--){}
				UARTSend((unsigned long)UART0_BASE, (unsigned char *) "ACK Received", 12);
				UARTCharPut(UART0_BASE, 13);
				rFlag = 1;
				a=0;
				memset(&response, 0, sizeof(response));
				//		}

				if(response[11] == 'R')
				{
					sFlag = 0;
				}

			}
		}
		while(1){}

		return 0;
	}
}
