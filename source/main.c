/* Author : Rizelle Vinluan
 * Description: Dodge! project for CS120B. 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#include "scheduler.h"
#include "timer.h"
#include "pwm.h"
// The top four header files are given from CS120B labs, with some exceptions in io.h
#include "joystick.h"

unsigned char i = 0;
//================================================================== LCD Display functions ==============================================================
unsigned char player[] = { 0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A };// custom player
unsigned char wall[] = { 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07 };	// custom obstacle
unsigned  int score = 0;													// score counter
unsigned char wall_symbol = '<';
unsigned char wall_position;												// wall's position
unsigned char currentPosition = 61;											// player's position

void start_LCD() {		// This is the start display
	LCD_ClearScreen();
	LCD_Display_No_Clear(1, (const unsigned char*)("-- Press Joystick --"));
	LCD_Display_No_Clear(21, (const unsigned char*)("----- To Start -----"));
	LCD_Display_No_Clear(41, (const unsigned char*)("--- Get Ready To ---"));
	LCD_Display_No_Clear(61, (const unsigned char*) ("====== Dodge! ======"));
}
void game_LCD() {		// This is the game display
	LCD_ClearScreen();
	LCD_Display_No_Clear(32, (const unsigned char *)(" "));				// Needed to refresh the screen
	LCD_Cursor(currentPosition);
	LCD_WriteData(1);
	LCD_Cursor(wall_position);
	LCD_WriteData(2);
}

// The next four functions create the walls
void spawnTopWall() {
	LCD_ClearScreen();
	wall_position = 20;
}
void spawnBottomWall() {
	LCD_ClearScreen();
	wall_position = 80;
}
void spawnUpperWall() {
	LCD_ClearScreen();
	wall_position = 40;
}
void spawnLowerWall() {
	LCD_ClearScreen();
	wall_position = 60;
}

unsigned char score_max[6];
void end_LCD() {		// This is the game over display
	LCD_ClearScreen();
	LCD_Display_No_Clear(1, (const unsigned char*)("You are dead. Press the button to reset."));
	LCD_Display_No_Clear(41, (const unsigned char*)("Score: "));
	LCD_Display_No_Clear(47, LCD_Score_Conversion(score, score_max, 6));
}

//====================================================================== LCD States ======================================================================
unsigned long gameTime = 0;
unsigned long moveTime = 20; // Length of LCD
unsigned long moveFactor = 5;
unsigned char wallDone;
#define tmpA (~PINA & 0x0C)
enum LCD_States {Display_init, Display_again, GoToStart_LCD, HoldStart_LCD, StartGame_LCD, MoveWall, MoveWall2, Update2, ResetWall2, ThirdWall, 
	SecondWall, Update, ResetWall, MoveWall3, Update3, ResetWall3, FourthWall, MoveWall4, Update4, ResetWall4, GoToReset_LCD, HoldReset_LCD, 
	Reset_LCD, Dead_LCD, Dead2_LCD } LCD_STATE;
void LCD_Tick() {
	switch(LCD_STATE) {
		case Display_init:
			currentPosition = 61;
			score = 0;
			if (tmpA == 0x00) {
				LCD_STATE = Display_again;
			}
			else if (tmpA == 0x04) {
				LCD_STATE = GoToStart_LCD;
			}
			else if (tmpA == 0x08) {
				LCD_STATE = GoToReset_LCD;
			}
			break;
		case Display_again:
			currentPosition = 61;
			score = 0;
			if (tmpA == 0x00) {
				LCD_STATE = Display_init;
			}
			else if (tmpA == 0x04) {
				LCD_STATE = GoToStart_LCD;
			}
			else if (tmpA == 0x08) {
				LCD_STATE = GoToReset_LCD;
			}			
		case GoToStart_LCD:
			if (tmpA == 0x00) {
				LCD_STATE = GoToStart_LCD;
			}
			else if (tmpA == 0x04) {
				LCD_STATE = HoldStart_LCD;
			}
			break;
			
			case HoldStart_LCD:
			if (tmpA == 0x00) {
				LCD_STATE = StartGame_LCD;
			}
			else if (tmpA == 0x04) {
				LCD_STATE = HoldStart_LCD;
			}
			break;
			case StartGame_LCD:
				if (tmpA == 0x00 && (gameTime < moveTime)) {
					LCD_STATE = StartGame_LCD;
				}
				else if (tmpA == 0x00 && (gameTime == moveTime)) {
					LCD_STATE = MoveWall;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				break;
			case MoveWall:
				if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				LCD_STATE = Update;
				break;
			case Update:
				LCD_STATE = MoveWall;
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				if (wallDone == 1) {
					LCD_STATE = ResetWall;
				}
				if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				break;
			case ResetWall:
				LCD_STATE = SecondWall;
				break;			
			case SecondWall:
				if (tmpA == 0x00 && (gameTime < moveTime)) {
					LCD_STATE = SecondWall;
				}
				else if (tmpA == 0x00 && (gameTime == moveTime)) {
					LCD_STATE = MoveWall2;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				break;		
			case MoveWall2:
				LCD_STATE = Update2;
				break;		
			case Update2:
				LCD_STATE = MoveWall2;
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				if (wallDone == 1) {
					LCD_STATE = ResetWall2;
				}
				if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				break;
			case ResetWall2:
					LCD_STATE = ThirdWall;
					break;
			case ThirdWall:
				if (tmpA == 0x00 && (gameTime < moveTime)) {
					LCD_STATE = ThirdWall;
				}
				else if (tmpA == 0x00 && (gameTime == moveTime)) {
					LCD_STATE = MoveWall3;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				break;
			case MoveWall3:
				LCD_STATE = Update3;
				break;
			case Update3:
				LCD_STATE = MoveWall3;
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				if (wallDone == 1) {
					LCD_STATE = ResetWall3;
				}
				if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				break;
			case ResetWall3:
				LCD_STATE = FourthWall;
				break;
			case FourthWall:
				if (tmpA == 0x00 && (gameTime < moveTime)) {
					LCD_STATE = FourthWall;
				}
				else if (tmpA == 0x00 && (gameTime == moveTime)) {
					LCD_STATE = MoveWall4;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				break;
			case MoveWall4:
				LCD_STATE = Update4;
				break;
			case Update4:
				LCD_STATE = MoveWall4;
				if(currentPosition == wall_position) {
					LCD_STATE = Dead_LCD;
				}
				if (wallDone == 1) {
					LCD_STATE = ResetWall4;
				}
				if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				break;
			case ResetWall4:
				LCD_STATE = StartGame_LCD;
				break;
			case GoToReset_LCD:
				if (tmpA == 0x00) {
					LCD_STATE = GoToReset_LCD;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = HoldReset_LCD;
				}
				break;
			case HoldReset_LCD:
				if (tmpA == 0x00) {
					LCD_STATE = Reset_LCD;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = HoldReset_LCD;
				}
				break;
			case Reset_LCD:
				if (tmpA == 0x00) {
					LCD_STATE = Display_again;
				}
				break;
			case Dead_LCD:
				currentPosition = 61;
				LCD_STATE = Dead2_LCD;
				break;
			case Dead2_LCD:
				if (tmpA == 0x00) {
					LCD_STATE = Dead2_LCD;
				}
				else if (tmpA == 0x08) {
					LCD_STATE = GoToReset_LCD;
				}
				break;
	}	
	
		switch(LCD_STATE) {
			case Display_init:
				start_LCD();
				PORTB = 0x00;
				break;
			case Display_again:
				start_LCD();
				PORTB = 0x00;
				score = 0;
				break;
			case GoToStart_LCD:
				break;
			case HoldStart_LCD:
				break;
			case StartGame_LCD:
				PORTD = 0x10;
				fetchStick();
				LCD_Cursor(currentPosition);
				spawnBottomWall();
				score++;
				gameTime++;
				game_LCD();
				break;
			case MoveWall:
				if (gameTime % gameTime == 0 && moveTime > 1) {
					moveTime--;
				}
				if (wall_position > 1 && wall_position <= 20) {
					--wall_position;
				}
				else if (wall_position > 21 && wall_position <= 40) {
					--wall_position;
				}
				else if (wall_position > 41 && wall_position <= 60) {
					--wall_position;
				}
				else if (wall_position > 61 && wall_position <= 80) {
					--wall_position;
				}
				else {
					wall_position = 0;
					wallDone = 1;
				}
				score++;
				break;
			case Update:
				fetchStick();
				LCD_Cursor(currentPosition);
				score++;
				gameTime = 0;
				game_LCD();
				break;
			case ResetWall:
				wallDone = 0;
				score++;
				break;
			case SecondWall:
				PORTD = 0x10;
				fetchStick();
				LCD_Cursor(currentPosition);
				spawnLowerWall();
				score++;
				gameTime++;
				game_LCD();
				break;
			case MoveWall2:
				if (gameTime % gameTime == 0 && moveTime > 1) {
					moveTime--;
				}
				if (wall_position > 1 && wall_position <= 20) {
					--wall_position;
				}
				else if (wall_position > 21 && wall_position <= 40) {
					--wall_position;
				}
				else if (wall_position > 41 && wall_position <= 60) {
					--wall_position;
				}
				else if (wall_position > 61 && wall_position <= 80) {
					--wall_position;
				}
				else {
					wall_position = 0;
					wallDone = 1;
				}
				score++;
				break;
			case Update2:
				fetchStick();
				LCD_Cursor(currentPosition);
				score++;
				gameTime = 0;
				game_LCD();
				break;
			case ResetWall2:
				wallDone = 0;
				score++;
				break;
			case ThirdWall:
				PORTD = 0x10;
				fetchStick();
				LCD_Cursor(currentPosition);
				spawnTopWall();
				score++;
				gameTime++;
				game_LCD();
				break;
			case MoveWall3:
				if (gameTime % gameTime == 0 && moveTime > 1) {
					moveTime--;
				}
				if (wall_position > 1 && wall_position <= 20) {
					--wall_position;
				}
				else if (wall_position > 21 && wall_position <= 40) {
					--wall_position;
				}
				else if (wall_position > 41 && wall_position <= 60) {
					--wall_position;
				}
				else if (wall_position > 61 && wall_position <= 80) {
					--wall_position;
				}
				else {
					wall_position = 0;
					wallDone = 1;
				}
				score++;
				break;
			case Update3:
				fetchStick();
				LCD_Cursor(currentPosition);
				score++;
				gameTime = 0;
				game_LCD();
				break;
			case ResetWall3:
				wallDone = 0;
				score++;
				break;
			case FourthWall:
				PORTD = 0x10;
				fetchStick();
				LCD_Cursor(currentPosition);
				spawnUpperWall();
				score++;
				gameTime++;
				game_LCD();
				break;
			case MoveWall4:
				if (gameTime % gameTime == 0 && moveTime > 1) {
					moveTime--;
				}
				if (wall_position > 1 && wall_position <= 20) {
					--wall_position;
				}
				else if (wall_position > 21 && wall_position <= 40) {
					--wall_position;
				}
				else if (wall_position > 41 && wall_position <= 60) {
					--wall_position;
				}
				else if (wall_position > 61 && wall_position <= 80) {
					--wall_position;
				}
				else {
					wall_position = 0;
					wallDone = 1;
				}
				score++;
				break;
			case Update4:
				fetchStick();
				LCD_Cursor(currentPosition);
				score++;
				gameTime = 0;
				game_LCD();
				break;
			case ResetWall4:
				wallDone = 0;
				score++;
				break;
			case GoToReset_LCD:
				break;
			case HoldReset_LCD:
				break;
			case Reset_LCD:
				break;
			case Dead_LCD:
				end_LCD();
				score = score * 3;
				break;
			case Dead2_LCD:
				end_LCD();
				break;
		}
	
}

//=============================================================== Joystick and Speaker (PORTB0) States ===================================================
// This includes a speaker that outputs sound when the player moves
// It's important to note that PWM is connected to PORTB6, so I need 2 states for each movement to create a sound wave
enum Joy_States {j_init, update, up, up2, down, down2, left, left2, right, right2 } J_State;
	void J_tick() {
		fetchStick();
		switch(J_State) {
			case j_init:
				currentPosition = 61;
				J_State = update;
				break;
			case update:
				if (coords[0] < 200 && (currentPosition >= 21 && currentPosition <= 80)) {
					J_State = up;
					currentPosition = currentPosition - 20;
				}
				else if (coords[0] > 800 && (currentPosition >= 1 && currentPosition <= 60)) {
					J_State = down;
					currentPosition = currentPosition + 20;
				}
				else if (coords[1] < 200 && (currentPosition != 20 && currentPosition != 40 && currentPosition != 60 && currentPosition != 80)) {
					J_State = right;
					currentPosition = currentPosition + 1;
				}
				else if (coords[1] > 800 && (currentPosition != 1 && currentPosition != 21 && currentPosition != 41 && currentPosition != 61)) {
					J_State = left;
					currentPosition = currentPosition - 1;
				}
				else {
					J_State = update;
				}
				break;
			case up:
				J_State = up2;
				break;
			case up2:
				J_State = update;
				break;
			case down:
				J_State = down2;
				break;
			case down2:
				J_State = update;
				break;
			case left:
				J_State = left2;
				break;
			case left2:
				J_State = update;
				break;
			case right:
				J_State = right2;
				break;
			case right2:
				J_State = update;
				break;
			default:
				J_State = j_init;
				break;
			}
		
		switch(J_State) {
			case j_init:
				LCD_Cursor(currentPosition);
				break;
			case update:
				LCD_Cursor(currentPosition);
				break;
			case up:
				PORTD = 0x01;
				PORTB = 0x01;
				LCD_Cursor(currentPosition);
				break;
			case down:
				PORTD = 0x02;
				PORTB = 0x01;
				LCD_Cursor(currentPosition);
				break;
			case left:
				PORTD = 0x04;
				PORTB = 0x01;
				LCD_Cursor(currentPosition);
				break;
			case right:
				PORTD = 0x08;
				PORTB = 0x01;
				LCD_Cursor(currentPosition);
				break;
			case up2:
				PORTD = 0x01;
				PORTB = 0x00;
				LCD_Cursor(currentPosition);
				break;
			case down2:
				PORTD = 0x02;
				PORTB = 0x00;
				LCD_Cursor(currentPosition);
				break;
			case left2:
				PORTD = 0x04;
				PORTB = 0x00;
				LCD_Cursor(currentPosition);
				break;
			case right2:
				PORTD = 0x08;
				PORTB = 0x00;
				LCD_Cursor(currentPosition);
				break;
			default:
				break;
		}
	}
	
//================================================================ Speaker (PORTB6) States ===============================================================
// This speaker outputs the BGM 
double sound[30] = {100.00, 30.00, 100.00, 30.00, 100.00, 30.00, 100.00, 30.00, 40.00, 80.00, 100.00, 30.00, 100.00, 30.00, 100.00, 30.00, 100.00, 30.00, 
	40.00, 80.00, 110.00, 30.00, 110.00, 30.00, 110.00, 30.00, 110.00, 30.00, 40.00, 80.00};

double btwn[29] = {65, 95, 65, 95, 65, 95, 50, 50, 50, 50, 65, 95, 65, 95, 65, 95, 50, 50, 50, 50, 65, 125, 65, 125, 65, 125, 50, 50, 50, 50 };
unsigned long  p, s, b, d = 0;
enum Speaker_States {S_Init, Hold, GoToPlay, Play, DelayNote, PlayAgain} S_STATE;
	void S_tick() {
		switch(S_STATE) {
			case S_Init:
				S_STATE = Hold;
				break;
			case Hold:
				S_STATE = GoToPlay;
				break;
			case GoToPlay:
				S_STATE = Play;
				break;
			case Play:
				if (p <= sound[s]) {
					S_STATE	= DelayNote;
				}
				else if (p > sound[s]) {
					S_STATE = PlayAgain;
				}
				break;
			case DelayNote:
				if (d <= btwn[b]) {
					S_STATE = Play;
				}
				else if (d > btwn[d]){
					S_STATE = PlayAgain;
				}
				break;
			case PlayAgain:
				S_STATE = Play;
				break;
		} 
		
		switch(S_STATE) {
			case S_Init:
				break;
			case Hold:
				p = 0; s = 0; d = 0; b = 0;
				break;
			case GoToPlay:
				PWM_on();
				break;
			case Play:
				if (s <= 37) {
					set_PWM(sound[s]);
					++s;
				}
				if (p < 39) {
					++p;
				}
				break;
			case DelayNote:
				if (b < 37) {
					TimerSet(btwn[b]);
					set_PWM(0);
					++b; ++d;
				}
				break;
			case PlayAgain:
				p = 0; s = 0; d = 0; b = 0;
				break;
		}
		
	}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	unsigned char V = 3;
	task tasks [V];
	const unsigned short tasksNum = V;

	// Speaker task
	tasks[i].state = S_Init;
	tasks[i].period = 15;
	tasks[i].elapsedTime = 15;
	tasks[i].TickFct = &S_tick;

	++i;

	// Joystick task
	tasks[i].state = j_init;
	tasks[i].period = 2;
	tasks[i].elapsedTime = 2;
	tasks[i].TickFct = &J_tick;	

	++i;

	// LCD task
	tasks[i].state = Display_init;
	tasks[i].period = 8;
	tasks[i].elapsedTime = 8;
	tasks[i].TickFct = &LCD_Tick;
	
	TimerSet(1);
	TimerOn();
	PWM_on();
	LCD_init();
	ADC_init();
	LCD_Custom(1, player);
	LCD_Custom(2, wall);

	unsigned short i;
	while(1) {
		for ( i = 0; i < tasksNum; i++ ) {
			if ( tasks[i].elapsedTime == tasks[i].period ) {
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

}
