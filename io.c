#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTC		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTD	// port connected to pins 4 and 6 of LCD disp.
#define RS 6			// pin number of uC connected to pin 4 of LCD disp.
#define E 7			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
	LCD_WriteCommand(0x01);
}

void LCD_init(void) {

	//wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);
}

void LCD_WriteCommand (unsigned char Command) {
	CLR_BIT(CONTROL_BUS,RS);
	DATA_BUS = Command;
	SET_BIT(CONTROL_BUS,E);
	asm("nop");
	CLR_BIT(CONTROL_BUS,E);
	delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
	SET_BIT(CONTROL_BUS,RS);
	DATA_BUS = Data;
	SET_BIT(CONTROL_BUS,E);
	asm("nop");
	CLR_BIT(CONTROL_BUS,E);
	delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
	LCD_ClearScreen();
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}

void LCD_Display_No_Clear(unsigned char column, const unsigned char* string) {		//needed to display the game without clearing
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}


void LCD_Cursor(unsigned char column) {	//edited for 20x4 LCD
	if ( column < 21 ) { 
		LCD_WriteCommand(0x80 + column - 1);
		} 
	else if (column < 41) {
		LCD_WriteCommand(0xC0 + column - 21);
	}
	else if (column < 61) {
		LCD_WriteCommand(0x94 + column - 41);
	}
	else {
		LCD_WriteCommand(0xD4 + column - 61);
	}
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
	int i,j;
	for(i=0;i<miliSec;i++)
	for(j=0;j<775;j++)
	{
		asm("nop");
	}
}

void LCD_Custom(unsigned char loc, unsigned char *ptr) //reference (very simple and helpful): https://openlabpro.com/guide/custom-character-lcd-pic/
{
	unsigned char i;				
	if(loc < 8)
	{
		LCD_WriteCommand(0x40 + (loc * 8));
		for(i = 0; i < 8; i++)
		{
			LCD_WriteData(ptr[i]);
		}
	}
	
}


unsigned char * LCD_Score_Conversion(unsigned int totalScore, unsigned char * arr, unsigned char size) {  //atoi function needed to convert score
	arr[size - 1] = '\0';
	for (short i = size - 2; i >= 0; i--) {
		arr[i] = totalScore % 10 + '0';
		totalScore /= 10;
	}
	return arr;
}