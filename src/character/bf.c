/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend skull fragments
static SkullFragment char_bf_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
enum
{
	BF_ArcMain_BF0,
	BF_ArcMain_BF1,
	BF_ArcMain_BF2,
	BF_ArcMain_BF3,
	BF_ArcMain_BF4,
	BF_ArcMain_BF5,
	BF_ArcMain_BF6,
	BF_ArcMain_Dead0, //BREAK
	
	BF_ArcMain_Max,
};

enum
{
	BF_ArcDead_Dead1, //Mic Drop
	BF_ArcDead_Dead2, //Twitch
	BF_ArcDead_Retry, //Retry prompt
	
	BF_ArcDead_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bf_skull)];
	u8 skull_scale;
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = {
	{BF_ArcMain_BF0, { 10,   7,  95,  97}, { 54,  84}}, //0 idle 1
	{BF_ArcMain_BF0, {121,   6,  85, 103}, { 54,  91}}, //1 idle 2
	{BF_ArcMain_BF0, { 15, 113,  84, 105}, { 54,  92}}, //2 idle 3
	{BF_ArcMain_BF0, {118, 110,  83, 108}, { 54,  95}}, //3 idle 4
	{BF_ArcMain_BF1, { 20,   3,  83, 109}, { 54,  96}}, //4 idle 5
	
	{BF_ArcMain_BF1, {121,   4,  89, 106}, { 54,  92}}, //5 left 1
	{BF_ArcMain_BF1, { 13, 113,  87, 106}, { 54,  92}}, //6 left 2
	
	{BF_ArcMain_BF1, {118, 129,  90,  88}, { 54, 72}}, //7 down 1
	{BF_ArcMain_BF2, { 10,   7,  89,  88}, { 54, 72}}, //8 down 2
	
	{BF_ArcMain_BF2, {115,   0,  88, 112}, { 43,  98}}, //9 up 1
	{BF_ArcMain_BF2, {  6, 120,  89, 111}, { 43,  98}}, //10 up 2
	
	{BF_ArcMain_BF2, {111, 116,  88, 108}, { 43,  93}}, //11 right 1
	{BF_ArcMain_BF3, { 11,   4,  88, 108}, { 43,  93}}, //12 right 2
	
	{BF_ArcMain_BF3, {125,   2,  82, 112}, { 50,  96}}, //13 peace 1
	{BF_ArcMain_BF3, { 21, 114,  83, 109}, { 50,  96}}, //14 peace 2
	{BF_ArcMain_BF3, {126, 116,  83, 109}, { 50,  96}}, //15 peace 3
	
	{BF_ArcMain_BF4, { 24,   6,  83, 109}, { 51,  94}}, //16 sweat 1
	{BF_ArcMain_BF4, {128,   6,  83, 109}, { 51,  94}}, //17 sweat 2
	{BF_ArcMain_BF4, { 24, 123,  83, 109}, { 51,  94}}, //18 sweat 3
	{BF_ArcMain_BF4, {127, 127,  83, 109}, { 51,  94}}, //19 sweat 4
	
	{BF_ArcMain_BF5, {  8,  73, 132,  59}, { 52, 69}}, //20 left miss 1
	{BF_ArcMain_BF5, {148,  73, 100,  59}, { 59, 69}}, //21 left miss 2
	
	{BF_ArcMain_BF5, {  8, 172, 132,  59}, { 52, 69}}, //22 down miss 1
	{BF_ArcMain_BF5, {148, 172, 100,  59}, { 59, 69}}, //23 down miss 2
	
	{BF_ArcMain_BF6, {  8,  68, 132,  59}, { 52, 69}}, //24 up miss 1
	{BF_ArcMain_BF6, {148,  68, 100,  59}, { 59, 69}}, //25 up miss 2
	
	{BF_ArcMain_BF6, {  8, 177, 132,  59}, { 52, 69}}, //26 right miss 1
	{BF_ArcMain_BF6, {148, 177, 100,  59}, { 59, 69}}, //27 right miss 2

	{BF_ArcMain_Dead0, {  0,   0,  99, 103}, { 54,  96}}, //28 dead0 0
	{BF_ArcMain_Dead0, {134,   0,  99, 103}, { 54,  96}}, //29 dead0 1
	{BF_ArcMain_Dead0, {  5, 128,  99, 103}, { 54,  96}}, //30 dead0 2
	{BF_ArcMain_Dead0, {136, 128,  99, 103}, { 54,  96}}, //31 dead0 3
	
	{BF_ArcDead_Dead1, {  0,   0,  99, 103}, { 54,  96}}, //32 dead1 0
	{BF_ArcDead_Dead1, {134,   0,  99, 103}, { 54,  96}}, //33 dead1 1
	{BF_ArcDead_Dead1, {  5, 128,  99, 103}, { 54,  96}}, //34 dead1 2
	{BF_ArcDead_Dead1, {136, 128,  99, 103}, { 54,  96}}, //35 dead1 3
	
	{BF_ArcDead_Dead2, {  0,   0,  99, 103}, { 54,  96}}, //36 dead2 body twitch 0
	{BF_ArcDead_Dead2, {134,   0,  99, 103}, { 54,  96}}, //37 dead2 body twitch 1
	{BF_ArcDead_Dead2, {  5, 128,  99, 103}, { 54,  96}}, //38 dead2 balls twitch 0
	{BF_ArcDead_Dead2, {136, 128,  99, 103}, { 54,  96}}, //39 dead2 balls twitch 1
};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 5, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 7, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 9, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){11, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){28, 28, 28, 29, 29, 29, 30, 30, 30, 30, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){31, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){32, 32, 32, 33, 33, 33, 34, 34, 34, 34, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){31, 32, 30, 30, 30, 30, 30, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){32, 32, 33, 33, 34, 34, 34, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){36, 36, 37, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){38, 38, 39, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);
	Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			this->arc_dead = IO_AsyncReadFile(&this->file_dead_arc);
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Unload main.arc
			Mem_Free(this->arc_main);
			this->arc_main = this->arc_dead;
			this->arc_dead = NULL;
			
			//Find dead.arc files
			const char **pathp = (const char *[]){
				"dead1.tim", //BF_ArcDead_Dead1
				"dead2.tim", //BF_ArcDead_Dead2
				"retry.tim", //BF_ArcDead_Retry
				NULL
			};
			IO_Data *arc_ptr = this->arc_ptr;
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BF_ArcDead_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_dead);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 0;
	
	this->character.focus_x = FIXED_DEC(-25,1);
	this->character.focus_y = (stage.stage_id == StageId_1_4) ? FIXED_DEC(-90,1) : FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
	this->arc_dead = NULL;
	IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFDEAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"bf0.tim",   //BF_ArcMain_BF0
		"bf1.tim",   //BF_ArcMain_BF1
		"bf2.tim",   //BF_ArcMain_BF2
		"bf3.tim",   //BF_ArcMain_BF3
		"bf4.tim",   //BF_ArcMain_BF4
		"bf5.tim",   //BF_ArcMain_BF5
		"bf6.tim",   //BF_ArcMain_BF6
		"dead0.tim", //BF_ArcMain_Dead0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bf_skull, sizeof(char_bf_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
