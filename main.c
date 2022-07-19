// ***** 1. Pre-processor Directives Section *****
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "Keypad.h"
#include "LCD.h"
#include "PLL.h"
#include "FlashProgram.h"
// ***** 2. Global Declarations Section *****
unsigned char In ;
unsigned char Inprev;
bool shift = 0;
bool password_change = 0;
unsigned char password_in[5];
unsigned char new_password[5];
int p = 0; //used for password
int n = 0; //associated with tbuf
int i = 0; //associated with math_exp
char tbuf[200]; //token buffer
char symbol;
char math_exp[100]; //mathematical expression string
char answer[100]; //answer string

float ans = 0;
int error = 0; // error flag
enum { ADDSUB, MULDIV, POWTEN , LEFT_P, RIGHT_P, NUMBER, END } look_ahead;

void initialise_parser(void);
void next(void);
void read(void);
void reset(void);
int scan(void);
float expression(void);
float unsigned_factor(void);
float factor(void);
float pow_factor(void);
float term(void);
void reset_math(void);
char returnInput(char c, bool s);
void append (char *string, char c);
void delay_100ms(int t);
void lcdUpdate(void);
void delay_100ms(int t);
void createPatterns(void);
void displayAnimation(char location);
uint32_t *ptr = (uint32_t *)0x00008000; //Password memory address 
uint32_t new_password_int;
#define FLASHADDRESS 0x00008000
#define PROTECTION 1 // 1 - ON / 0 - OFF

unsigned char Pattern0[]= {0x07,0x08,0x12,0x10,0x14,0x13,0x08,0x07}; 
unsigned char Pattern1[]= {0x1C,0x02,0x09,0x01,0x05,0x19,0x02,0x1C};
unsigned char Pattern2[]= {0x09,0x05,0x03,0x01,0x01,0x03,0x04,0x08}; 
unsigned char Pattern3[]= {0x12,0x14,0x18,0x10,0x10,0x08,0x04,0x02}; 
unsigned char Pattern4[]= {0x03,0x05,0x09,0x01,0x01,0x01,0x02,0x06}; 
unsigned char Pattern5[]= {0x18,0x14,0x12,0x10,0x10,0x10,0x08,0x0C}; 

// ***** 3. Subroutines Section *****
int main (void) {
	uint32_t password = *ptr; //Password value
	KeypadInit();
	LCDPort_Init();
	InitDisplayPort();
	createPatterns();

	if(PROTECTION){
	moveDisplayCursor(0x00);
	printDisplay("Enter Password");	
	moveDisplayCursor(0x40);
	while(1){
		In = readKeypad();
		if(Inprev != In){ // condition used to avoid multiple reads while key is pressed
			if(In != 'N'){
			append(password_in, In);
			writeDisplay('*');
			p++;
			}
		Inprev = In;
		delay_100ms(1);
		}
		
		if (p == 4){ //Password length is 4 characters
			uint32_t password_int;
			password_int = atoi(password_in);	//Convert string to int	
			if(password_int == password){p=0; break;}
			else{
				p = 0;
				password_in[0] = '\0';
				clearDisplay();
				SysTick_Wait(500000);
				printDisplay("Wrong password");
				delay_100ms(20);
				clearDisplay();
				SysTick_Wait(500000);
				printDisplay("Enter Password");
				moveDisplayCursor(0x40);
			}
    }
	}
	//--------------PASSWORD ACCEPT ANIMATION---------------
	clearDisplay();
	delay_100ms(1);
	moveDisplayCursor(0x00);
	printDisplay("Accepted");
	int counter = 0;
	while(counter != 3){
	displayAnimation(0x0A);
	counter++;
	}
	//----------------------------------------------------
	clearDisplay();
	delay_100ms(1);
	printDisplay("Press A to");
	moveDisplayCursor(0x40);
	printDisplay("Change Password");
	delay_100ms(2);
	
	//--------------PASSWORD CHANGE ROUTINE---------------
	while(1){
		In = readKeypad();
		if(In == 'A'){password_change = 1; break;}
		else if (In != 'A' && In != 'N') break;
	}
	if(password_change == 1){
		while(1){
		new_password[0] = '\0';
		p = 0; //
		clearDisplay();
		delay_100ms(1);
		printDisplay("Enter New PSW:");
		moveDisplayCursor(0x40);
		printDisplay("4 digits: ");
		Inprev = In;
		while(1){
			In = readKeypad();
			if(Inprev != In){ // condition used to avoid multiple reads while key is pressed
			if(In != 'N' && isdigit(In)){
			append(new_password, In);
			writeDisplay(In);
			p++;
			}
			if(In == '*') break;
			Inprev = In;
			delay_100ms(1);
		}
	}
		if(p == 4){
		clearDisplay();
		delay_100ms(1);
		printDisplay("New PSW:");
		printDisplay(new_password);
		new_password_int = atoi(new_password);	//Convert string to int	
		Flash_Erase(FLASHADDRESS);
		SysTick_Wait(8000000);
		Flash_Write(FLASHADDRESS,new_password_int);
		SysTick_Wait(8000000);
		delay_100ms(30);
		break;
		}	
		else{
		clearDisplay();
		delay_100ms(1);
		printDisplay("Invalid PSW");
		moveDisplayCursor(0x40);
		printDisplay("NOT 4 DIGITS");
		delay_100ms(20);
		}
}
}	
	clearDisplay();
	Inprev = In;
}
	
	while(1){	
		reset_math();	
		error = 0;
		//--------------INPUT ROUTINE---------------
		while(1){
			In = readKeypad();
			if (In == '*') break; //END INPUT
			In = returnInput(In, shift); // Convert into mathematical character
			if(Inprev != In){ // condition used to avoid multiple reads while key is pressed
			if(In == 'D') shift = !shift; // control SHIFT flag
			if(In != 'N' && In != 'R' && In != 'Z' && In != 'D' && In != 'R' && In != 'Z' && In != 'A') {
				writeDisplay(In);
				append(math_exp, In); //Concatenate into mathematical expression
			}
			
			if(In == 'A') { //Get previous answer and concatenate into math_exp
				SysTick_Wait(500000);
				printDisplay(answer);
				strcat(math_exp, answer);
				}
			if(In == 'R') {
				math_exp[strlen(math_exp)-1] = '\0'; // Remove last character
				lcdUpdate();//Update
				}
			
			if(In == 'Z') {math_exp[0] = '\0'; // Empty string
				lcdUpdate();
			}
			Inprev = In;
			delay_100ms(1); //short delay
		}
	}
		//--------------EVALUATION ROUTINE---------------
		initialise_parser();
		ans = expression();
		sprintf(answer, "%g", ans);
		moveDisplayCursor(0x40);
		if(error == 1){strcpy(answer,"Error");}
		lcdUpdate();
		
		//--------------END ROUTINE---------------	
		while(1){
		In = readKeypad();
		if (In != 'N' && In != '*') break;
	}
		clearDisplay();
		delay_100ms(2);
		moveDisplayCursor(0x40);
		printDisplay(answer);
		moveDisplayCursor(0x00);		
	}
}

char returnInput(char c, bool s){		//Checks if shift is applied and return correct character
		if ((!s) && isdigit(c)) return c;
		else if ((!s) && (c == 'A')) return '+';
		else if ((!s) && (c == 'B')) return '-';
		else if ((!s) && (c == 'C')) return '.';
		else if ((!s) && (c == 'D')) return 'D';
		else if ((!s) && (c == '#')) return 'R'; //used for removal of last character
		else if (s && (c == '#')) return 'Z'; //used for deleting all elements in char array
		else if (s && (c == 'A')) return 'x';
		else if (s && (c == 'B')) return '/';
		else if (s && (c == 'C')) return 'E';
		else if (s && (c == 'D')) return 'D';
		else if (s && (c == '1')) return '(';
		else if (s && (c == '2')) return ')';
		else if (s && (c == '3')) return '^';
		else if (s && (c == '4')) return 'A';
		else return 'N';
}
void delay_100ms(int t){ // delay (n * 0.1 ms)
	int j = 0;
	for(j=0; j<t;j++) {
		SysTick_Wait(8000000);
	}
}
void reset_math(void){
		math_exp[0] = '\0';
		i=0;
}
void append(char* string, char c) {
			unsigned int len = strlen(string);
			string[len] = c;
			string[len+1] = '\0';
}
void lcdUpdate(void){
			clearDisplay();
			SysTick_Wait(500000);
			moveDisplayCursor(0x40);
			printDisplay(answer);
			moveDisplayCursor(0x00);
			SysTick_Wait(500000);
			printDisplay(math_exp);
}
void read(){
  tbuf[n] = symbol;
	n++;
  tbuf[n] = '\0';
  symbol = math_exp[i];
	i++;
}

void reset(){ 
  n = 0;
  tbuf[0] = '\0';
}

int scan(){
  reset();
START:
  switch (symbol) {
    case '-': case '+':
      read();
      return ADDSUB;

    case 'x': case '/':
      read();
      return MULDIV;

		case 'E': case '^': case 'e':
      read();
      return POWTEN;

    case '(':
      read();
      return LEFT_P;

    case ')':
      read();
      return RIGHT_P;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': 
		case '.':
      read();
			if(isdigit(symbol) || symbol == '.')goto START;
      else return NUMBER;

    case '\0':
      return END;

    default:
      error = 1;
  }
}

void next(){
  look_ahead = scan();
}

void initialise_parser(){
  reset();
  symbol = math_exp[i];
	i++;
  next();
}

float unsigned_factor()
{
    float output = 0;

    if(error == 0){
    if(look_ahead == NUMBER ) {
        sscanf(tbuf, "%f", &output);
        next();
        }
    else if (look_ahead == LEFT_P ) {
        next();
        output = expression();
        if (look_ahead != RIGHT_P) error = 1;
        next();
        }
    else error = 1;
    return output;
    }
    else return 0;
}

float factor()
{
  float output = 0;
  if(error == 0){
  if (look_ahead == ADDSUB && tbuf[0] == '-') {
    next();
    output = -unsigned_factor();
  }
  else
    output = unsigned_factor();
  return output;
  }
  else return 0;
}

float pow_factor()
{
  float output = factor();
  if(error == 0){
  while (look_ahead == POWTEN) {
        if(tbuf[0] == '^'){
            next();
            output = powf(output, factor());
        }
        else if(tbuf[0] == 'E'){
            next();
            output = output * powf(10, factor());
        }
				else if(tbuf[0] == 'e'){ //  to deal with float formatting when number is fetched from answer
           next();
						if(tbuf[0] == '-') output = output * powf(10, factor());
						else if (tbuf[0] == '+'){
							next();
							output = output * powf(10, factor());
						}
        }
  }
  return output;
  }
  else return 0;
}

float term()
{
  float output = pow_factor();
	float denominator;
  if(error == 0){
  while (look_ahead == MULDIV) {
    if(tbuf[0] == 'x') {
        next();
        output = output * pow_factor();
    }
    else if(tbuf[0] == '/'){
        next();
				denominator = pow_factor();
        if (denominator != 0)output = output / denominator;
				else error = 1;
    }
  }
  return output;
  }
  else return 0;
}

float expression()
{
  float output = term();
  if(error == 0){
  while (look_ahead == ADDSUB) { 
    if(tbuf[0] == '+') {
        next();
        output = output + term();
    }
    else if(tbuf[0] == '-') {
        next();
        output = output - term();
    }
  }
  return output;}
  else return 0;
}
void createPatterns(void){
CreateCustomCharacter(Pattern0, 0);
CreateCustomCharacter(Pattern1, 1);
CreateCustomCharacter(Pattern2, 2);
CreateCustomCharacter(Pattern3, 3);
CreateCustomCharacter(Pattern4, 4);
CreateCustomCharacter(Pattern5, 5);
}
void displayAnimation(char location){
	moveDisplayCursor(location);
	writeDisplay(0);
	moveDisplayCursor(location+0x01);
	writeDisplay(1);
	moveDisplayCursor(location+0x40);
	writeDisplay(2);
	moveDisplayCursor(location+0x41);
	writeDisplay(3);
	delay_100ms(5);
	moveDisplayCursor(location+0x40);
	writeDisplay(4);
	moveDisplayCursor(location+0x41);
	writeDisplay(5);
	delay_100ms(5);
}
