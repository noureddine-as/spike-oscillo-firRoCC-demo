/* 
 * Timer Registers Map
 */
#define TIMER_ADDRESS_ENABLE   	 	 0x00002010
#define TIMER_ADDRESS_TIMERLOAD  	 0x00002014
#define TIMER_ADDRESS_TIMEOUT    	 0x00002018
#define TIMER_ADDRESS_TIMERVALUE 	 0x0000201C
#define TIMER_ADDRESS_TIMERINTENABLE 0x00002020

/*
 * Defines
 */
#define TIMER_ADDRESS_MASKTIMERINT0  0x00000001
#define TIMER_ADDRESS_MASKTIMERINT1  0x00000002
#define TIMER_ADDRESS_MASKTIMERINT2  0x00000004
#define TIMER_ADDRESS_MASKTIMERINT3  0x00000008

/*
 * Prototypes functions
 */
void Timer_Enable(void);
void Timer_Disable(void);
void Timer_Load(unsigned int);
void Timer_Delayus(unsigned int);
unsigned int Timer_getValue(void);
unsigned int Timer_Timeout(void);
void Timer_EnableINT0(void);
void Timer_EnableINT1(void);
void Timer_EnableINT2(void);
void Timer_EnableINT3(void);
void Timer_DisableINT0(void);