/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "spong2.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Spong2 character structure
enum
{
	Spong2_ArcMain_Idle0,
	Spong2_ArcMain_Idle1,
	Spong2_ArcMain_Idle2,
	Spong2_ArcMain_Idle3,
	Spong2_ArcMain_LeftA0,
	Spong2_ArcMain_LeftA1,
	Spong2_ArcMain_LeftB0,
	Spong2_ArcMain_LeftB1,
	Spong2_ArcMain_DownA0,
	Spong2_ArcMain_DownA1,
	Spong2_ArcMain_DownB0,
	Spong2_ArcMain_DownB1,
	Spong2_ArcMain_UpA0,
	Spong2_ArcMain_UpA1,
	Spong2_ArcMain_UpB0,
	Spong2_ArcMain_UpB1,
	Spong2_ArcMain_RightA0,
	Spong2_ArcMain_RightA1,
	Spong2_ArcMain_RightB0,
	Spong2_ArcMain_RightB1,
	Spong2_ArcMain_Scream0,
	Spong2_ArcMain_Scream1,
	Spong2_ArcMain_Scream2,
	
	Spong2_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Spong2_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Spong2;

//Spong2 character definitions
static const CharFrame char_spong2_frame[] = {
	{Spong2_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Spong2_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Spong2_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Spong2_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4

	{Spong2_ArcMain_LeftA0, {  0,   0, 256, 256}, { 125, 250}}, //4 left a 1
	{Spong2_ArcMain_LeftA1, {  0,   0, 256, 256}, { 125, 250}}, //5 left a 2
	{Spong2_ArcMain_LeftB0, {  0,   0, 256, 256}, { 125, 250}}, //6 left b 1
	{Spong2_ArcMain_LeftB1, {  0,   0, 256, 256}, { 125, 250}}, //7 left b 2
	
	{Spong2_ArcMain_DownA0, {  0,   0, 256, 256}, { 125, 250}}, //8 down a 1
	{Spong2_ArcMain_DownA1, {  0,   0, 256, 256}, { 125, 250}}, //9 down a 2
	{Spong2_ArcMain_DownB0, {  0,   0, 256, 256}, { 125, 250}}, //10 down b 1
	{Spong2_ArcMain_DownB1, {  0,   0, 256, 256}, { 125, 250}}, //11 down b 2
	
	{Spong2_ArcMain_UpA0, {   0,   0,  256, 256}, { 125, 250}}, //12 up a 1
	{Spong2_ArcMain_UpA1, {   0,   0,  256, 256}, { 125, 250}}, //13 up a 2
	{Spong2_ArcMain_UpB0, {   0,   0,  256, 256}, { 125, 250}}, //14 up b 1
	{Spong2_ArcMain_UpB1, {   0,   0,  256, 256}, { 125, 250}}, //15 up b 2
	
	{Spong2_ArcMain_RightA0, {  0,   0, 256, 256}, { 104, 250}}, //16 right a 1
	{Spong2_ArcMain_RightA1, {  0,   0, 256, 256}, { 104, 250}}, //17 right a 2
	{Spong2_ArcMain_RightB0, {  0,   0, 256, 256}, { 104, 250}}, //18 right b 1
	{Spong2_ArcMain_RightB1, {  0,   0, 256, 256}, { 104, 250}}, //19 right b 2

	{Spong2_ArcMain_Scream0, {  0,   0, 256, 256}, { 104, 250}}, //20 scream 1
	{Spong2_ArcMain_Scream1, {  0,   0, 256, 256}, { 104, 250}}, //21 scream 2
	{Spong2_ArcMain_Scream2, {  0,   0, 256, 256}, { 104, 250}}, //22 scream 3
};

static const Animation char_spong2_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  2, ASCR_BACK, 1}},                 	   //CharAnim_Idle
	{2, (const u8[]){ 4,  5,  4, ASCR_BACK, 1}},       		           //CharAnim_Left
	{2, (const u8[]){ 6,  7,  6, ASCR_BACK, 1}},                     	   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9,  8, ASCR_BACK, 1}},  		                   //CharAnim_Down
	{2, (const u8[]){10, 11, 10, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAlt
	{2, (const u8[]){12, 13, 12, ASCR_BACK, 1}},              		   //CharAnim_Up
	{2, (const u8[]){14, 15, 14, ASCR_BACK, 1}},                               //CharAnim_UpAlt
	{2, (const u8[]){16, 17, 16, ASCR_BACK, 1}},  				   //CharAnim_Right
	{2, (const u8[]){18, 19, 18, ASCR_BACK, 1}},                               //CharAnim_RightAlt
	{2, (const u8[]){20, 21, 22, 21, ASCR_BACK, 1}},                           //CharAnim_Scream
};

//Spong2 character functions
void Char_Spong2_SetFrame(void *user, u8 frame)
{
	Char_Spong2 *this = (Char_Spong2*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_spong2_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Spong2_Tick(Character *character)
{
	Char_Spong2 *this = (Char_Spong2*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Spong2_SetFrame);
	Character_Draw(character, &this->tex, &char_spong2_frame[this->frame]);

	if (character-> animatable.anim == CharAnim_Up && stage.stage_id == StageId_1_3 && stage.song_step >= 760 && stage.song_step <= 790)
	character->set_anim(character, CharAnim_Scream);
}

void Char_Spong2_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Spong2_Free(Character *character)
{
	Char_Spong2 *this = (Char_Spong2*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Spong2_New(fixed_t x, fixed_t y)
{
	//Allocate spong2 object
	Char_Spong2 *this = Mem_Alloc(sizeof(Char_Spong2));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Spong2_New] Failed to allocate spong2 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Spong2_Tick;
	this->character.set_anim = Char_Spong2_SetAnim;
	this->character.free = Char_Spong2_Free;
	
	Animatable_Init(&this->character.animatable, char_spong2_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPONG2.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Spong2_ArcMain_Idle0
		"idle1.tim", //Spong2_ArcMain_Idle1
		"idle2.tim", //Spong2_ArcMain_Idle2
		"idle3.tim", //Spong2_ArcMain_Idle3
		"lefta0.tim",  //Spong2_ArcMain_LeftA0
		"lefta1.tim",  //Spong2_ArcMain_LeftA1
		"leftb0.tim",  //Spong2_ArcMain_LeftB0
		"leftb1.tim",  //Spong2_ArcMain_LeftB1
		"downa0.tim",  //Spong2_ArcMain_DownA0
		"downa1.tim",  //Spong2_ArcMain_DownA1
		"downb0.tim",  //Spong2_ArcMain_DownB0
		"downb1.tim",  //Spong2_ArcMain_DownB1
		"upa0.tim",    //Spong2_ArcMain_UpA0
		"upa1.tim",    //Spong2_ArcMain_UpA1
		"upb0.tim",    //Spong2_ArcMain_UpB0
		"upb1.tim",    //Spong2_ArcMain_UpB1
		"righta0.tim", //Spong2_ArcMain_RightA0
		"righta1.tim", //Spong2_ArcMain_RightA1
		"rightb0.tim", //Spong2_ArcMain_RightB0
		"rightb1.tim", //Spong2_ArcMain_RightB1
		"scream0.tim", //Spong2_ArcMain_Scream0
		"scream1.tim", //Spong2_ArcMain_Scream1
		"scream2.tim", //Spong2_ArcMain_Scream2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
