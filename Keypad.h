#ifndef _KEYPAD_H_
#define _KEYPAD_H_

//Keypad functions

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
	
//PORTE define
//Keypad rows: input from PORTE [0:3].
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514)) //Row inputs will need pull-down resistors.
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
//Individual pins
#define PORTE0 									(*((volatile unsigned long *)0x40024004))
#define PORTE1									(*((volatile unsigned long *)0x40024008))
#define PORTE2									(*((volatile unsigned long *)0x40024010))
#define PORTE3									(*((volatile unsigned long *)0x40024020))

//PORTD define
//Keypad columns: output to PORTD [0:3].
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C)) 	
//Individual pins
#define PORTD0									(*((volatile unsigned long *)0x40007004))
#define PORTD1									(*((volatile unsigned long *)0x40007008))
#define PORTD2									(*((volatile unsigned long *)0x40007010))
#define PORTD3									(*((volatile unsigned long *)0x40007020))
void SysTick_Wait(unsigned long delay);
void KeypadInit(void);
char readKeypad(void);

#endif
