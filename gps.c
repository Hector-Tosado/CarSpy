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

volatile char response[1000];
volatile char command[1000];
int a;
int count=0;
char cChar;
int foundflag=0;
int flagP=0;
volatile char coordinates[100];
int coordinatesCount=0;
int coordinatesSize=0;
char proof1[4];
char proof2[4];
int y =0;
int resultCompare=0;
int z=0;
char cCharS;
//*****************************************************************************
//
// Send a string to the UART.
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
		//UARTCharPut(UART0_BASE, *pucBuffer);
		UARTCharPut(ulBase, *pucBuffer++);

	}
}



//*****************************************************************************
//
// Receive a string from the UART.
//
//*****************************************************************************
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
		//pucBuffer[a++] = cChar;
		//Start of an output command
		if(cChar=='$')
		{
			cCharS=cChar;

			//loop to get output type
			for (y=0; y<=2; y++)
			{

				cChar = UARTCharGet(ulBase);

				//verifies if its coordinate output
				if(cChar=='G')
				{
					flagP=1;
				}

				if (flagP==1)
				{
					if (y==0)
					{
						coordinates[coordinatesCount]=cCharS;
						proof2[coordinatesCount]=cCharS;
						coordinatesCount++;

						UARTCharPut(UART0_BASE, cCharS);
						coordinates[coordinatesCount]=cChar;
						proof2[coordinatesCount]=cChar;
						coordinatesCount++;
						UARTCharPut(UART0_BASE, cChar);
					}
					else
					{
						coordinates[coordinatesCount]=cChar;
						proof2[coordinatesCount]=cChar;
						coordinatesCount++;
						UARTCharPut(UART0_BASE, cChar);
					}
				}
				resultCompare = strncmp(proof1, proof2, 4);
			}

			flagP=0;

			if (resultCompare==0)
			{
				foundflag = 1;
				resultCompare=1;
			}
			else
			{
				//z=50000;
				//while(z--)
				//{}
				coordinatesCount=0;
			}
		}
		else if (foundflag==1)
		{ 	//cChar = UARTCharGet(ulBase);
			if(coordinatesCount==7)
			{
				coordinatesCount=7;
			}
			coordinates[coordinatesCount++]=cChar;
			UARTCharPut(UART0_BASE, cChar);
		}

		if (foundflag==1 && cChar =='*')
		{
			foundflag=0;
			coordinatesSize=coordinatesCount;
			if (coordinatesSize <50)
			{
				UARTCharPut(UART0_BASE, 26);
				UARTCharPut(UART0_BASE, 'N');
				UARTCharPut(UART0_BASE, 'O');
				UARTCharPut(UART0_BASE, 26);
			}
			else
			{
				UARTCharPut(UART0_BASE, 26);
				UARTCharPut(UART0_BASE, 'Y');
				UARTCharPut(UART0_BASE, 'E');
				UARTCharPut(UART0_BASE, 'S');
				UARTCharPut(UART0_BASE, 26);

			}
			coordinatesCount=0;

		//If coordinate is valid then the interrupt will be disable.
			IntDisable(INT_UART1);
		}
		//If not valid then try again after a delay
		//DELAY HERE!!!
	}
}

//interrupt for testing purposes this will receive command from terminal
UARTReceive2(unsigned long ulBase, volatile char pucBuffer[])
{
	//
	// Loop while there are more characters to send.
	//
	while(UARTCharsAvail(ulBase))
	{
		// Read the next character from the UART
		cChar = UARTCharGet(ulBase);
		pucBuffer[a++] = cChar;
		UARTCharPut(UART1_BASE, cChar);
		UARTCharPut(UART1_BASE, 13);
		UARTCharPut(UART1_BASE, 26);
	}
}

UARTReceive3(unsigned long ulBase, volatile char pucBuffer[])
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

//*****************************************************************************
//
// The UART0 interrupt handler.
//
//****************************************************************************
/*void
UART0IntHandler(void)
{
	unsigned long ulStatus;
	//tBoolean bRc;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART0_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART0_BASE, ulStatus);

	// Mientras haya data en el UART0
	UARTReceive2(UART0_BASE, command);
	// UARTReceive(UART1_BASE, response);
	//Print to terminal, device response.

}

 */
//////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//
// The UART1 interrupt handler.
//
//****************************************************************************

void
UART1IntHandler(void)
{
	unsigned long ulStatus;
	//tBoolean bRc;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART1_BASE, ulStatus);

	// Mientras haya data en el UART1
	UARTReceive(UART1_BASE, response);

}



//////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	volatile long int i=1000000;
	proof1[0]='$';
	proof1[1]='G';
	proof1[2]='P';
	proof1[3]='R';

	volatile long int seg=1000000;//
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
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));


	//IntDisable(INT_UART1);

	//---------*********COMMANDS START HERE***********-------------

	i=seg/2;
	while(i--)
	{}
	//--------------Set RMC only output sentences
	UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n",53);

	IntEnable(INT_UART1);
			UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
	//UARTReceive(UART1_BASE, response);


	//--------------Enter Low Power Mode
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK161,0*28\r\n",17);

	//--------------Test and activate low power mode
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK000*32\r\n",15);

	i=seg/2;
			while(i--)
			{}


	//--------------Get Threshold Speed
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK447*35\r\n",15);

	//--------------Set Threshold Speed
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK397,2.0*3F\r\n",19);

	//--------------Get Baudrate
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK251,38400*27\r\n",21);

	//--------------Set Baudrate
	/*UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK251,115200*27\r\n",22);
	i=50000;
		while(i--)
		{}
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));
	 */
	//i=50000;
	//while(i--)
	//{}
	//UARTSend((unsigned long)UART1_BASE, (unsigned char *)"$PMTK000*32\r\n",15);



	//UARTCharPut(UART1_BASE, 13);
	//UARTCharPut(UART1_BASE, 26);
	//	int x=0;
	//while (x<5)
	//{x++;
	//}

	while(1){
		//UARTReceive(UART1_BASE, response);
		//	UARTReceive(UART1_BASE, response);
		//UARTReceive(UART1_BASE, response);
		//IntDisable(INT_UART1);
		//$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29
		//UARTSend((unsigned long)UART1_BASE, (unsigned char *) "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29",49);
		//UARTCharPut(UART1_BASE, 13);
		//UARTCharPut(UART1_BASE, 26);
		//UARTSend((unsigned long)UART0_BASE, (unsigned char *) "$PMTK414*33",11);

		// UARTReceive2(UART2_BASE, response);



	}

	return 0;
}
