#ifndef __io_h__
#define __io_h__

void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void LCD_Display_No_Clear(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);
void LCD_Custom(unsigned char loc, unsigned char *ptr);
unsigned char * LCD_Score_Conversion(unsigned int totalScore, unsigned char * arr, unsigned char size);
void LCD_Display_Custom(unsigned char column, const unsigned char* string);
#endif
