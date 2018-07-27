#include "timer.h"

void Timer_Enable(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_ENABLE = 1;
}

void Timer_Disable(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_ENABLE = 0;
}

void Timer_Load(unsigned int l)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERLOAD = l;	
}

unsigned int Timer_getValue(void)
{
	return *(volatile unsigned int *)TIMER_ADDRESS_TIMERVALUE;
}

unsigned int Timer_Timeout(void)
{
	return *(volatile unsigned int *)TIMER_ADDRESS_TIMEOUT;
}

void Timer_Delayus(unsigned int d)
{
	Timer_Disable();
	Timer_Load(d*500);
	Timer_Enable();
	while(!Timer_Timeout());
	Timer_Disable();
}

void Timer_EnableINT0(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE = TIMER_ADDRESS_MASKTIMERINT0;
}

void Timer_DisableINT0(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE = *(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE && (~TIMER_ADDRESS_MASKTIMERINT0);	
}


void Timer_EnableINT1(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE = TIMER_ADDRESS_MASKTIMERINT0;
}

void Timer_EnableINT2(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE = TIMER_ADDRESS_MASKTIMERINT0;
}

void Timer_EnableINT3(void)
{
	*(volatile unsigned int *)TIMER_ADDRESS_TIMERINTENABLE = TIMER_ADDRESS_MASKTIMERINT0;
}	