#include "LCD.h"
void LCDPort_Init(void){

	unsigned long delay;
	//Init for PortA[2:3]
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) A clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTA_CR_R = 0x0C;           // allow changes to PA2-3       
  GPIO_PORTA_AMSEL_R = 0x00;        // disable analog function
  GPIO_PORTA_PCTL_R = 0x00000000;   // GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R = 0x0C;          // PA2, PA3 Outputs
  GPIO_PORTA_AFSEL_R = 0x00;        // no alternate function 
  GPIO_PORTA_DEN_R = 0x0C;          // enable digital pins PA2-3   

	//Init for PortB[]
	SYSCTL_RCGC2_R |= 0x00000002;     // B clock
  delay = SYSCTL_RCGC2_R;           // delay   
	GPIO_PORTB_CR_R = 0x0F;           // allow changes to PB0-PB3	
  GPIO_PORTB_AMSEL_R = 0x00;        // disable analog function
  GPIO_PORTB_PCTL_R = 0x00000000;   // GPIO clear bit PCTL  
	GPIO_PORTB_DIR_R = 0x0F;         // PB0, PB1, PB2, PB3 Outputs
  GPIO_PORTB_AFSEL_R = 0x00;        // no alternate function   
 GPIO_PORTB_DEN_R = 0x0F;          // enable digital pins to PB0-PB3 
}

void Pulse_EN(unsigned long time){ 		// Set EN high
	LCD_EN |= 0x04;
	SysTick_Wait(time);
	LCD_EN &= ~0x04;
}

void SendDisplayNibble(char RS, char DB){
	LCD_RS = RS; // 0x00 or 0x08
	SysTick_Wait(delay5000ns);
	LCD_DB = DB;
	Pulse_EN(delay5000ns);
	SysTick_Wait(delay150us); //at least 37 us delay
}

void SendDisplayByte(char RS, char DB){
	LCD_RS = RS; // 0x00 or 0x08 
	SysTick_Wait(delay500ns);	
	LCD_DB = (DB&0xF0) >>4; //Send first nibble

	Pulse_EN(delay5000ns);
	
	LCD_DB = (DB&0x0F);
	Pulse_EN(delay5000ns);
	SysTick_Wait(delay150us); //at least 37 us delay
}


void InitDisplayPort(void){
	PLL_Init();
	SysTick_Init();

  //Set EN
	LCD_EN = 0x00;
	
	// Power ON then delay (at least 40ms)
	SysTick_Wait(delay100ms);
	
	SendDisplayNibble(0x00, 0x03); 	//Send intruction 0011 to DB[4:7] (PORTB[0:3])
	SysTick_Wait(delay4500us);	// Delay (at least 4.1ms)
	
	SendDisplayNibble(0x00, 0x03);	//Send intruction 0011 to DB[4:7] (PORTB[0:3])
	SysTick_Wait(delay150us);	 // Delay (at least 100us)
	
	SendDisplayNibble(0x00, 0x03);	//Send intruction 0011 to DB[4:7] (PORTB[0:3])
	SysTick_Wait(delay150us);	 // Delay (at least 100us)
	
	SendDisplayNibble(0x00, 0x02); 	//Send intruction 0010 to DB[4:7] (PORTB[0:3])
	SysTick_Wait(delay150us);	 // Delay (at least 100us)
	
	//After this points device is in 4 bit mode.
	//
	SendDisplayByte(0x00, 0x28); // Function set instruction. 4bit D=0; 2 line N=1; 5x8 font F=0 ||| 0010 1000
	SysTick_Wait(delay150us);	 // Delay 
	
	SendDisplayByte(0x00, 0x08); // Display Off
	SysTick_Wait(delay150us);	 // Delay 
	
	SendDisplayByte(0x00, 0x01); // Display Clear
	SysTick_Wait(delay4500us);	 // Delay 
	
	SendDisplayByte(0x00, 0x06); // Entry mode I/D=1 to increment and S=0 to not shift
	SysTick_Wait(delay150us);	 // Delay 
	//Initialization ends.
	
	SendDisplayByte(0x00, 0x0F); // Display on cursor visible blinking ON11
	SysTick_Wait(delay150us);	 // Delay 
	
	
}
void moveDisplayCursor(char address){ //  First row (0x00 - 0x0F) Second row (0x40 - 0x4F) 
	SendDisplayByte(0x00, (address|0x80));
}
void clearDisplay(void){
	SendDisplayByte(0x00, 0x01); // Clear display
	SysTick_Wait(delay150us);
	moveDisplayCursor(0x00); // Cursor go to 0x00
	SysTick_Wait(delay150us);
}

void writeDisplay(char c){
	SendDisplayByte(0x08, c);
}
	
void printDisplay(char *string){
	char *c = string;
	while (*c) SendDisplayByte(0x08, *(c++) );
}
void CreateCustomCharacter (unsigned char *Pattern, const char Location)
{ 
int m=0; 
SendDisplayByte(0x00,0x40+(Location*8));     //Send the Address of CGRAM
SysTick_Wait(delay150us);
for (m=0; m<8; m++){
SendDisplayByte(0x08,Pattern[ m ]); 
SysTick_Wait(delay150us);}	//Pass the bytes of pattern on LCD 
}