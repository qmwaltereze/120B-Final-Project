
/*	Author: qwalt002
 *  Partner(s) Name: kyle
 *	Lab Section:
 *	Assignment: Lab # 6 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 * Create a synchSM to blink three LEDs connected to PB0, PB1, and PB2 in sequence, 1 second each. Implement that synchSM in C using the method defined in class. In addition to demoing your program, you will need to show that your code adheres entirely to the method with no variations. 


 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "eeprom.h"
#include "timer.h"
#include "usart.h"
//#include "keypad.h"
//#include "scheduler.h"
//#include "io.c"


void set_PWM(double frequency) {
    static double current_frequency; // Keeps track of the currently set frequency
// Will only update the registers when the frequency changes, otherwise allows 
// music to play uninterrupted.
    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
        else { TCCR3B |= 0x03; } // resumes/continues timer/counter            
        
        // prevents OCR3A from overflowing, using prescaler 64
        // 0.954 is smallest frequency that will not result in overflow
        if (frequency < 0.954) { OCR3A = 0xFFFF; }
    
        // prevents OCR3A from underflowing, using prescaler 64                    // 31250 is largest frequency that will not result in underflow    
        else if (frequency > 31250) { OCR3A = 0x0000; }
    
        // set OCR3A based on desired frequency            
        else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

        TCNT3 = 0; // resets counter
        current_frequency = frequency; // Updates the current frequency
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);
        // COM3A0: Toggle PB3 on compare match between counter and OCR3A
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);    
        // WGM32: When counter (TCNT3) matches OCR3A, reset counter
        // CS31 & CS30: Set a prescaler of 64
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}




//Functionality - Sets bit on a PORTx
//Parameter: Takes in a uChar for a PORTx, the pin number and the binary value 
//Returns: The new value of the PORTx
unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value) 
{
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets bit from a PINx
//Parameter: Takes in a uChar for a PINx and the pin number
//Returns: The value of the PINx
unsigned char GetBit(unsigned char port, unsigned char number) 
{
	return ( port & (0x01 << number) );

}
// Keypad Setup Values
#define KEYPADPORT PORTC
#define KEYPADPIN  PINC
#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3
#define COL1 4
#define COL2 5
#define COL3 6
#define COL4 7



////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets input from a keypad via time-multiplexing
//Parameter: None
//Returns: A keypad button press else '\0'
unsigned char GetKeypadKey() {

	// Check keys in col 1
	KEYPADPORT = SetBit(0xFF,COL1,0); // Set Px4 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '1'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '4'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '7'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '*'; }

	// Check keys in col 2
	KEYPADPORT = SetBit(0xFF,COL2,0); // Set Px5 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '2'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '5'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '8'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '0'; }

	// Check keys in col 3
	KEYPADPORT = SetBit(0xFF,COL3,0); // Set Px6 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '3'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '6'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '9'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '#'; }

	// Check keys in col 4
	KEYPADPORT = SetBit(0xFF,COL4,0); // Set Px7 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if (GetBit(~KEYPADPIN,ROW1) ) { return 'A'; }
	if (GetBit(~KEYPADPIN,ROW2) ) { return 'B'; }
	if (GetBit(~KEYPADPIN,ROW3) ) { return 'C'; }
	if (GetBit(~KEYPADPIN,ROW4) ) { return 'D'; }
	
	return '\0';
}

http://www.handsonembedded.com/lcd16x2-hd44780-tutorial-5/ 
http://saeedsolutions.blogspot.com/2012/12/how-to-display-custom-characters-on-lcd.html
void LCD_BuildCharacter(unsigned char pick_Pattern, unsigned char *pattern){
	unsigned char i;
	
	if(pick_Pattern < 8){
		
		LCD_WriteCommand(0x40 + (pick_Pattern*8));
		
		for(i=0; i<8; i++){
			LCD_WriteData(pattern[i]);
		}
	}
	LCD_WriteCommand(0x80);
}



//scheduler.h---------------------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a % b;
		if( c == 0 ) { return b; }
		a = b;
		b = c;
	}
	return 0;
}


char codes[4] = {'0','0','0','\0'};
unsigned char led1 = 0x00;
unsigned char led2 = 0x00;
unsigned char newcodeInput = 0x00;
unsigned char s = 0;
unsigned char new = 0;

unsigned char days = 0;
unsigned char count = 0;
unsigned char Q = 0;
unsigned char led = 0x00;
unsigned char lit = 0x00;

unsigned char character[8] = {0x0E,0x11,0x11, 0x1F, 0x1B,0x1B,0x1F,0x00};
unsigned char c[8] = {0x0E,0x10,0x10, 0x1F, 0x1B,0x1B,0x1F,0x00};

char data[9] = {'1','2',':','0','0',' ','A','M','\0'}; 
char M = 0;
char I = 0;
char H = 0;
char AMPM = 0;


void updatminutes(char minutes){
	if((minutes/10 +48) == '6'){
	data[3] = '0';
	} 
	else{
	data[3] = minutes/10 + 48;
	}
	minutes = minutes % 10;
	data[4] = minutes + 48;

	LCD_DisplayString(25,data);
}
void updatehour(char hour){
	data[0] = hour/10 + 48;
	hour = hour % 10;
	data[1] = hour + 48;
	
	LCD_DisplayString(25,data);
	
}
void updateAMPM(char ampm){
	if(ampm){
		data[6]= 'P';
		/*if((H == 12) || (H>=0 && H<=5) || (H>=5 && H<=11)){
		led = 0x01;}*/
	}
	else{
		data[6] = 'A';
		/*if(H==0){
		led = 0x02;}*/
		/*if((H == 12) || (H>=0 && H<=5) || (H>=5 && H<=11)){
		led = 0x02;}*/
		
	}
	LCD_DisplayString(25,data);
}

////////////////////////////////////////////////////////////////////////////////
//Struct for Tasks represent a running process in our simple real-time operating system
typedef struct _task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
}_task;



//--------Find GCD function --------------------------------------------------

//-----State MAchine Implemenation--------------------------------------------

// Displaying clock time
// countinuosly displays the current clock time 
// in mock setup _ _:_ _ AM/PM
enum clockDisplay_state {init, clockMin, clockHour};
int clockTime(int state){




//state transitions
switch(state){

	case init:
		if(I == 60){
			state = clockMin;
			
		}
		else
			{state = init;}
		I++;
	break;
	case clockMin:
		if(M == 60){
			state = clockHour;		
		}
		else
			state = init;	
	break;
	case clockHour:
			state = init;
		
	break;
}
//STATE ACTIONS
switch(state){
	case init:
		break;

	case clockMin:
		I = 0;
		M++;
	updatminutes(M);
		
	break;
	case clockHour:
		M = 0;
		if(H == 11){
			AMPM = (AMPM == 0)? 1: 0;
			if (AMPM == 1){
				//count++;
				days++;// = count;
				
				
			}
			/*LCD_Cursor(21);
			LCD_WriteData('0' + days);*/
		}
			
			updateAMPM(AMPM);
			//LCD_Cursor(18);
			//LCD_WriteData(days);

		if(H==12){
			H=0;
		}
		
		H++;
		updatehour(H);
		eeprom_write_byte(0,days);
		
	break;
}
//PORTB = led;
 return state;
}

/*void clockled(){

PORTB =led;
}*/

unsigned char leave = 0;
unsigned char unlock = 0;



unsigned char codeInput;
//unsigned char unlock = 0;

enum UnlockOpen_state { locked, on1, on2, Done };

int UnlockDoor(int state){

	unsigned char led = 0x00;

        switch (state){
		case locked:
			
			if(Q==0 && (unlock == 0)){
				state =locked;
			}
			else if(!(codeInput == codes[0])){
				state = locked;
				if(unlock==0){
					LCD_DisplayString(3,"Enter Code");
					LCD_BuildCharacter(0, character);
					LCD_WriteData(0x00);
				}
				else{
				LCD_DisplayString(3,"open      ");
				}
			}
			else{
				state = on1;
			}

			break;

		case on1:
			if(Q==0 && (unlock == 0)){
				state =locked;
			}
			else if(!(codeInput == codes[1])){
				state=on1;
			}
			else{
				state=on2;
			}
			
			break;

		case on2:
			if(Q==0 && (unlock == 0)){
				state =locked;
			}
			else if(!(codeInput == codes[2])){
				state = on2;
			}
			else{
				state = Done;
			}
			break;	

		case Done:
				state = locked;
			unlock = (unlock == 0)? 1: 0;
			if(unlock==1){
			//LCD_DisplayString(3,"Come in   ");
			LCD_Cursor(7);
			LCD_BuildCharacter(1,c);
			LCD_WriteData(0x01);}
			break;	
	}

	 switch (state){
		case locked:
			
			//led2 = 0x01;
			break;

		case on1:
			//led2 = 0x02;
			break;

		case on2:
			//led2 = 0x01;
			break;	
	
		case Done:
			//led2 = 0x02;
			break;	
	}
	//PORTB = led;
	return state;
}

//--------------------------------------------------------------

//char data[5] = {'0','0','0','0','\0'};

enum Input_state { initCode, inputCode};
//i=0;
int DoorCode(int state){
	
	
	switch(state){
		case initCode:
			Q=0;
			if((((H == 12)||(H>=0 && H<=6)) && (AMPM == 0)) || ((H>=9 && H<=11) &&(AMPM == 1))){
				if(unlock == 1){
					state = inputCode;
				}
				else{
					state = initCode;
				}
			}
			else{
				state= inputCode;
			}

		case inputCode:
			Q=1;
			if(unlock == 1){
				state = inputCode;
			}
			else if((((H == 12)||(H>=0 && H<=8)) && (AMPM == 1)) || ((H>=7 && H<=11) &&(AMPM == 0))){state = inputCode;}
						
			else{
			state = locked;}
			break;
	}
	
	switch(state){
		case inputCode:
			codeInput = GetKeypadKey();
			//LCD_DisplayString(1," Enter Code ");
			break;
	}
	return state;
}


enum Ring_state { ringOff, ringOn, pressAgain };

int DoorBell(int state){
	
	unsigned char A0 = ~PINA & 0x01;


	switch(state){
		case ringOff:
			if(!A0){
				state = ringOff;
			}
			else if(A0){
				state = ringOn;
			}
			break;
	
		case ringOn:
			state = pressAgain;
			break;

		case pressAgain:
			if(A0){
				state = pressAgain;
			}
			else if(!A0){
				state = ringOff;
			}
			break;
	}
	
	switch(state){
		case ringOff:
			//LCD_DisplayString(1,"         ");
			set_PWM(0);
			break;

		case ringOn:
			set_PWM(236.55);
			LCD_DisplayString(2,"Ding Dong ");
			break;

		case pressAgain:
			set_PWM(0);
			//LCD_DisplayString(1,"         ");
			break;
	}
	return state;
}

enum Lights_state { lightOff, lightOn, /*Morning*/};

int LightsOut(int state){
	switch(state){
		case lightOff:
			/*if(AMPM==0){*/if((((H == 12)||(H>=0 && H<=5)) && (AMPM == 1)) || ((H>=5 && H<=11) && (AMPM == 0))){
				state=lightOff;
			}
			else{
				state = lightOn;
			}
			break;

		case lightOn:
			/*if(AMPM==1){*/if((((H == 12)||(H>=0 && H<=5)) && (AMPM == 0)) || ((H>=5 && H<=11) &&(AMPM == 1))){
				state = lightOn;
			}
			else{
			state = lightOff;//Morning;
			}
			break;

		/*case Morning:
			state=lightOff;
			break;*/
			
	}

	switch(state){
		case lightOff:
			//LCD_DisplayString(17, " ");
			//PORTB = USART_Receive(1);
			USART_Send(0x00,1);
			//lit = 0x00;
			break;

		case lightOn:
			//LCD_DisplayString(17, "*"); //16:16AM
			//PORTB = USART_Receive(1);
			USART_Send(0x01,1);
			//lit = 0x01;
			break;
		
		/*case Morning:
			LCD_ClearScreen();
			break;*/
	}
/*PORTA = lit;*//* PORTA = USART_Receive(1);*/
	return state;
}

enum NewInput_state { NewCode_init, buttonPress, newInputCode, setNewCode };

int NewCode(int state){
//unsigned char newcodeInput = 0x00;
unsigned char A0 = ~PINA & 0x01;

	switch(state){
		case NewCode_init:
			if(!A0){

				state = NewCode_init;	
			}
			else{
				state = buttonPress;
			}
			break;

		case buttonPress:
			if(A0){
				state = buttonPress;
			}
			else {
				state = newInputCode;
			}
			break;

		case newInputCode:
			if(!A0){
				state = newInputCode;	
			}
			else if(A0 && (s==0)){
				state = newInputCode;
			} 
			else if(A0 && (s==1)){ 
				state=setNewCode;
			}
			break;

		case setNewCode:
			if(A0){
				state = setNewCode;	
			}
			else {
				state = NewCode_init;
				s=0;
			}
			break;
	}

	
	switch(state){
		case NewCode_init:
			led1 = 0x00;
			break;

		case buttonPress:
			led1 = 0x00;
			break;

		case newInputCode:
			newcodeInput = GetKeypadKey();
			
			led1 = 0x10;
			break;

		case setNewCode:
			led1 = 0x20;
			break;
	}
	//PORTB=led;
	return state;
	
}

enum change_state { change_init, c1, c2, c3, next, set, done};

int ChangeCode(int state){

 switch (state){
		case change_init:
			state = c1;
			break;

		case c1:
			
			if(newcodeInput == '\0'){
				state = c1;
			}
			else{
				state = c2;
				codes[0] =newcodeInput;
				new = newcodeInput;
				eeprom_write_byte(1,codes[0]);
				
			}

			break;

		case c2:
			if((newcodeInput == '\0')||(newcodeInput == new)){
				state =c2;
			}
			else{
				state=c3;
				codes[1] =newcodeInput; 
				new = newcodeInput;
				eeprom_write_byte(2,codes[1]);
			}
			
			break;

		case c3:
			if((newcodeInput == '\0')||(newcodeInput == new)){
				state =c3;
			}
			else{
				state = set;
				codes[2] =newcodeInput;
				new = newcodeInput; 
				eeprom_write_byte(3,codes[2]);
			}
			break;	
		
		case set:
			state =done;
			new=0;
			//unlock = 1;
			s = (s == 0)? 1: 0;
			break;
	
		case done:
			if(s==1){
			state = done;}
			else{
			state =c1;}
			break;
	}

	 switch (state){
		case c1:
			
			led = 0x08;
			break;

		case c2:
			led = 0x04;
			break;

		case c3:
			led = 0x02;
			break;	
	
		case set:
			led = 0x21;
			break;	

		case done:
			break;	

	}
	//PORTB = led;
	return state;


}

void tick(){
PORTB= led1| led /*| led2*/;
}



int main(void) {
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	//TimerSet(1); // changing this changes the clock speed
	//TimerOn();
	DDRA=0x00; PORTA=0xFF;
	DDRC = 0xF0; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	
	
static _task task1, task2, task3, task4, task5, task6;
_task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6 };
const unsigned short numTasks = 6; //sizeof(tasks)/sizeof(task*);

//task1
task1.state = inputCode;//Task initial state.
task1.period = 1;//Task Period.
task1.elapsedTime = task1.period;//Task current elapsed time.
task1.TickFct = &DoorCode;//Function pointer for the tick.*/
// Task 2 (toggleLED0SM)
task2.state = locked;//Task initial state.
task2.period = 500;//Task Period.
task2.elapsedTime = task2.period;//Task current elapsed time.
task2.TickFct = &UnlockDoor;//Function pointer for the tick.*/
// Task 3 (clock time)
task3.state = init;
task3.period = 1;//Task Period.
task3.elapsedTime = task3.period;//Task current elapsed time.
task3.TickFct = &clockTime;//Function pointer for the tick.
// Task 4 (toggleLED0SM)
task4.state = lightOff;//Task initial state.
task4.period = 300;//Task Period.
task4.elapsedTime = task4.period;//Task current elapsed time.
task4.TickFct = &LightsOut;//Function pointer for the tick.
// Task 5 (toggleLED0SM)
task5.state = NewCode_init;//Task initial state.
task5.period = 1;//Task Period.
task5.elapsedTime = task5.period;//Task current elapsed time.
task5.TickFct = &NewCode;//Function pointer for the tick.
// Task 6 (toggleLED1SM)
task6.state = change_init;//Task initial state.
task6.period = 1;//Task Period.
task6.elapsedTime = task6.period;//Task current elapsed time.
task6.TickFct = &ChangeCode;//Function pointer for the tick.
// Task 4 (displaySM)
/*task4.state = start;//Task initial state.
task4.period = 10;//Task Period.
task4.elapsedTime = task4.period; // Task current elasped time.
task4.TickFct = &displaySMTick; // Function pointer for the tick.
*/
unsigned short i;
unsigned long GCD = tasks[0]->period;
	for ( i = 1; i < numTasks; i++ ) { 
   	 	GCD = findGCD(GCD,tasks[i]->period);
	}
// Set the timer and turn it on

PWM_on();

TimerSet(GCD);
TimerOn();
LCD_init();
LCD_DisplayString(25,data);
/*LCD_DisplayString(21,"0");
LCD_DisplayString(17,"day");*/


	initUSART(1);

	// PORTB = USART_Receive(1);
	/*USART_Send(0x01,1);*/


//eeprom_write_byte(0,days);

count=eeprom_read_byte(0);
codes[0]=eeprom_read_byte(1);
codes[1]=eeprom_read_byte(2);
codes[2]=eeprom_read_byte(3);

LCD_Cursor(18);
LCD_WriteData('0'+count);

int state;
 // Scheduler for-loop iterator
while(1) { 
tick();
//days=eeprom_read_byte(0);
   for ( i = 0; i < numTasks; i++ ) { // Scheduler code
        if ( tasks[i]->elapsedTime == tasks[i]->period ) { // Task is ready to tick
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state); // Set next state 
            tasks[i]->elapsedTime = 0; // Reset the elapsed time for next tick.
        }
        tasks[i]->elapsedTime += GCD;
    }

	//state=keypad(state);
	//PORTB=0xFF;
   while(!TimerFlag);
        TimerFlag = 0;
}
return 0; // Error: Program should not exit!
}




