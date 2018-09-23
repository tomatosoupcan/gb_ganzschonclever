//TODO: Bug testing and fixes!
//TODO: add music and sound effects

#include <gb/gb.h>
#include <gb/cgb.h>
#include "tiles.c"
#include "map.c"
#include "map2.c"
#include "map3.c"
#include "map4.c"
#include "rand.h"
#include "sprite1.c"
#include "window.c"
//#include "windowtiles.c"

UINT8 cur[2];
UINT8 curtemp[2];
UINT8 rcur[2];
UINT8 temp;
UINT8 temp2;
UINT8 temp3;
UINT8 temp4;
UINT8 temp5;
UINT8 temp6;
UINT8 temp7;
UINT8 whitemode;
UINT8 bwStore;
UINT8 opRound = 0;
UINT8 plusOne = 0;
UINT8 opUsed = 0;
UINT8 bonusRun = 0;
UINT8 individualscore[3];
int tempint;
unsigned char temp10;
unsigned char temp11;
UINT8 prevPurp;
UINT8 curDel;
UINT8 infoTrack[6];
UINT8 selectmode = 0;
UINT8 temparr[];
unsigned int seed;
UINT8 winActive = 0;

UINT8 unhiddenDice[] = 
{
	0,0,0,0,0,0
};

const unsigned char diceArray[] = 
{
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x48
};

unsigned char diceValues[] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00
};

const unsigned char usedDiceTiles[] =
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

const unsigned char diceTiles[] =
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

UINT8 g_array[] =
{
	1,2,3,4,5,1,2,3,4,5,6
};

const UINT8 g_prog[] = 
{
	0,1,3,6,10,15,21,28,36,45,55,66,0
};

const UINT8 b_prog[] = 
{
	0,1,2,4,7,11,16,22,29,37,46,56,0
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
UINT8 die2Num(unsigned char die);
UINT8 tile2Num(unsigned char tile);
void reroll();
UINT8 bignum2Tile(unsigned char num);
void midrollDice();
void bonus(int type);
void checkBonuses();
UINT8 getScore(UINT8 zone);
UINT8 scoreAssist(unsigned char tile);
void checkMoveBack();
void showPlusOnes();
void hidePlusOnes();
UINT8 num2Die(unsigned char num);
UINT8 checkLegal();

void main() {
	map1[340] = 21;
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
	HIDE_WIN;
	set_win_tiles(0,0,20,4, window);
	move_win(7,112);
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
	move_sprite(0, 0, 0);
	set_sprite_palette(0,8, bkg_palette);
	set_sprite_prop(0,1);
	
	rcur[0] = 0;
	rcur[1] = 0;
	cur[0] = 0; //set the cursor positions
	cur[1] = 0;
	infoTrack[0] = 1; //set the current round to 1
	infoTrack[1] = 0; //set the number of +1 to 0
	infoTrack[2] = 1; //set the number of rerolls to 0
	infoTrack[3] = 0; //set the score to 0
	infoTrack[4] = 0; //set the green pos to 0
	infoTrack[5] = 0; //set the blue pos to 0
	/*rollDice();*/
}

void updateSwitches() { //show just the background for now, will need a sprite and window eventually
	
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
		checkBonuses();
	}
	if (selectmode == 0){
		if (curDel == 0) {
			if (joypad() & J_DOWN && infoTrack[0] != 11) { //All of these move the cursor in the dice selector, will need to break this down further once more movement and sprites come into play
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
			if (joypad() & J_UP && infoTrack[0] != 11) {
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
			if (joypad() & J_RIGHT && infoTrack[0] != 11) {
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
			if (joypad() & J_LEFT && infoTrack[0] != 11) {
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
			if (joypad() & J_A && infoTrack[0] != 11) {
				if (cur[0] == 2 && cur[1] == 5 && infoTrack[2] > 0 && infoTrack[0] % 2 != 0) {
					infoTrack[2]--;
					reroll();
				}
				else if (cur[0] == 2 && cur[1] == 4 && infoTrack[1] > 0  && plusOne == 0) {
					infoTrack[1]--;
					plusOne = 1;
					if ((infoTrack[0]+1)%2 != 1){
						//map1[1]++;
						showPlusOnes();
					}
					updateBG();
				}
				else if (map1[39] != 72 && map1[59] != 72 && map1[79] != 72 && infoTrack[0] % 2 != 0) {
					return;
				}
				else if (infoTrack[0] % 2 == 0 && opUsed == 0 && plusOne == 0)
				{
					return;
				}
				else if (infoTrack[0] % 2 == 0 && cur[1] < 3 && cur[0] == 2 && map1[temp] != 0x48 && checkLegal() == 1) {
					selectmode = 1;
				}
				else if (cur[0] == 0 && map1[temp] != 0x48){
					selectmode = 1;
				}
				curDel = 1;
			}
			if (joypad() & J_START) {
				opRound = 0;
				curDel = 1;
				newRound();
			}
			if (joypad() & J_SELECT) {
				curDel = 1;
			}
		}
	}
	else if (selectmode == 1){
		temp = (cur[0]+17)+(20*(cur[1]+1));
		if (cgbmap1[temp] == 5 || whitemode == 5 || bonusRun == 1) { //yellow
			if (curDel == 0) {
				//Begin Yellow Handling
				if (rcur[0] == 0) {
					rcur[0] = 16;
					rcur[1] = 24;
					}
				move_sprite(0, rcur[0], rcur[1]);
				SHOW_SPRITES;
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
					rcur[0] = 0;
					rcur[1] = 0;
					move_sprite(0, rcur[0], rcur[1]);
					selectmode = 0;
					whitemode = 0;
					curDel = 1;
					bonusRun = 0;
				}
				temp2 = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
				temp3 = (cur[0]+17)+(20*(cur[1]+1));
				if (joypad() & J_A && map1[temp2] != 0x14) {
					if (die2Num(temp3) != tile2Num(temp2) && bonusRun == 0){
						curDel = 0;
						return;
					}
					HIDE_SPRITES;
					pickDie();
					selectmode = 0;
					temp = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
					rcur[0] = 0;
					rcur[1] = 0;
					move_sprite(0, rcur[0], rcur[1]);
					map1[temp] = 0x14;
					whitemode = 0;
					reroll();
					bonusRun = 0;
					checkMoveBack();
					updateBG();
				}
				//end yellow handling
			}
		}
		else if (cgbmap1[temp] == 1 || whitemode == 1 || bonusRun == 2) { //blue
			if (curDel == 0) {
				if (rcur[0] == 0) {
					rcur[0] = 88;
					rcur[1] = 24;
					}
				move_sprite(0, rcur[0], rcur[1]);
				SHOW_SPRITES;
				if(joypad() & J_RIGHT) {
					rcur[0]+=8;
					if (rcur[0] > 104) {
						rcur[0] = 104;
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_LEFT) {
					rcur[0]-=8;
					if (rcur[0] < 80) {
						rcur[0] = 80;
					}
					if (rcur[0] == 80 && rcur[1] == 24){
						rcur[0] = 88;
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_UP) {
					rcur[1]-=8;
					if (rcur[1] < 24) {
						rcur[1] = 24;
					}
					if (rcur[0] == 80 && rcur[1] == 24){
						rcur[1] = 32;
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_DOWN) {
					rcur[1]+=8;
					if (rcur[1] > 40) {
						rcur[1] = 40;
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_B) {
					HIDE_SPRITES;
					rcur[0] = 0;
					rcur[1] = 0;
					move_sprite(0, rcur[0], rcur[1]);
					selectmode = 0;
					whitemode = 0;
					bonusRun = 0;
					curDel = 1;
				}
				temp2 = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
				if (joypad() & J_A && map1[temp2] != 0x14) {
					if (bwStore != tile2Num(temp2) && bonusRun == 0){
						curDel = 0;
						return;
					}
					HIDE_SPRITES;
					pickDie();
					selectmode = 0;
					temp = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
					map1[temp] = 0x14;
					rcur[0] = 0;
					rcur[1] = 0;
					move_sprite(0, rcur[0], rcur[1]);
					infoTrack[5]++;
					map1[133] = bignum2Tile(b_prog[infoTrack[5]]);
					map1[134] = bignum2Tile(b_prog[infoTrack[5]+1]);
					whitemode = 0;
					reroll();
					bonusRun = 0;
					checkMoveBack();
					updateBG();
				}
			}
		}
		else if (cgbmap1[temp] == 4 || whitemode == 4 || bonusRun == 3) { //green
			if (map1[193] != 56){
				curDel = 0;
				selectmode = 0;
				return;}
			temp4 = 0;
			if (rcur[0] == 0) {
				rcur[0] = 88;
				rcur[1] = 24;
			}
			for (temp2 = 0; temp2 < 11; temp2++){
				map1[1] = g_array[temp2];
				map1[2] = die2Num(temp);
				map1[1] = 1;
				map1[2] = 1;
				updateBG();
				delay(5);
				if ((int)g_array[temp2] > (int)die2Num(temp)) {
					if ((map1[187] == 0x37 || temp2 > 4) && bonusRun == 0){
						if (whitemode == 4) {
							curDel = 0;
							whitemode = 0;
							return;
						}
						curDel = 0;
						selectmode = 0;
						return;
					}
				}
				if (map1[183+temp2] != 0x16 && map1[183+temp2] != 0x17 && map1[183+temp2] != 0x18 && map1[183+temp2] != 0x19 && map1[183+temp2] != 0x1A && map1[183+temp2] != 0x1B){
					temp3 = (cur[0]+17)+(20*(cur[1]+1));
					switch(map1[temp3]) {
						case 0x40 :
							temp10 = 0x16;
							break;
						case 0x41 :
							temp10 = 0x17;
							break;
						case 0x42 :
							temp10 = 0x18;
							break;
						case 0x43 :
							temp10 = 0x19;
							break;
						case 0x44 :
							temp10 = 0x1A;
							break;
						case 0x45 :
							temp10 = 0x1B;
							break;
					}
					if (bonusRun != 0) {temp10 = 0x1B;}
					map1[temp2+183] = temp10;
					cgbmap1[temp2+183] = 0x00;
					whitemode = 0;
					HIDE_SPRITES;
					rcur[0] = 0;
					rcur[1] = 0;
					infoTrack[4]++;
					map1[181] = bignum2Tile(g_prog[infoTrack[4]]);
					map1[182] = bignum2Tile(g_prog[infoTrack[4]+1]);
					checkMoveBack();
					updateBG();
					pickDie();
					reroll();
					bonusRun = 0;
					selectmode = 0;
					return;
				}
			}
		}
		else if (cgbmap1[temp] == 2 || whitemode == 2 || bonusRun == 4 || bonusRun == 5 || bonusRun == 6) { //orange
			if (map1[253] != 58){
				curDel = 0;
				selectmode = 0;
				return;}
			if (rcur[0] == 0) {
				rcur[0] = 88;
				rcur[1] = 24;
			}
			for (temp2 = 0; temp2 < 11; temp2++){
				if (map1[243+temp2] != 0x16 && map1[243+temp2] != 0x17 && map1[243+temp2] != 0x18 && map1[243+temp2] != 0x19 && map1[243+temp2] != 0x1A && map1[243+temp2] != 0x1B){
					temp3 = (cur[0]+17)+(20*(cur[1]+1));
					switch(map1[temp3]) {
						case 0x40 :
							temp10 = 0x16;
							break;
						case 0x41 :
							temp10 = 0x17;
							break;
						case 0x42 :
							temp10 = 0x18;
							break;
						case 0x43 :
							temp10 = 0x19;
							break;
						case 0x44 :
							temp10 = 0x1A;
							break;
						case 0x45 :
							temp10 = 0x1B;
							break;
					}
					switch (bonusRun) {
						case 4:
							temp10 = 0x19;
							break;
						case 5:
							temp10 = 0x1A;
							break;
						case 6:
							temp10 = 0x1B;
							break;
						case 8:
							temp10 = 0x1B;
						default:
							break;
					}
					map1[temp2+243] = temp10;
					cgbmap1[temp2+243] = 0x00;
					whitemode = 0;
					HIDE_SPRITES;
					rcur[0] = 0;
					rcur[1] = 0;
					checkMoveBack();
					updateBG();
					pickDie();
					reroll();
					bonusRun = 0;
					selectmode = 0;
					return;
				}
			}
		}
		else if (cgbmap1[temp] == 3 || whitemode == 3 || bonusRun == 7) { //purple
			if (map1[313] != 59){
				curDel = 0;
				selectmode = 0;
				return;}
			if (rcur[0] == 0) {
				rcur[0] = 88;
				rcur[1] = 24;
			}
			for (temp2 = 0; temp2 < 11; temp2++){
				if (map1[303+temp2] != 0x16 && map1[303+temp2] != 0x17 && map1[303+temp2] != 0x18 && map1[303+temp2] != 0x19 && map1[303+temp2] != 0x1A && map1[303+temp2] != 0x1B){
					temp3 = (cur[0]+17)+(20*(cur[1]+1));
					if (temp2 == 0 || prevPurp < die2Num(temp3) || prevPurp == 6) {
						prevPurp = die2Num(temp3);
					}
					else if (bonusRun != 0) {
						prevPurp = 6;
					}
					else {
						if (whitemode == 3) {
							curDel = 0;
							whitemode = 0;
							return;
						}
						curDel = 0;
						selectmode = 0;
						return;
					}

					switch(map1[temp3]) {
						case 0x40 :
							temp10 = 0x16;
							break;
						case 0x41 :
							temp10 = 0x17;
							break;
						case 0x42 :
							temp10 = 0x18;
							break;
						case 0x43 :
							temp10 = 0x19;
							break;
						case 0x44 :
							temp10 = 0x1A;
							break;
						case 0x45 :
							temp10 = 0x1B;
							break;
					}
					if (bonusRun != 0) {temp10 = 0x1B;}
					map1[temp2+303] = temp10;
					cgbmap1[temp2+303] = 0x00;
					whitemode = 0;
					HIDE_SPRITES;
					rcur[0] = 0;
					rcur[1] = 0;
					checkMoveBack();
					updateBG();
					pickDie();
					reroll();
					selectmode = 0;
					bonusRun = 0;
					return;
				}
			}
		}
		else if (cgbmap1[temp] == 0 || bonusRun == 8) { //white
			if (curDel == 0) {
				if (rcur[0] == 0) {
					rcur[0] = 8;
					rcur[1] = 16;
					}
				move_sprite(0, rcur[0], rcur[1]);
				SHOW_SPRITES;
				if(joypad() & J_RIGHT) {
					if (rcur[0] != 72) {
						rcur[0]+=64;
						if (rcur[1] != 16) {
							rcur[0] = 8;
						}
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_LEFT) {
					if (rcur[0] != 8) {
						rcur[0]-=64;
						if (rcur[0] < 8) {
							rcur[0] = 64;
						}
						move_sprite(0, rcur[0], rcur[1]);
						curDel = 1;
					}
				}
				if(joypad() & J_UP) {
					if (rcur[1] != 16) {
						if (rcur[1] == 80) {
						rcur[1]-=64;
						}
						else {
							rcur[1]-=24;
						}
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_DOWN) {
					if (rcur[1] == 16 && rcur[0] == 8) {
						rcur[1]+=64;
					}
					else if (rcur[0] == 8) {
						rcur[1]+=24;
					}
					if (rcur[1] > 128) {
						rcur[1] = 128;
					}
					move_sprite(0, rcur[0], rcur[1]);
					curDel = 1;
				}
				if(joypad() & J_B) {
					HIDE_SPRITES;
					rcur[0] = 0;
					rcur[1] = 0;
					move_sprite(0, rcur[0], rcur[1]);
					selectmode = 0;
					curDel = 1;
					bonusRun = 0;
				}
				if (joypad() & J_A && map1[temp2] != 0x14) {
					curDel = 1;
					temp = (((rcur[0]/8))+(((rcur[1]/8)-2)*20))-1;
					switch (cgbmap1[temp]){
						case 5 : //yellow
							rcur[0] = 16;
							rcur[1] = 24;
							whitemode = 5;
							updateBG();
							break;
						case 1 : //blue
							rcur[0] = 88;
							rcur[1] = 24;
							whitemode = 1;
							updateBG();
							break;
						case 4 : //green
							whitemode = 4;
							checkInput();
							break;
						case 2 : //orange
							whitemode = 2;
							checkInput();
							break;
						default : //purple
							whitemode = 3;
							checkInput();
							break;
					}
				}
			}
		}
	}
}

void reroll() {
	curDel = 1;
	if (bonusRun != 0){
		return;}
	if (opUsed == 1 && plusOne == 0) 
	{
		//newRound();
		opUsed = 0;
	}
	else if (plusOne == 1){
		plusOne = 0;
		hidePlusOnes();
		return;
	}
	else if (infoTrack[0] % 2 == 0) {
		return;
	}
	else {
		temp11 = 0x02;
		for (temp = 0; temp < 6; temp++)
		{
			tempint = (rand() % 6);
			if (tempint < 0) {tempint = tempint*-1;}
			temp2 = diceTiles[temp];
			if (map1[temp2] != 72) {
				cgbmap1[temp2] = diceAvaPalettes[temp];
				map1[temp2] = diceArray[tempint];
				diceValues[temp] = tempint;
			}
		}
		for (temp = 0; temp < 6; temp++)
			{
				if (diceAvaPalettes[temp] == 0x00 || diceAvaPalettes[temp] == 0x01)
				{
					temp11 = temp11 + diceValues[temp];
				}
			}
		bwStore = temp11;
		switch (temp11){
			case 2 :
				temp11 = 38;
				break;
			case 3 :
				temp11 = 78;
				break;
			case 4 :
				temp11 = 39;
				break;
			case 5 :
				temp11 = 79;
				break;
			case 6 :
				temp11 = 80;
				break;
			case 7 :
				temp11 = 40;
				break;
			case 8 :
				temp11 = 114;
				break;
			case 9 :
				temp11 = 115;
				break;
			case 10 :
				temp11 = 82;
				break;
			case 11 :
				temp11 = 41;
				break;
			case 12 :
				temp11 = 116;
				break;
			default :
				break;
		}
		map1[29] = temp11;
		cgbmap1[29] = 0x01;
	}
	updateBG();
}

void rollDice() {
	//Roll the colors and dice
	shuffle(diceAvaPalettes, 6);
	temp11 = 0x02;
	for (temp = 0; temp < 6; temp++)
		{
			int r1 = (rand() % 6);
			if (r1 < 0) {r1 = r1*-1;}
			temp2 = diceTiles[temp];
			cgbmap1[temp2] = diceAvaPalettes[temp];
			map1[temp2] = diceArray[r1];
			diceValues[temp] = r1;
		}
	for (temp = 0; temp < 6; temp++)
		{
			if (diceAvaPalettes[temp] == 0x00 || diceAvaPalettes[temp] == 0x01)
			{
				temp11 = temp11 + diceValues[temp];
			}
		}
	bwStore = temp11;
	switch (temp11){
		case 2 :
			temp11 = 38;
			break;
		case 3 :
			temp11 = 78;
			break;
		case 4 :
			temp11 = 39;
			break;
		case 5 :
			temp11 = 79;
			break;
		case 6 :
			temp11 = 80;
			break;
		case 7 :
			temp11 = 40;
			break;
		case 8 :
			temp11 = 114;
			break;
		case 9 :
			temp11 = 115;
			break;
		case 10 :
			temp11 = 82;
			break;
		case 11 :
			temp11 = 41;
			break;
		case 12 :
			temp11 = 116;
			break;
		default :
			break;
	}
	map1[29] = temp11;
	cgbmap1[29] = 0x01;
	updateBG();
}

void pickDie() {
	if (bonusRun != 0){
		return;}
	rcur[0] = 0;
	rcur[1] = 0;
	if (infoTrack[0] % 2 != 1 && plusOne == 0) {
		return;
	}
	else if (infoTrack[0] % 2 == 1 && plusOne == 1){
		hidePlusOnes();
		updateBG();
		return;
	}
	//plusOne = 0;
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
	rcur[0] = 0;
	rcur[1] = 0;
	/*map1[0] = 0x15;*/
	if (opUsed == 1 && plusOne == 0){opUsed = 0;}
	for (temp2 = 0; temp2 < 3; temp2++){
		temp4 = 39+(20*temp2);
		map1[temp4] = 0x48;
		cgbmap1[temp4] = 0x00;
		usedDiceValues[temp2] = 0xFF;
	}
	plusOne = 0;
	temp3 = (infoTrack[0])/2;
	map1[(int)temp3+340] = 0x15;
	switch (infoTrack[0]) {
		case 2:
			infoTrack[1]++;
			break;
		case 4:
			infoTrack[2]++;
			break;
		case 6:
			bonus(8);
			break;
		case 10:
			//show the score here
			set_win_tiles(0,0,20,4, window);
			SHOW_WIN;
			break;
		case 11:
			//show the score here
			HIDE_BKG;
			curDel = 1;
			for (tempint = 0; tempint < 359; tempint++){
				map1[tempint] = map1s[tempint];
				cgbmap1[tempint] = cgbmap1s[tempint];
			}
			map1[340] = 21;
			init();

			while(!joypad()){seed++; if(seed>=255)seed=1;} //generate the seed based on when you hit a button
			initrand(seed);
			rollDice();
			updateBG();
			infoTrack[0] = 0;
			break;
	}

	if (infoTrack[0] % 2 == 0) {
		rollDice();
	}
	else {
		midrollDice();
	}

	infoTrack[0]++;
	updateBG();
}

void updateBG(){
	temp5 = infoTrack[3] + getScore(1) + getScore(2) + getScore(3) + getScore(4) + getScore(5);
	individualscore[0] = (int)temp5/100;
	individualscore[1] = (int)(temp5-individualscore[0]*100)/10;
	individualscore[2] = (int)(temp5-individualscore[0]*100-individualscore[1]*10);
	window[32] = bignum2Tile(individualscore[0]);
	window[33] = bignum2Tile(individualscore[1]);
	window[34] = bignum2Tile(individualscore[2]);


	map1[350] = bignum2Tile(infoTrack[1]);
	map1[353] = bignum2Tile(infoTrack[2]);
	VBK_REG = 1;
	set_bkg_tiles(0,0,20,18,cgbmap1);
	VBK_REG = 0;
	set_bkg_tiles(0,0,20,18,map1);
}

UINT8 tile2Num(unsigned char tile){
	switch (map1[tile]){
		case 22:
			return 1;
			break;
		case 23:
			return 2;
			break;
		case 24:
			return 3;
			break;
		case 25:
			return 4;
			break;
		case 26:
			return 5;
			break;
		case 27:
			return 6;
			break;
		case 29:
			return 7;
			break;
		case 30:
			return 8;
			break;
		case 31:
			return 9;
			break;
		case 32:
			return 10;
			break;
		case 33:
			return 11;
			break;
		case 34:
			return 12;
			break;
		default:
			return 0;
			break;
	}
}

UINT8 die2Num(unsigned char die){
	switch (map1[die]) {
		case 64:
			return 1;
			break;
		case 65:
			return 2;
			break;
		case 66:
			return 3;
			break;
		case 67:
			return 4;
			break;
		case 68:
			return 5;
			break;
		case 69:
			return 6;
			break;
	}
}

UINT8 num2Die(unsigned char num){
	switch (num) {
		case 1:
			return 64;
			break;
		case 2:
			return 65;
			break;
		case 3:
			return 66;
			break;
		case 4:
			return 67;
			break;
		case 5:
			return 68;
			break;
		case 6:
			return 69;
			break;
	}
}

UINT8 bignum2Tile(unsigned char num){
	switch (num) {
		case 1:
			return 37;
			break;
		case 2:
			return 38;
			break;
		case 4:
			return 39;
			break;
		case 7:
			return 40;
			break;
		case 11:
			return 41;
			break;
		case 16:
			return 42;
			break;
		case 22:
			return 43;
			break;
		case 29:
			return 44;
			break;
		case 37:
			return 45;
			break;
		case 46:
			return 46;
			break;
		case 56:
			return 47;
			break;
		case 0:
			return 48;
			break;	
		case 10:
			return 12;
			break;
		case 14:
			return 13;
			break;
		case 20:
			return 15;
			break;
		case 3:
			return 78;
			break;	
		case 5:
			return 79;
			break;	
		case 6:
			return 80;
			break;	
		case 15:
			return 83;
			break;	
		case 21:
			return 84;
			break;	
		case 28:
			return 85;
			break;
		case 36:
			return 86;
			break;	
		case 45:
			return 87;
			break;	
		case 55:
			return 88;
			break;	
		case 66:
			return 89;
			break;			
		case 8:
			return 114;
			break;	
		case 9:
			return 115;
			break;
		case 12:
			return 116;
			break;			
	}
}

void midrollDice() {
	//Roll the colors and dice
	shuffle(diceAvaPalettes, 6);
	temp11 = 0x02;
	for (temp = 0; temp < 6; temp++)
		{
			int r1 = (rand() % 6);
			if (r1 < 0) {r1 = r1*-1;}
			if (temp < 3) {
				temp2 = diceTiles[temp];
				cgbmap1[temp2] = diceAvaPalettes[temp];
				map1[temp2] = diceArray[r1];
				diceValues[temp] = r1;
				if (diceAvaPalettes[temp] == 0x00 || diceAvaPalettes[temp] == 0x01) {
					temp11 = temp11 + diceValues[temp];
				}
			}
			else {
				temp2 = usedDiceTiles[temp-3]-1;
				cgbmap1[temp2] = diceAvaPalettes[temp];
				map1[temp2] = diceArray[r1];
				usedDiceValues[temp-3] = r1;
				diceValues[temp] = 0;
				temp2 = diceTiles[temp];
				cgbmap1[temp2] = 0x00;
				map1[temp2] = 0x48;
				if (diceAvaPalettes[temp] == 0x00 || diceAvaPalettes[temp] == 0x01) {
					temp11 = temp11 + usedDiceValues[temp-3];
				}
			}

		}
	bwStore = temp11;
	switch (temp11){
		case 2 :
			temp11 = 38;
			break;
		case 3 :
			temp11 = 78;
			break;
		case 4 :
			temp11 = 39;
			break;
		case 5 :
			temp11 = 79;
			break;
		case 6 :
			temp11 = 80;
			break;
		case 7 :
			temp11 = 40;
			break;
		case 8 :
			temp11 = 114;
			break;
		case 9 :
			temp11 = 115;
			break;
		case 10 :
			temp11 = 82;
			break;
		case 11 :
			temp11 = 41;
			break;
		case 12 :
			temp11 = 116;
			break;
		default :
			break;
	}
	map1[29] = temp11;
	cgbmap1[29] = 0x01;
	selectmode = 0;
	opRound = 1;
	opUsed = 1;
	updateBG();
}

void bonus(int type) {
	curDel=1;
	//1 is yellow bonus
	//2 is blue bonus
	//3 is green bonus
	//4 is orange 4
	//5 is orange 5
	//6 is orange 6
	//7 is purple bonus
	//8 is wildcard bonus
	//9 is fox
	//10 is +1
	//11 is reroll
	if (type < 9)
	{curtemp[0] = cur[0];
	curtemp[1] = cur[1];
	cur[0] = 2;
	cur[1] = 3;
	clearCursor();
	bonusRun = type;
	selectmode = 1;
	}
	else if (type == 9){
		temparr[0] = getScore(1);
		temparr[1] = getScore(2);
		temparr[2] = getScore(3);
		temparr[3] = getScore(4);
		temparr[4] = getScore(5);
		tempint = 0;
		for (temp5 = 0; temp5 < 5; temp5++){
			if (temp5 == 0){
				tempint = temparr[temp5];
			}
			else {
				if (temparr[temp5]<tempint){
					tempint = temparr[temp5];
				}
			}
		}
		
		infoTrack[3]+=tempint; //increment the score by the lowest section
	}
	else if (type == 10){
		curDel = 1;
		infoTrack[1]++;
		updateBG();
	}
	else if (type == 11){
		curDel = 1;
		infoTrack[2]++;
		updateBG();
	}
	//checkInput();
}

void checkBonuses(){
	if (map1[21] == map1[22] && map1[22] == map1[23] && map1[23] == map1[24] && map1[26] != 21) {
		map1[26] = 21;
		cgbmap1[26] = 0x00;
		bonus(2);
	}
	else if (map1[41] == map1[42] && map1[42] == map1[43] && map1[43] == map1[44] && map1[46] != 21) {
		map1[46] = 21;
		cgbmap1[46] = 0x00;
		bonus(4);
	}
	else if (map1[61] == map1[62] && map1[62] == map1[63] && map1[63] == map1[64] && map1[66] != 21) {
		map1[66] = 21;
		cgbmap1[66] = 0x00;
		bonus(3);
	}
	else if (map1[81] == map1[82] && map1[82] == map1[83] && map1[83] == map1[84] && map1[86] != 21) {
		map1[86] = 21;
		cgbmap1[86] = 0x00;
		bonus(9);
	}
	else if (map1[21] == map1[42] && map1[42] == map1[63] && map1[63] == map1[84] && map1[126] != 21) {
		map1[126] = 21;
		cgbmap1[126] = 0x00;
		bonus(10);
	}
	else if (map1[30] == map1[31] && map1[31] == map1[32] && map1[34] != 21) {
		map1[34] = 21;
		cgbmap1[34] = 0x00;
		bonus(5);
	}
	else if (map1[49] == map1[50] && map1[50] == map1[51] && map1[51] == map1[52] && map1[54] != 21) {
		map1[54] = 21;
		cgbmap1[54] = 0x00;
		bonus(1);
	}
	else if (map1[69] == map1[70] && map1[70] == map1[71] && map1[71] == map1[72] && map1[74] != 21) {
		map1[74] = 21;
		cgbmap1[74] = 0x00;
		bonus(9);
	}
	else if (map1[49] == map1[69] && map1[109] != 21) {
		map1[109] = 21;
		cgbmap1[109] = 0x00;
		bonus(11);
	}
	else if (map1[30] == map1[50] && map1[50] == map1[70] && map1[110] != 21) {
		map1[110] = 21;
		cgbmap1[110] = 0x00;
		bonus(3);
	}
	else if (map1[31] == map1[51] && map1[51] == map1[71] && map1[111] != 21) {
		map1[111] = 21;
		cgbmap1[111] = 0x00;
		bonus(7);
	}
	else if (map1[32] == map1[52] && map1[52] == map1[72] && map1[112] != 21) {
		map1[112] = 21;
		cgbmap1[112] = 0x00;
		bonus(10);
	}
	else if (map1[186] != 60 && map1[176] != 21){
		map1[176] = 21;
		cgbmap1[176] = 0x00;
		bonus(10);
	}
	else if (map1[188] != 61 && map1[177] != 21){
		map1[177] = 21;
		cgbmap1[177] = 0x00;
		bonus(2);
	}
	else if (map1[189] != 62 && map1[178] != 21){
		map1[178] = 21;
		cgbmap1[178] = 0x00;
		bonus(9);
	}
	else if (map1[191] != 60 && map1[179] != 21){
		map1[179] = 21;
		cgbmap1[179] = 0x00;
		bonus(7);
	}
	else if (map1[192] != 63 && map1[196] != 21){
		map1[196] = 21;
		cgbmap1[196] = 0x00;
		bonus(11);
	}
	else if (map1[245] != 59 && map1[236] != 21){
		map1[236] = 21;
		cgbmap1[236] = 0x00;
		bonus(11);
	}
	else if (map1[247] != 59 && map1[237] != 21){
		map1[237] = 21;
		cgbmap1[237] = 0x00;
		bonus(1);
	}
	else if (map1[248] != 59 && map1[238] != 21){
		map1[238] = 21;
		cgbmap1[238] = 0x00;
		bonus(10);
	}
	else if (map1[250] != 59 && map1[239] != 21){
		map1[239] = 21;
		cgbmap1[239] = 0x00;
		bonus(9);
	}
	else if (map1[252] != 59 && map1[256] != 21){
		map1[256] = 21;
		cgbmap1[256] = 0x00;
		bonus(7);
	}
	else if (map1[305] != 59 && map1[296] != 21){
		map1[296] = 21;
		cgbmap1[296] = 0x00;
		bonus(11);
	}
	else if (map1[306] != 59 && map1[297] != 21){
		map1[297] = 21;
		cgbmap1[297] = 0x00;
		bonus(2);
	}
	else if (map1[307] != 59 && map1[298] != 21){
		map1[298] = 21;
		cgbmap1[298] = 0x00;
		bonus(10);
	}
	else if (map1[308] != 59 && map1[299] != 21){
		map1[299] = 21;
		cgbmap1[299] = 0x00;
		bonus(1);
	}
	else if (map1[309] != 59 && map1[316] != 21){
		map1[316] = 21;
		cgbmap1[316] = 0x00;
		bonus(9);
	}
	else if (map1[310] != 59 && map1[317] != 21){
		map1[317] = 21;
		cgbmap1[317] = 0x00;
		bonus(11);
	}
	else if (map1[311] != 59 && map1[318] != 21){
		map1[318] = 21;
		cgbmap1[318] = 0x00;
		bonus(3);
	}
	else if (map1[312] != 59 && map1[319] != 21){
		map1[319] = 21;
		cgbmap1[319] = 0x00;
		bonus(6);
	}
	else if (map1[313] != 59 && map1[336] != 21){
		map1[336] = 21;
		cgbmap1[336] = 0x00;
		bonus(10);
	}
	updateBG();
}

UINT8 getScore(UINT8 zone) {
	temp5 = 0;
	switch (zone) {
		case 1: //yellow
			if (map1[21] == map1[41] && map1[41] == map1[61] && map1[61] == map1[81]) {
				temp5+=10;
			}
			if (map1[22] == map1[42] && map1[42] == map1[62] && map1[62] == map1[82]) {
				temp5+=14;
			}
			if (map1[23] == map1[43] && map1[43] == map1[63] && map1[63] == map1[83]) {
				temp5+=16;
			}
			if (map1[24] == map1[44] && map1[44] == map1[64] && map1[64] == map1[84]) {
				temp5+=20;
			}
			return temp5;
		case 2: //blue
			return scoreAssist(map1[133]);
		case 3: //green
			return scoreAssist(map1[181]);
		case 4: //orange
			for (temp4 = 0; temp4 < 11; temp4++){
				if (temp4 == 3 || temp4 == 6 || temp4 == 8){
					temp5+=(tile2Num(temp4+243))*2;
				}
				else if (temp4 == 10) {
					temp5+=(tile2Num(temp4+243))*3;
				}
				else {
					temp5+=tile2Num(temp4+243);
				}
			}	
			return temp5;	
		case 5: //purple
			for (temp4 = 0; temp4 < 11; temp4++) {
				switch (map1[303+temp4]){
					case 22:
						temp5+=1;
						break;
					case 23:
						temp5+=2;
						break;
					case 24:
						temp5+=3;
						break;
					case 25:
						temp5+=4;
						break;
					case 26:
						temp5+=5;
						break;
					case 27:
						temp5+=6;
						break;
					default:
						temp5+=0;
						break;
				}
			}
			return temp5;
	}
}

const UINT8 scoreAssist(unsigned char tile) {
	switch (tile) {
		case 37:
			return 1;
			break;
		case 38:
			return 2;
			break;
		case 39:
			return 4;
			break;
		case 40:
			return 7;
			break;
		case 41:
			return 11;
			break;
		case 42:
			return 16;
			break;
		case 43:
			return 22;
			break;
		case 44:
			return 29;
			break;
		case 45:
			return 37;
			break;
		case 46:
			return 46;
			break;
		case 47:
			return 56;
			break;
		case 48:
			return 0;
			break;	
		case 12:
			return 10;
			break;
		case 13:
			return 14;
			break;
		case 15:
			return 20;
			break;
		case 78:
			return 3;
			break;	
		case 79:
			return 5;
			break;	
		case 80:
			return 6;
			break;	
		case 83:
			return 15;
			break;	
		case 84:
			return 21;
			break;	
		case 85:
			return 28;
			break;
		case 86:
			return 36;
			break;	
		case 87:
			return 45;
			break;	
		case 88:
			return 55;
			break;	
		case 89:
			return 66;
			break;			
		case 114:
			return 8;
			break;	
		case 115:
			return 9;
			break;
		case 116:
			return 12;
			break;
		default:
			return 0;
			break;		
	}
}

void checkMoveBack(){
	if (cur[1] == 3 && cur[0] == 2) {
		cur[0] = curtemp[0];
		cur[1] = curtemp[1];
		moveCursor();
	}
}

void showPlusOnes(){
	winActive = 1;
	for (temp5 = 0; temp5 < 6; temp5++)
	{
		//map1[temp5+1] = num2Die(diceValues[temp5]+1);
		if (map1[37 + temp5*20] == 72) {
			unhiddenDice[temp5] = 1;
			map1[37 + temp5*20] = num2Die(diceValues[temp5]+1);
		}
	}
}

void hidePlusOnes(){
	winActive = 0;
	for (temp5 = 0; temp5 < 6; temp5++)
	{
		//map1[temp5+1] = num2Die(diceValues[temp5]+1);
		if (unhiddenDice[temp5] == 1) {
			unhiddenDice[temp5] = 0;
			map1[37 + temp5*20] = 72;
		}
	}
}

UINT8 checkLegal(){
	tempint = 0;
	for (temp5 = 0; temp5 < 3; temp5++)
	{
		temp4 = die2Num(37+(temp5*20));
		temp3 = cgbmap1[37+(temp5*20)];
		switch (temp3){
			case 0:
				//check white
				temp6 = 0;
				if (map1[253] == 20 && map1[313] == 20 && map1[193] == 20)
				{
					if (scoreAssist(map1[29]) == 2 && map1[30] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 3 && map1[31] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 4 && map1[32] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 5 && map1[49] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 6 && map1[50] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 7 && map1[51] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 8 && map1[52] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 9 && map1[69] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 10 && map1[70] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 11 && map1[71] == 20){temp6++;}
					else if (scoreAssist(map1[29]) == 12 && map1[72] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 1 && map1[42] == 20 && map1[61] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 2 && map1[41] == 20 && map1[63] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 3 && map1[21] == 20 && map1[82] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 4 && map1[83] == 20 && map1[64] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 5 && map1[23] == 20 && map1[44] == 20){temp6++;}
					else if (die2Num(39+(temp5*20)) == 6 && map1[22] == 20 && map1[84] == 20){temp6++;}		
				}
				if (temp6 != 0){tempint++;}
				break;
			case 1:
				//check blue
				temp6 = 0;
				if (scoreAssist(map1[29]) == 2 && map1[30] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 3 && map1[31] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 4 && map1[32] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 5 && map1[49] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 6 && map1[50] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 7 && map1[51] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 8 && map1[52] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 9 && map1[69] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 10 && map1[70] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 11 && map1[71] == 20){temp6++;}
				else if (scoreAssist(map1[29]) == 12 && map1[72] == 20){temp6++;}
				if (temp6 != 0){tempint++;}
				break;
			case 2:
				//check orange 
				if (map1[253] == 20){tempint++;}
				break;
			case 3:
				//check purple
				if (map1[313] == 20){tempint++;}
				else if (prevPurp >= die2Num(39+(temp5*20))) {tempint++;}
				break;
			case 4:
				//check green
				temp6 = 0;
				if (map1[193] == 20){tempint++;}
				else if (map1[183] == 51 && die2Num(39+(temp5*20)) < 1){temp6++;}
				else if (map1[184] == 52 && die2Num(39+(temp5*20)) < 2){temp6++;}
				else if (map1[185] == 53 && die2Num(39+(temp5*20)) < 3){temp6++;}
				else if (map1[186] == 60 && die2Num(39+(temp5*20)) < 4){temp6++;}
				else if (map1[187] == 55 && die2Num(39+(temp5*20)) < 5){temp6++;}
				else if (map1[188] == 61 && die2Num(39+(temp5*20)) < 1){temp6++;}
				else if (map1[189] == 62 && die2Num(39+(temp5*20)) < 2){temp6++;}
				else if (map1[190] == 53 && die2Num(39+(temp5*20)) < 3){temp6++;}
				else if (map1[191] == 60 && die2Num(39+(temp5*20)) < 4){temp6++;}
				else if (map1[192] == 63 && die2Num(39+(temp5*20)) < 5){temp6++;}
				else if (map1[193] == 56 && die2Num(39+(temp5*20)) < 6){temp6++;}
				if (temp6 != 0){tempint++;}
				break;
			case 5:
				temp6 = 0;
				if (die2Num(39+(temp5*20)) == 1 && map1[42] == 20 && map1[61] == 20){temp6++;}
				else if (die2Num(39+(temp5*20)) == 2 && map1[41] == 20 && map1[63] == 20){temp6++;}
				else if (die2Num(39+(temp5*20)) == 3 && map1[21] == 20 && map1[82] == 20){temp6++;}
				else if (die2Num(39+(temp5*20)) == 4 && map1[83] == 20 && map1[64] == 20){temp6++;}
				else if (die2Num(39+(temp5*20)) == 5 && map1[23] == 20 && map1[44] == 20){temp6++;}
				else if (die2Num(39+(temp5*20)) == 6 && map1[22] == 20 && map1[84] == 20){temp6++;}
				if (temp6 != 0){tempint++;}
				break;

		}
	}
	if (tempint >= 3){
		return 1;
	}
	else{
		return 0;
	}
}