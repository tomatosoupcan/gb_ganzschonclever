#include <gb/gb.h>
#include <gb/cgb.h>
#include "tiles.c"
#include "map.c"
#include "map2.c"
#include "map3.c"
#include "map4.c"
#include "rand.h"
#include "sprite1.c"

UINT8 cur[2];
UINT8 rcur[2];
UINT8 temp;
UINT8 temp2;
UINT8 temp3;
UINT8 temp4;
UINT8 temp5;
UINT8 temp6;
UINT8 temp7;
UINT8 temp8;
UINT8 temp9;
UINT8 curDel;
UINT8 infoTrack[3];
UINT8 selectmode = 0;
unsigned char tempArray[];
unsigned int seed;

unsigned char diceArray[] = 
{
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x48
};

unsigned char diceValues[] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char usedDiceArray[] = 
{
	0x48,0x48,0x48
};

unsigned char usedDiceTiles[] =
{
	0x28,0x3C,0x50
};

unsigned char usedDicePalettes[] = 
{
	0x00,0x00,0x00
};

unsigned char usedDiceValues[] =
{
	0xFF,0xFF,0xFF
};

unsigned char dicePalettes[] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

unsigned char diceTiles[] =
{
	0x25,0x39,0x4D,0x61,0x75,0x89
};

unsigned char diceAvaPalettes[] =
{
	0x01,0x02,0x03,0x04,0x05,0x00
};

const UWORD bkg_palette[] =
{
    RGB(29,30,23), RGB(7,23,8),  RGB(4,14,6), RGB(0,7,5),
	RGB(2,4,31),   RGB(7,23,8),  RGB(28,5,0), RGB(0,7,5),
	RGB(25,17,0),  RGB(7,23,8),  RGB(4,14,6), RGB(0,7,5),
	RGB(20,3,31),  RGB(7,23,8),  RGB(4,14,6), RGB(0,7,5),
	RGB(13,25,0),  RGB(7,23,8),  RGB(4,14,6), RGB(0,7,5),
	RGB(31,31,5),  RGB(7,23,8),  RGB(4,14,6), RGB(0,7,5),
	RGB(0,0,0),    RGB(29,30,23),RGB(30,3,31),RGB(31,31,5),
	RGB(29,30,23), RGB(13,25,0), RGB(25,17,0),RGB(2,4,31)
};

UINT8 y_array[] =
{
	3,6,5,0,
	2,1,0,5,
	1,0,2,4,
	0,3,4,6
};

UINT8 b_array[] =
{
	0,2,3,4,
	5,6,7,8,
	9,10,11,12
};

UINT8 g_array[] =
{
	0,0,0,0,0,0,0,0,0,0,0
};

UINT8 o_array[] =
{
	0,0,0,0,0,0,0,0,0,0,0
};

UINT8 p_array[] =
{
	0,0,0,0,0,0,0,0,0,0,0
};



void init();
void checkInput();
void updateSwitches();
void rollDice();
void shuffle(unsigned char *array, UINT8 n);
void main_int();
void pickDie();
void newRound();
void updateBG();

void main() {
	init();

	while(!joypad()){seed++; if(seed>=255)seed=1;} //generate the seed based on when you hit a button
	initrand(seed);
	rollDice();
	updateBG();
	while(1) {
		checkInput();				// Check for user input
		updateSwitches();			// Turn on screen
		wait_vbl_done();			// Wait until VBLANK to avoid corrupting memory
	}
	
}

void init() {
	curDel = 1; //set the cursor lock
	set_bkg_palette(0, 8, bkg_palette); //load the pallets
	VBK_REG = 1; //switch to color map
	set_bkg_tiles(0,0,20,18,cgbmap2); //load color map
	VBK_REG = 0; //switch to regular map
	DISPLAY_ON;						// Turn on the display
	set_bkg_data(0, 127, TileLabel);		// Load tiles into background memory
	
	set_bkg_tiles(0,0,20,18,map2); //load starting screen

	set_sprite_data(0,1,sprites);
	set_sprite_tile(0,0);
	move_sprite(0, 16, 24);
	set_sprite_palette(0,8, bkg_palette);
	set_sprite_prop(0,1);
	
	rcur[0] = 16;
	rcur[1] = 24;
	cur[0] = 0; //set the cursor positions
	cur[1] = 0;
	infoTrack[0] = 1; //set the current round to 1
	infoTrack[1] = 0; //set the number of +1 to 0
	infoTrack[2] = 1; //set the number of rerolls to 0
	/*rollDice();*/
}

void updateSwitches() { //show just the background for now, will need a sprite and window eventually
	
	HIDE_WIN;
	SHOW_BKG;
	
}

void clearCursor() { //Blanks the current position of the cursor, situational on what tile is below it sort of
	temp = (cur[0]+16)+(20*(cur[1]+1));
	if (cur[0] == 2) {
		temp2 = 0x47;
	}
	else {
		temp2 = 0x15;
	}
	map1[temp] = temp2;
}

void moveCursor() { //This is used to handle moving in the dice selector, and nothing more right now
	temp = (cur[0]+16)+(20*(cur[1]+1));
	if (cur[0] == 2) {
		temp2 = 0x49;
	}
	else {
		temp2 = 0x4A;
	}
	map1[temp] = temp2;
	set_bkg_tiles(0,0,20,18,map1);
	curDel = 1;
}

void checkInput() {
	//Don't allow the joypad to be used again until you let go of a button
	if (!joypad()) {
		curDel = 0;
	}
	if (selectmode == 0){
		if (curDel == 0) {
			if (joypad() & J_DOWN) { //All of these move the cursor in the dice selector, will need to break this down further once more movement and sprites come into play
				clearCursor();
				if (cur[1] < 5) {
					cur[1]++;
				}
				else {
					cur[1] = 0;
				}
				if (cur[1] == 3 && cur[0] == 2) {
					cur[1]++;
				}
				moveCursor();
			}
			if (joypad() & J_UP) {
				clearCursor();
				if (cur[1] > 0) {
					cur[1]--;
				}
				else {
					cur[1] = 5;
				}
				if (cur[1] == 3 && cur[0] == 2) {
					cur[1]--;
				}
				moveCursor();
			}
			if (joypad() & J_RIGHT) {
				if (cur[1] == 3) {
					return;
				}
				clearCursor();
				if (cur[0] == 2) {
					cur[0] = 0;
				}
				else {
					cur[0] = 2;
				}
				moveCursor();
			}
			if (joypad() & J_LEFT) {
				if (cur[1] == 3) {
					return;
				}
				clearCursor();
				if (cur[0] == 2) {
					cur[0] = 0;
				}
				else {
					cur[0] = 2;
				}
				moveCursor();
			}
			temp = (cur[0]+17)+(20*(cur[1]+1));
			if (joypad() & J_A) {
				if (cur[0] == 2 && cur[1] == 5) {
					rollDice();
				}
				else if (cur[0] == 0 && map1[temp] != 0x48){
					SHOW_SPRITES;
					/*pickDie();*/
					selectmode = 1;
				}
				curDel = 1;
			}
			if (joypad() & J_START) {
				newRound();
				curDel = 1;
			}
		}
	}
	else if (selectmode == 1){
		if (curDel == 0) {
			//Begin Yellow Handling
			if(joypad() & J_RIGHT) {
				rcur[0]+=8;
				if (rcur[0] > 40) {
					rcur[0] = 40;
				}
				move_sprite(0, rcur[0], rcur[1]);
				curDel = 1;
			}
			if(joypad() & J_LEFT) {
				rcur[0]-=8;
				if (rcur[0] < 16) {
					rcur[0] = 16;
				}
				move_sprite(0, rcur[0], rcur[1]);
				curDel = 1;
			}
			if(joypad() & J_UP) {
				rcur[1]-=8;
				if (rcur[1] < 24) {
					rcur[1] = 24;
				}
				move_sprite(0, rcur[0], rcur[1]);
				curDel = 1;
			}
			if(joypad() & J_DOWN) {
				rcur[1]+=8;
				if (rcur[1] > 48) {
					rcur[1] = 48;
				}
				move_sprite(0, rcur[0], rcur[1]);
				curDel = 1;
			}
			if(joypad() & J_B) {
				HIDE_SPRITES;
				rcur[0] = 16;
				rcur[1] = 24;
				move_sprite(0, rcur[0], rcur[1]);
				selectmode = 0;
				curDel = 1;
			}
			if (joypad() & J_A) {
				HIDE_SPRITES;
				pickDie();
				selectmode = 0;
				temp = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
				map1[temp] = 0x14;
				updateBG();
			}
			//end yellow handling
		}
	}
}

void rollDice() {
	//Roll the colors and dice
	shuffle(diceAvaPalettes, 6);
	for (temp = 0; temp < 6; temp++)
		{
			int r1 = (rand() % 6);
			if (r1 < 0) {r1 = r1*-1;}
			temp2 = diceTiles[temp];
			cgbmap1[temp2] = diceAvaPalettes[temp];
			map1[temp2] = diceArray[r1];
			diceValues[temp] = r1;
		}
	updateBG();
}

void pickDie() {
	temp = (cur[0]+17)+(20*(cur[1]+1));
	temp3 = 0;
	temp5 = 0;
	for (temp2 = 0; temp2 < 3; temp2++){
		if (usedDiceValues[temp2] == 0xFF && map1[temp] != 0x48) {
			temp4 = 39+(20*temp2);
			map1[temp4] = map1[temp];
			cgbmap1[temp4] = cgbmap1[temp];
			usedDiceValues[temp2] = diceValues[cur[1]];
			map1[temp] = 0x48;
			/*cgbmap1[temp] = 0x00;*/
			
			for (temp5 = 0; temp5 < 6; temp5++)
			{
				if (diceValues[temp5] < diceValues[cur[1]]){
					map1[diceTiles[temp5]] = 0x48;
					/*cgbmap1[diceTiles[temp5]] = 0x00;*/
				}
			}
			
			
			updateBG();
			return;
		}
		else {
			temp5++;
		}
	}
}

void shuffle(unsigned char *array, UINT8 n) {    
	//Swap things around 100 or so times until it's sort of shuffled, probably a better way to do this
	for (temp = 0; temp < 20; temp++){
		int r1 = rand() % n;
		int r2 = rand() % n;
		if (r1 < 0) {r1 = r1*-1;}
		if (r2 < 0) {r2 = r2*-1;}
		temp2 = array[r1];
		array[r1] = array[r2];
		array[r2] = temp2;
	}
}

void newRound(){
	/*map1[0] = 0x15;*/
	for (temp2 = 0; temp2 < 3; temp2++){
		temp4 = 39+(20*temp2);
		map1[temp4] = 0x48;
		cgbmap1[temp4] = 0x00;
		usedDiceValues[temp2] = 0xFF;
	}
	temp3 = infoTrack[0]-1;
	map1[temp3+340] = 0x15;



	infoTrack[0]++;
	rollDice();
	updateBG();
}

void updateBG(){
	VBK_REG = 1;
	set_bkg_tiles(0,0,20,18,cgbmap1);
	VBK_REG = 0;
	set_bkg_tiles(0,0,20,18,map1);
}