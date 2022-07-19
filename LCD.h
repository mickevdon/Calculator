#ifndef _LCD_H_
#define _LCD_H_

// LCD initialisation and interfacing
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
//PORTA define
//#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))

//PORTB [0:3] define
//#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
	
#define LCD_EN									(*((volatile unsigned long *)0x40004010)) //PA2
#define LCD_RS									(*((volatile unsigned long *)0x40004020)) //PA3
#define LCD_DB									(*((volatile unsigned long *)0x4000503C)) //PB[0:3]

// The SysTick_Wait(unsigned long delay) delay parameter is in units of the 80 MHz core clock. (12.5 ns)
// Time delay define statements
#define delay500ns 40
#define delay5000ns 400
#define delay100ms 8000000
#define delay4500us 360000
#define delay150us 12000
void SysTick_Init(void);
void PLL_Init(void);
void SysTick_Wait(unsigned long delay);
void SendDisplayNibble(char RS, char DB);
void SendDisplayByte(char RS, char DB);
void Pulse_EN(unsigned long time);
void LCDPort_Init(void);
void InitDisplayPort(void);
void clearDisplay(void);
void moveDisplayCursor(char address);
void writeDisplay(char c);
void printDisplay(char *string);
void CreateCustomCharacter (unsigned char *Pattern, const char Location);

#endif
