
#define F_CPU 8000000UL			/* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>			/* Include Delay header file */

#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD			/* Define LCD data port */
#define RS PD0				/* Define Register Select pin */
#define EN PD1 				/* Define Enable signal pin */
 

void LCD_WriteCommand( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}


void LCD_WriteData( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_init (void)			/* LCD Initialize function */
{
	LCD_Dir = 0xFF;			/* Make LCD port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_WriteCommand(0x02);		/* send for 4 bit initialization of LCD  */
	LCD_WriteCommand(0x28);              /* 2 line, 5*7 matrix in 4-bit mode */
	LCD_WriteCommand(0x0c);              /* Display on cursor off*/
	LCD_WriteCommand(0x06);              /* Increment cursor (shift cursor to right)*/
	LCD_WriteCommand(0x01);              /* Clear display screen*/
	_delay_ms(2);
}


void LCD_DisplayString(unsigned char column,char *str)		/* Send string to LCD function */
{//LCD_ClearScreen();
unsigned char c = column;
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{LCD_Cursor(c++);
		LCD_WriteData (str[i]);
	}
}

void  LCD_Cursor(char column/*,/* char pos, char *str*/)	/* Send string to LCD with xy position */
{

  if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
  //if (row == 0 && pos<16)
  //	LCD_WriteCommand((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
  //	else if (row == 1 && pos<16)
  //	LCD_WriteCommand((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
  	LCD_DisplayString;		/* Call LCD string function */
  }
}

void LCD_ClearScreen()
{
	LCD_WriteCommand (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_WriteCommand (0x80);		/* Cursor at home position */
}
/*
int main()
{

	LCD_init();			/* Initialization of LCD*/

/*	LCD_DisplayString("ElectronicWINGS");	/* Write string on 1st line of LCD*/
/*	LCD_WriteCommand(0xC1);		/* Go to 2nd line*/
/*	LCD_DisplayString("Hello World");	/* Write string on 2nd line*/
/*	while(1);
}*/
