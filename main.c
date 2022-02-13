// -----------------------------------------------------------------------------
// C-Skeleton to be used with HAM Library from www.ngine.de
// -----------------------------------------------------------------------------
#include "numbers.h"
#include "gba.h"

#define KEYCNT *(volatile u16*)0x4000132
#define X_START 50 //x start position 
#define Y_START 72 //y start position
//int time = 0;
int state = 0; //level indicator
int p_x = X_START; //x position of character
int p_y = Y_START; //y position of character
int v = 2 ; //speed of character per frame update


void drawSprite(int numb, int N, int x, int y)
{
    // Gift function: displays sprite number numb on screen at position (x,y), as sprite object N
    *(unsigned short *)(0x7000000 + 8*N) = y | 0x2000;
    *(unsigned short *)(0x7000002 + 8*N) = x;
    *(unsigned short *)(0x7000004 + 8*N) = numb*2;
}

void clearSprite(int N){//removes sprite N
	*(unsigned short *)(0x7000000 + 8*N) = 0;
    *(unsigned short *)(0x7000002 + 8*N) = 0;
    *(unsigned short *)(0x7000004 + 8*N) = 0;
}

/*void drawPlayer(int N, int x, int y){ // C representation of the drawPlayer function
	*(unsigned short *)(0x7000000 + 8*N) = y | 0x2000;
    *(unsigned short *)(0x7000002 + 8*N) = x | 0x4000;
    *(unsigned short *)(0x7000004 + 8*N) = 12*2;
}
*/
extern void drawPlayer(u32 N, u32 x, u32 y);//C representation commented above

int xpos(int in){ // determines x position for the next update
	int temp = p_x + (in*v);
	if(temp > 112 && temp <= 120 && (p_y < 140 || !(state == 2 || state == 3)) && (state >=2 && state <= 5)) {//handles first vertical wall
		return 112;
	}
	if(temp < 151 && temp > 120 && (p_y < 140  || !(state== 2 || state == 3)) && (state >= 2 && state <= 5)) {
		return 151;
	}
	if(temp > 150 && temp <=158 && p_y >= 140 && state == 3){//handles second vertical wall at the bottom of level 3
		return 150;
	}
	if(temp < 170 && temp >158 && p_y >=140 && state == 3) {
		return 170;
	}
	if(temp >180 && temp <=190 && p_y <=60 && state == 5) {//handles vertical wall at the top right of level 5
		return 180;
	}
	if(temp < 201 && temp >190 && p_y <=60 && state == 5) {
		return 201;
	}
	if((state == 4 || state ==5)&& (p_y<=36 || p_y > 108)) {//handles phasing through stage boundary at level 4 and 5
		if(temp<0){
			return SCREEN_WIDTH+temp;
		}
		return temp % SCREEN_WIDTH;
	}
	if(temp > SCREEN_WIDTH-16){//handles collision with stage boundary at level 1-3
		return SCREEN_WIDTH-16;
	} else if( temp < 0){
		return 0;
	}
	return temp;
}

int ypos(int in){ //determines y position for the next update
	int temp = p_y + (in*v);
	if(p_x < 151 && p_x > 112 && temp <140 && temp > 124&& (state == 2 || state == 3)) {//collision with bottom of vertical wall at level 2 and 3
		return 140;
	}
	if(p_x >= 151 && p_x <170 && temp >124 && state == 3) {//collision with top of the second wall at level 3
		return 124;
	} 
	if(p_x>=150 && temp<=108 && temp>100 && state == 5) {//horizontal wall at bottom right of level 5
		return 109;
	}
	if(p_x>=150 && temp>92 && temp<=100 && state ==5) {
		return 92;
	}
	if(p_x>180 && temp<=60 && temp>=50 && state == 5) {//horizontal wall at top right of level 5
		return 61;
	}
	if(p_x>180 && temp>36 && temp<50 && state == 5) {
		return 36;
	}
	if((state == 3 && !(p_x>112 && p_x<150)) || (state == 5 && !(p_x>112 && p_x<150) && !(p_x>180 && p_x<201))){//phasing through stage wall at level 3 and 5
		if(temp < 0){
			return SCREEN_HEIGHT+temp;
		}
		return temp % SCREEN_HEIGHT;
	}
	if(temp > SCREEN_HEIGHT-16){//stage boundary handling
		return SCREEN_HEIGHT-16;
	} else if( temp < 0){
		return 0;
	}
	return temp;
}



void Handler(void)//handler function
{
    REG_IME = 0x00; // Stop all other interrupt handling, while we handle this current one
	u16 buttons = (KEY_MASK & (~REG_KEYS));//reads buttons
	u16 Int_Flag = REG_IF;
	
	if(	((REG_IF & INT_BUTTON) == INT_BUTTON) && state == 0){//handles transition from state 0/main menu to level 1
		state++;//increses state
		REG_IE = INT_VBLANK;//disables button interupt 
		drawSprite(state, 0, 10, 10);//draws objects for level 1
		drawSprite(11, 1, 0, 52);//draws blocks
		drawSprite(11, 2, 0, 100);
		drawSprite(11, 3, 232, 52);
		drawSprite(11, 4, 232, 100);
		drawPlayer(5, p_x,p_y);//draws player at start position
	}
	
	if( (REG_IF & INT_VBLANK) == INT_VBLANK && state != 0) {//updates every frame for vsync

		if((buttons & KEY_RIGHT) == KEY_RIGHT) {//right button press position update
			p_x = xpos(1);
		} else if ((buttons & KEY_LEFT) == KEY_LEFT) {//left button press position update
			p_x = xpos(-1);
		}
		if((buttons & KEY_UP) == KEY_UP) {//up button press position update
			p_y = ypos(-1);
		} else if ((buttons & KEY_DOWN) == KEY_DOWN) {//down button position update
			p_y = ypos(1);
		}
		if((p_x == SCREEN_WIDTH -16) && (p_y>=52) && (p_y <= 92)){//handles level transition
			state++;
			if(state != 6) {
				drawSprite(state, 0, 10, 10);//updates level indicator
			}
			p_x = X_START;//brings player to start position when updating level
			p_y = Y_START;
		}
		if (state!=6){
			drawPlayer(5, p_x,p_y);//updates player position for this frame
		}
		if (state == 6){//handles returning to main menu
			state = 0;
			clearSprite(1);//removes level sprites
			clearSprite(2);
			clearSprite(3);
			clearSprite(4);
			clearSprite(5);
			REG_IE = INT_BUTTON | INT_VBLANK;//reinitializes button interrupt 
			drawSprite(10, 0, 116, 76);//draws main menu
		}
		
			
	}

		
		
    /*if ((REG_IF & INT_TIMER0) == INT_TIMER0) // TODO: replace XXX with the specific interrupt you are handling
    {
        
      drawSprite(time % 10, 0, 120, 76);
      drawSprite(time / 10, 1, 112, 76);
	  time++;
	  time = time % 100;		  
    }
	*/
    
    REG_IF = Int_Flag; // Update interrupt table, to confirm we have handled this interrupt
    
    REG_IME = 0x01;  // Re-enable interrupt handling
}


// -----------------------------------------------------------------------------
// Project Entry Point
// -----------------------------------------------------------------------------
int main(void)
{
	int i;
	
    // Set Mode 2
    *(unsigned short *) 0x4000000 = 0x40 | 0x2 | 0x1000;

    // Fill SpritePal
    *(unsigned short *) 0x5000200 = 0;
    *(unsigned short *) 0x5000202 = RGB(10,27,31);
	*(unsigned short *) 0x5000204 = RGB(31,31,31);

    // Fill SpriteData
    for (i = 0; i < 16*8*8/2; i++)
        spriteData[i] = (numbers[i*2+1] << 8) + numbers[i*2];

    // Set Handler Function for interrupts and enable selected interrupts
    REG_INT = (int)&Handler;
    REG_IE = INT_BUTTON | INT_VBLANK; //enables interrupt 
	KEYCNT = KEY_RIGHT | 0x4000; //enables right key interrupt 
	REG_DISPSTAT |=0x8;
    REG_IME = 0x1;			// Enable interrupt handling
	
	drawSprite(10, 0, 116, 76); // draw start menu sprite

    // Set Timer Mode (fill that section and replace TMX with selected timer number)
  //  REG_TM0D =	-16384;		// TODO: complete this line to set timer initial value
  //  REG_TM0CNT =  TIMER_ENABLE | TIMER_INTERRUPTS |	TIMER_FREQUENCY_1024;	// TODO: complete this line to set timer frequency and enable timer

    while(1);

	return 0;
}

