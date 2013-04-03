#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "string.h"
#include "stdio.h"
#include "driverlib/flash.h"

volatile char response[100];
volatile char photo[13000];
volatile int a;
char cChar;
char temp;
int EndFlag = 0;
int StartFlag = 0;
int READ = 0;
int i = 0;
int j = 0;
int k = 0;
volatile int count = 0;


//*****************************************************************************
//
// Send command to the UART.
//
//*****************************************************************************
void
UARTSend(unsigned long ulBase, const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(ulBase, *pucBuffer++);
	}
}



//*****************************************************************************
//
// Receive response from the UART.
//
//*****************************************************************************
void
UARTReceive(unsigned long ulBase, volatile char pucBuffer[])
{
	//
	// Loop while there are more characters to send.
	//
	if(READ==1){ // Photo raw data
		while(UARTCharsAvail(ulBase) && !EndFlag)
		{
			// Read the next character from the UART
			cChar = UARTCharGet(ulBase);

			if(i >=5)
			{
				pucBuffer[count++] = cChar;

				// new code
				if((temp == 255) && (cChar == 217)){
					EndFlag = 1;
				}

				temp = cChar;
			}
			i++;
			//Print device response to terminal
			//UARTCharPut(UART0_BASE, cChar);
		}
	}

	else
		while(UARTCharsAvail(ulBase))
		{
			// Read the next character from the UART
			cChar = UARTCharGet(ulBase);
			pucBuffer[a++] = cChar;

			//Print device response to terminal
			UARTCharPut(UART0_BASE, cChar);
		}
}







//*****************************************************************************
//
// The UART interrupt handler.
//
//****************************************************************************
void
UART1IntHandler(void)
{
	unsigned long ulStatus;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART1_BASE, ulStatus);

	// Mientras haya data en el UART1
	if(READ == 1)
		UARTReceive(UART1_BASE, photo);
	else
		UARTReceive(UART1_BASE, response);

}


//////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	unsigned char bytes[16];
	unsigned char a = 0;
	unsigned int MH;
	unsigned int ML;

	//char KH;
	//char KL;
	//volatile char pulData[8];
	//unsigned long pulRead[2];
	//Word 1. Word is of 32 bits.
	//	pulData[0] = 0x12;
	//	pulData[1] = 0x34;
	//	pulData[2] = 0x56;
	//	pulData[3] = 0x78;
	//	//Word 2
	//	pulData[4] = 0x11;
	//	pulData[5] = 0x22;
	//	pulData[6] = 0x33;
	//	pulData[7] = 0x44;
	// Setear clock
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Configuracion de los puertos UART que se van a usar en el micro
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable processor interrupts
	IntMasterEnable();

	// Configure the GPIO pin muxing for the UART function.
	GPIOPinConfigure(0x00000001);//GPIO_PA0_U0RX
	GPIOPinConfigure(0x00000401);//GPIO_PA1_U0TX
	GPIOPinConfigure(0x00010001);//GPIO_PB0_U0RX
	GPIOPinConfigure(0x00010401);//GPIO_PB1_U0TX

	// Since GPIO A0 and A1 are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	// Enable interrupt
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');


	/////////////////////////////////////////////////////////////////////////////
	//Reset Command
	bytes[0] = 86; 	// 0x56
	bytes[1] = 00; 	// 0x00
	bytes[2] = 38;	// 0x26
	bytes[3] = 00;	// 0x00

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));

	// DELAY??

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));

	/////////////////////////////////////////////////////////////////////////////
	// Change Baud Rate Command (for a 115200 baud rate)
	//	bytes[0] = 86; 	// 0x56
	//	bytes[1] = 00; 	// 0x00
	//	bytes[2] = 36;	// 0x24
	//	bytes[3] = 03;	// 0x03
	//	bytes[4] = 01;	// 0x01
	//	bytes[5] = 13;	// 0x0D = 13 (para 115200)    or 1C = 28
	//	bytes[6] = 166;	// 0xA6 = 166 or 4C = 76
	//
	//
	//	// Delay 2-3 secs before taking picture
	//	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns
	//
	//	// Send to UART0 using UARTSend
	//	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));
	//
	//	// DELAY??
	//
	//	// Send to UART1 using UARTSend
	//	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));

	/////////////////////////////////////////////////////////////////////////////
	// Take Picture Command
	bytes[0] = 86;	// 0x56
	bytes[1] = 00;	// 0x00
	bytes[2] = 54;	// 0x36
	bytes[3] = 01;	// 0x01
	bytes[4] = 00;	// 0x00

	// Delay 2-3 secs before taking picture
	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));

	// DELAY??

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Read JPEG file size command
	bytes[0] = 86;	// 0x56
	bytes[1] = 00;	// 0x00
	bytes[2] = 52;	// 0x34
	bytes[3] = 01;	// 0x01
	bytes[4] = 00;	// 0x00

	// Delay 2-3 secs before taking picture
	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));

	// DELAY??

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Read JPEG file content
	//int length = strlen((const char*)response);
	//KH = response[length-2];
	//KL = response[length-1];

	//memset((void*)response, 0, 100);

	// Delay 2-3 secs before reading data
	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns

	MH = a/256;
	ML = a%256;
	bytes[0] = 86;	// 0x56
	bytes[1] = 00;	// 0x00
	bytes[2] = 50;	// 0x32
	bytes[3] = 12;	// 0x0C
	bytes[4] = 00;	// 0x00
	bytes[5] = 10; 	// 0x0A
	bytes[6] = 00;	// 0x00
	bytes[7] = 00;	// 0x00
	bytes[8] = MH;	// 0xMM
	bytes[9] = ML;	// 0xMM
	bytes[10] = 00;	// 0x00
	bytes[11] = 00;	// 0x00
	bytes[12] = 50;	// 0xKK - 0x00                 // KK es el tamano de la foto
	bytes[13] = 200;// 0xKK - 0x20                 //50 200 dec es 13KB
	bytes[14] = 00;	// 0xXX - 0x00
	bytes[15] = 10;	// 0xXX - 0x0A
	a += 32;

	// Delay 2-3 secs before reading data
	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns

	READ = 1;

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));

	// DELAY??

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));
	/////////////////////////////////////////////////////////////////////////////

	//	FlashErase(0x10000);
	//	unsigned long length = sizeof(photo);
	//	FlashProgram((unsigned long*)photo, 0x10000, length);
	//	SysCtlDelay(20000000);

	while(1){}

	return 0;
}




