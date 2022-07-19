#include "Keypad.h"
void KeypadInit(void) //Keypad Initialisation function
{
	unsigned long delay;
	//Init PORTE
	SYSCTL_RCGC2_R |= 0x00000010;     // E clock
	delay = SYSCTL_RCGC2_R;	
	GPIO_PORTE_CR_R = 0x0F;           // allow changes to PE3-0
	GPIO_PORTE_AMSEL_R = 0x00;				// disable analog function
	GPIO_PORTE_PCTL_R = 0x00000000;		// GPIO clear bit PCTL 
	GPIO_PORTE_DIR_R = 0x00;					// PE3, PE2, PE1, PE0 Inputs
	GPIO_PORTE_AFSEL_R = 0x00;        // no alternate function
	GPIO_PORTE_PDR_R = 0x0F;// pull down resistors
	GPIO_PORTE_DEN_R = 0x0F;          // enable digital pins PE3-PE0 


	SYSCTL_RCGC2_R |= 0x00000008;     // D  clock
	delay = SYSCTL_RCGC2_R;
	//Init PORTD
	GPIO_PORTD_CR_R = 0x0F;           // allow changes to PD3-0
	GPIO_PORTD_AMSEL_R = 0x00;				// disable analog function
	GPIO_PORTD_PCTL_R = 0x00000000;	  // GPIO clear bit PCTL 
	GPIO_PORTD_DIR_R = 0x0F;					// PD3, PD2, PD1, PD0 Outputs
	GPIO_PORTD_AFSEL_R = 0x00;        // no alternate function
	GPIO_PORTD_DEN_R = 0x0F;          // enable digital pins PD3-PD0 
}

char readKeypad(){					// Returns button pressed as char variable
	//D0 high
	PORTD0 = 0x01;	PORTD1 = 0x00; PORTD2 = 0x00; PORTD3 = 0x00;
SysTick_Wait(1200); //150 us delay
	if(PORTE0 == 0x01) {return '1';}
	else if(PORTE1 == 0x02) {return '4';}
	else if(PORTE2 == 0x04) {return '7';}
	else if(PORTE3 == 0x08) {return '*';}
	
	//D1 high
	PORTD0 = 0x00;	PORTD1 = 0x02; PORTD2 = 0x00; PORTD3 = 0x00;
SysTick_Wait(1200); //150 us delay
	if(PORTE0 == 0x01) {return '2';}
	else if(PORTE1 == 0x02) {return '5';}
	else if(PORTE2 == 0x04) {return '8';}
	else if(PORTE3 == 0x08) {return '0';}	
	
	//D2 high
	PORTD0 = 0x00;	PORTD1 = 0x00; PORTD2 = 0x04; PORTD3 = 0x00;
SysTick_Wait(1200); //150 us delay
	if(PORTE0 == 0x01) {return '3';}
	else if(PORTE1 == 0x02) {return '6';}
	else if(PORTE2 == 0x04) {return '9';}
	else if(PORTE3 == 0x08) {return '#';}	
	
		//D3 high
	PORTD0 = 0x00;	PORTD1 = 0x00; PORTD2 = 0x00; PORTD3 = 0x08;
	SysTick_Wait(1200); //150 us delay
	if(PORTE0 == 0x01) {return 'A';}
	else if(PORTE1 == 0x02) {return 'B';}
	else if(PORTE2 == 0x04) {return 'C';}
	else if(PORTE3 == 0x08) {return 'D';}
	
	else {return 'N';} //return N if nothing is pressed
}
