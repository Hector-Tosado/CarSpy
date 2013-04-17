// Camera
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"



// Variables for camera
volatile char response[100];
volatile char photo[13000];
volatile int a;
char cChar;
char temp;
int EndFlag = 0;
int StartFlag = 0;
int READ = 0;
int i, j, k = 0;
volatile int count = 0;
unsigned char command[16];



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

			// Discard first 5 characters
			if(i >=5)
			{
				pucBuffer[count++] = cChar;

				// If data reaches FF D9
				if((temp == 255) && (cChar == 217)){
					EndFlag = 1;
				}

				temp = cChar;
			}

			i++;
			//Print device (photo) response to terminal
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
//*****************************************************************************
void
UART1IntHandler(void)
{
	unsigned long ulStatus;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART1_BASE, ulStatus);

	// If there is data in the UART call Receive function
	// with corresponding buffer
	if(READ == 1)
		UARTReceive(UART1_BASE, photo);
	else
		UARTReceive(UART1_BASE, response);

}


void resetCamera(){
	//Reset Command
	command[0] = 86; 	// 0x56
	command[1] = 00; 	// 0x00
	command[2] = 38;	// 0x26
	command[3] = 00;	// 0x00

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, command, sizeof(command));

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, command, sizeof(command));
}


void takePhoto(){

	// Take Picture Command
	command[0] = 86;	// 0x56
	command[1] = 00;	// 0x00
	command[2] = 54;	// 0x36
	command[3] = 01;	// 0x01
	command[4] = 00;	// 0x00

	// Delay 2-3 secs before taking picture
	SysCtlDelay(15000000); // @ 50MHz each MCU cycle is 20ns

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, command, sizeof(command));

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, command, sizeof(command));
}


void readPhotoLength(){
	// Read JPEG file size command
	command[0] = 86;	// 0x56
	command[1] = 00;	// 0x00
	command[2] = 52;	// 0x34
	command[3] = 01;	// 0x01
	command[4] = 00;	// 0x00

	// Delay 2-3 secs before taking picture
	SysCtlDelay(150000000); // @ 50MHz each MCU cycle is 20ns

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, command, sizeof(command));

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, command, sizeof(command));
}


void readPhoto(){
		// Read JPEG file content

		// Delay 2-3 secs before reading data
		SysCtlDelay(15000000); // @ 50MHz each MCU cycle is 20ns

		command[0] = 86;	// 0x56
		command[1] = 00;	// 0x00
		command[2] = 50;	// 0x32
		command[3] = 12;	// 0x0C
		command[4] = 00;	// 0x00
		command[5] = 10; 	// 0x0A
		command[6] = 00;	// 0x00
		command[7] = 00;	// 0x00
		command[8] = 00;	// 0xMM
		command[9] = 00;	// 0xMM
		command[10] = 00;	// 0x00
		command[11] = 00;	// 0x00
		command[12] = 50;	// 0xKK - 0x00                 // KK es el tamano de la foto
		command[13] = 200;	// 0xKK - 0x20                 //50 200 dec es 13KB
		command[14] = 00;	// 0xXX - 0x00
		command[15] = 10;	// 0xXX - 0x0A
		a += 32;

		// Delay 2-3 secs before reading data
		SysCtlDelay(15000000); // @ 50MHz each MCU cycle is 20ns

		// READ Flag
		READ = 1;

		// Send to UART0 using UARTSend
		UARTSend((unsigned long)UART0_BASE, command, sizeof(command));

		// Send to UART1 using UARTSend
		UARTSend((unsigned long)UART1_BASE, command, sizeof(command));

}



int main(void)
{

	// Setear clock
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Configuring UART ports
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

	// configure GPIO A0 and A1 for use as a peripherals.
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 38,400, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	// Enable interrupt
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character on terminal to indicate start.
	UARTCharPut(UART0_BASE, '!');


	resetCamera();
	takePhoto();
	readPhotoLength();
	readPhoto();


	///////////////////////////// ENCRYPTION /////////////////////////////////

	while(1){}

	return 0;
}




