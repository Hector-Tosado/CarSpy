#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
int main(void)
{
  //unsigned long ulPeriod;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//declaration
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_2);
	SysCtlPeripheralClockGating(1);
	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA|SYSCTL_PERIPH_GPIOF);

	//interrupts
	GPIOPinIntClear(GPIO_PORTA_BASE,GPIO_PIN_2);
	GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_HIGH_LEVEL);
	GPIOPinIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);
	IntEnable(INT_GPIOA);

	//SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	//TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	//ulPeriod = (SysCtlClockGet() / 10) / 2;
	//TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);
	//IntEnable(INT_TIMER0A);
	//TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	SysCtlSleep();
	//TimerEnable(TIMER0_BASE, TIMER_A);
	while(1)
	{

			SysCtlDelay(64000000);
			//Aqui estara todo el codigo de carspy
			//luego de que el codigo termina habra
			//estas lineas para activar el interrupt y poner a dormir el micro
			//IntEnable(INT_GPIOA);
			//SysCtlSleep();
	}
}
void gpioAHandler(void)
{
	// Clear the timer interrupt
	IntDisable(INT_GPIOA);
	GPIOPinIntClear(GPIO_PORTA_BASE,GPIO_PIN_2);
	//TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}



}
