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

#define Camera		UART1_BASE
#define GSM 		UART3_BASE
#define GPS			UART4_BASE

#define DeviceIdentifier		1

volatile char response[1000];

int a, connEstablished = 0;

volatile long int i=1000000;
volatile long int seg=1000000;//3000000 funciona con 38400 baudrate.

//typedef struct initPacket{
//	unsigned int devId;
//	volatile char encryption_Key[16];
//}initPacket;
//
//typedef struct dataPacket{
//	unsigned int dataType;
//	unsigned int size;
//	const char *data;//[13000];
//}dataPacket;
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
		UARTCharPut(ulBase, *pucBuffer++);
	}
}



//*****************************************************************************
//
// Receive a string from the UART.
//
//*****************************************************************************
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

//*****************************************************************************
//
// The UART0 interrupt handler.
//
//****************************************************************************
//void
//UART0IntHandler(void)
//{
//	unsigned long ulStatus;
//	//tBoolean bRc;
//
//	// Get the interrupt status.
//	ulStatus = UARTIntStatus(UART0_BASE, true);
//
//	// Clear the asserted interrupts.
//	UARTIntClear(UART0_BASE, ulStatus);
//
//	// Mientras haya data en el UART1
//	UARTReceive(UART0_BASE, command);
//
//	//Print to terminal, device response.
//
//}


//////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//
// The UART1 interrupt handler.
//
//****************************************************************************
//void
//UART1IntHandler(void)
//{
//	unsigned long ulStatus;
//	//tBoolean bRc;
//
//	// Get the interrupt status.
//	ulStatus = UARTIntStatus(UART1_BASE, true);
//
//	// Clear the asserted interrupts.
//	UARTIntClear(UART1_BASE, ulStatus);
//
//	// Mientras haya data en el UART1
//	UARTReceive(UART1_BASE, response);
//
//}

//////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//
// The UART3 interrupt handler.
//
//****************************************************************************
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

////////////////////////////////////////////////////////////////////////////////////////////
////*****************************************************************************
////
//// The UART4 interrupt handler.
////
////****************************************************************************
//void
//UART4IntHandler(void)
//{
//	unsigned long ulStatus;
//	//tBoolean bRc;
//
//	// Get the interrupt status.
//	ulStatus = UARTIntStatus(UART4_BASE, true);
//
//	// Clear the asserted interrupts.
//	UARTIntClear(UART4_BASE, ulStatus);
//
//	// Mientras haya data en el UART1
//	UARTReceive(UART4_BASE, response);
//
//}
/*
 * Setup TCP Connection
 * Configuration, Activates Context, Opens server connection, File transfer type,
 */



//////////////////////////////////////////////////////////////////////////////////////////


void
makeCall()
{
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
checkConnection()
{
	/*
	 * Returns OK if carrier if GSM is correctly connected.
	 * Returns NO CARRIER otherwise.
	 */

	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "AT", 2);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);

	i=seg;
	while(i--){}

	while(!connEstablished)
	{
		/*
		 * Expected 0,1 if registered in network. Re-try until this is 0,1.
		 */
		UARTSend((unsigned long)GSM, (unsigned char *) "AT+CREG?", 8);
		UARTCharPut(GSM, 13);
		UARTCharPut(GSM, 26);

		SysCtlDelay(10000000);//~1 second delay

		if((response[a-11] == '0') && (response[a-9] == '1'))
		{
			connEstablished = 1;
			//Responsd back with 0. Registered on network.
			UARTCharPut(UART0_BASE, response[a-11]);
		}
	}
}

int sFlag = 0; //Send Flag
int rFlag = 0; //Receive Flag
int contActivated;

void
SendData(const unsigned char *pucBuffer)
{
	while(!rFlag)
	{
		a = 0;
		i=seg;
		while(i--){}
		int l = strlen((const char*)pucBuffer);
		//Change to photo after program is integrated.
		UARTSend((unsigned long)GSM, pucBuffer, l);
		UARTCharPut(GSM, 13);
		UARTCharPut(GSM, 26);

		SysCtlDelay(25000000);//3 second delay
		if(response[0] == '*')
		{

			//Testing Acknowledge
			i=seg;
			while(i--){}
			UARTSend((unsigned long)UART0_BASE, (unsigned char *) "ACK Received", 12);
			UARTCharPut(UART0_BASE, 13);
			rFlag = 1;
			a=0;
			memset(&response, 0, sizeof(response));
		}

		//	}while(response[0] == '*');
	}
}

//Context Activate flag, debugs error on 1st time context activation and
//connection command AT#SD getting stuck if server is also not listening or ready.
void
TCPConnect()
{
	connEstablished = 0;

	/*INITIAL Configuration
	 *Creates context 1.
	 *APN: carspy.ece.uprm.edu
	 *IP address: dynamically assigned by the ISP
	 *Packet Data Protocol type: Internet Protocol (IP)
	 *Data compression: OFF (0)
	 *Header compression: OFF (0)
	 */
	//		i=seg;
	//		while(i--){}
	//		UARTSend((unsigned long)GSM, (unsigned char *) "AT+CGDCONT=1,IP,internet.claropr.com,0.0.0.0",44);//52 with carspy.ece...
	//		UARTCharPut(UART1_BASE, 13);
	//		UARTCharPut(UART1_BASE, 26);
	//		SysCtlDelay(10000000);


	/* INITIAL CONFIGURATION
	 * The TCP/IP stack behavior must be configured by setting:
	 * The packet default size: 512
	 * The socket inactivity timeout: 90
	 * The connection timeout: 300
	 * The data sending timeout: 100
	 */

	//		i=seg;
	//		while(i--){}
	//		UARTSend((unsigned long)GSM, (unsigned char *) "AT#SCFG=1,1,512,90,300,100 <cr> <lf>", 36);
	//		UARTCharPut(UART1_BASE, 13);
	//		UARTCharPut(UART1_BASE, 26);

	/*
	 * Check assigned context Id's. NOT WORKING ATM
	 */
	//			i=seg;
	//			while(i--){}
	//			UARTSend((unsigned long)GSM, (unsigned char *) "AT+CGDCONT?", 11);
	//			UARTCharPut(UART1_BASE, 13);
	//			UARTCharPut(UART1_BASE, 26);

	/*When enabled, device related errors cause the +CME ERROR: <err>
	 * final result code instead of the default ERROR final result code.
	 * ERROR is anyway returned normally when the error message is
	 * related to syntax, invalid parameters, or DTE functionality
	 */

	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)GSM, (unsigned char *) "AT+CMEE=1", 9);
	//	UARTCharPut(GSM, 13);
	//	UARTCharPut(GSM, 26);

	/*
	 * Return activated context.
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)GSM, (unsigned char *) "AT+CGDCONT?", 11);
	//	UARTCharPut(GSM, 13);
	//	UARTCharPut(GSM, 26);

	/*
	 * Returns IP Assigned by Claro for GSM for Client service.
	 */
	//		i=seg;
	//		while(i--){}
	//		UARTSend((unsigned long)GSM, (unsigned char *) "AT#CGPADDR=1", 12);
	//		UARTCharPut(GSM, 13);

	/*
	 * SELINT is 2.
	 */
	//			i=seg;
	//			while(i--){}
	//			UARTSend((unsigned long)GSM, (unsigned char *) "AT#SELINT?", 10);
	//			UARTCharPut(GSM, 13);
	//			UARTCharPut(GSM, 26);
	/*
	 * Activate context defined in AT+CGDCONT. Returns IP assigned by network.
	 * On GSM Power On This Command Has To Be Sent.
	 * 0 - Disactivate | 1 - Activate Context
	 */
	i=seg;
	while(i--){}
	UARTSend((unsigned long)GSM, (unsigned char *) "AT#SGACT=1,1", 12);
	UARTCharPut(GSM, 13);
	UARTCharPut(GSM, 26);
	//	DELAY AQUI PARA PODER LEER EL IP BIEN.
	SysCtlDelay(100000000);//12 second delay

	while(!sFlag)
	{
		i=seg;
		while(i--){}
		UARTSend((unsigned long)GSM, (unsigned char *) "AT#SD=1,0,6102,carspy.ece.uprm.edu", 34);
		UARTCharPut(GSM, 13);

		//Delay to assure socket connection.
		SysCtlDelay(50000000);//6 second
		if(response[a-5] == 'E' && response[a-4] == 'C' && response[a-3] == 'T')
			sFlag = 1;
		else if(response[a-5] == 'R' && response[a-4] == 'O' && response[a-3] == 'R' && !sFlag)
			a = 0;


	}

}

int main(void)
{

	//Set Clock for MCU to user. 50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//Setup all UART peripherals to be used.
	UARTSetup();
	// Enable processor interrupts
	IntMasterEnable();
	IntEnable(INT_UART3);
	UARTIntEnable(GSM, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');

	//Verify GSM is connected to carrier.
	checkConnection();

	SysCtlDelay(10000000);//~1 Second Delay
	if(connEstablished == 1)
	{
		//	Initial Configuration of GSM is done before shipping. Instructions are set
		TCPConnect();
	}

	SendData((unsigned char*)response);


	//Check available ports using `netstat -vatn

		/*
	 * HAS to be sent after a connection has been established. It requires +++ to exit Data Traffic Mode (Socket Mode)
	 */
	//		i=seg;
	//		while(i--){}
	//		UARTSend((unsigned long)UART1_BASE, (unsigned char *) "+++",3);
	//		SysCtlDelay(100000000); //Needs 2 second delay between the command and the LF and CR characters for correct reading of pause command.
	//		UARTCharPut(UART1_BASE, 13);
	//		UARTCharPut(UART1_BASE, 26);

	// AFTER +++ #SO continues connection.


	/*
	 * Prints out possible baud rates.
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)GSM, (unsigned char *) "AT+IPR=?",8);
	//	UARTCharPut(GSM, 13);
	//	UARTCharPut(GSM, 26);

	while(1){}

	return 0;
}