/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "spong.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Spong character structure
enum
{
	Spong_ArcMain_Idle0,
	Spong_ArcMain_Idle1,
	Spong_ArcMain_Idle2,
	Spong_ArcMain_Idle3,
	Spong_ArcMain_Idle4,
	Spong_ArcMain_Idle5,
	Spong_ArcMain_LeftA0,
	Spong_ArcMain_LeftA1,
	Spong_ArcMain_LeftB0,
	Spong_ArcMain_LeftB1,
	Spong_ArcMain_DownA0,
	Spong_ArcMain_DownA1,
	Spong_ArcMain_DownB0,
	Spong_ArcMain_DownB1,
	Spong_ArcMain_UpA0,
	Spong_ArcMain_UpA1,
	Spong_ArcMain_UpB0,
	Spong_ArcMain_UpB1,
	Spong_ArcMain_RightA0,
	Spong_ArcMain_RightA1,
	Spong_ArcMain_RightB0,
	Spong_ArcMain_RightB1,
	
	Spong_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Spong_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Spong;

//Spong character definitions
static const CharFrame char_spong_frame[] = {
	{Spong_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Spong_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Spong_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Spong_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Spong_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Spong_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	
	{Spong_ArcMain_LeftA0, {  0,   0, 256, 256}, { 125, 250}}, //6 left a 1
	{Spong_ArcMain_LeftA1, {  0,   0, 256, 256}, { 125, 250}}, //7 left a 2
	{Spong_ArcMain_LeftB0, {  0,   0, 256, 256}, { 125, 250}}, //8 left b 1
	{Spong_ArcMain_LeftB1, {  0,   0, 256, 256}, { 125, 250}}, //9 left b 2
	
	{Spong_ArcMain_DownA0, {  0,   0, 256, 256}, { 125, 250}}, //10 down a 1
	{Spong_ArcMain_DownA1, {  0,   0, 256, 256}, { 125, 250}}, //11 down b 2
	{Spong_ArcMain_DownB0, {  0,   0, 256, 256}, { 125, 250}}, //12 down a 1
	{Spong_ArcMain_DownB1, {  0,   0, 256, 256}, { 125, 250}}, //13 down b 2
	
	{Spong_ArcMain_UpA0, {   0,   0,  256, 256}, { 125, 250}}, //14 up a 1
	{Spong_ArcMain_UpA1, {   0,   0,  256, 256}, { 125, 250}}, //15 up a 2
	{Spong_ArcMain_UpB0, {   0,   0,  256, 256}, { 125, 250}}, //16 up b 1
	{Spong_ArcMain_UpB1, {   0,   0,  256, 256}, { 125, 250}}, //17 up b 2
	
	{Spong_ArcMain_RightA0, {  0,   0, 256, 256}, { 104, 250}}, //18 right a 1
	{Spong_ArcMain_RightA1, {  0,   0, 256, 256}, { 104, 250}}, //19 right a 2
	{Spong_ArcMain_RightB0, {  0,   0, 256, 256}, { 104, 250}}, //20 right b 1
	{Spong_ArcMain_RightB1, {  0,   0, 256, 256}, { 104, 250}}, //21 right b 2
};

static const Animation char_spong_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  5,  5, ASCR_BACK, 1}},           //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  6, ASCR_BACK, 1}},       		           //CharAnim_Left
	{2, (const u8[]){ 8,  9,  8, ASCR_BACK, 1}},                     	   //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 10, ASCR_BACK, 1}},  		                   //CharAnim_Down
	{2, (const u8[]){12, 13, 12, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAlt
	{2, (const u8[]){14, 15, 14, ASCR_BACK, 1}},              		   //CharAnim_Up
	{2, (const u8[]){16, 17, 16, ASCR_BACK, 1}},                               //CharAnim_UpAlt
	{2, (const u8[]){18, 19, 18, ASCR_BACK, 1}},  				   //CharAnim_Right
	{2, (const u8[]){20, 21, 20, ASCR_BACK, 1}},                               //CharAnim_RightAlt
};

//Spong character functions
void Char_Spong_SetFrame(void *user, u8 frame)
{
	Char_Spong *this = (Char_Spong*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_spong_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Spong_Tick(Character *character)
{
	Char_Spong *this = (Char_Spong*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Spong_SetFrame);
	Character_Draw(character, &this->tex, &char_spong_frame[this->frame]);
}

void Char_Spong_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Spong_Free(Character *character)
{
	Char_Spong *this = (Char_Spong*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Spong_New(fixed_t x, fixed_t y)
{
	//Allocate spong object
	Char_Spong *this = Mem_Alloc(sizeof(Char_Spong));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Spong_New] Failed to allocate spong object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Spong_Tick;
	this->character.set_anim = Char_Spong_SetAnim;
	this->character.free = Char_Spong_Free;
	
	Animatable_Init(&this->character.animatable, char_spong_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPONG.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Spong_ArcMain_Idle0
		"idle1.tim", //Spong_ArcMain_Idle1
		"idle2.tim", //Spong_ArcMain_Idle2
		"idle3.tim", //Spong_ArcMain_Idle3
		"idle4.tim", //Spong_ArcMain_Idle4
		"idle5.tim", //Spong_ArcMain_Idle5
		"lefta0.tim",  //Spong_ArcMain_LeftA0
		"lefta1.tim",  //Spong_ArcMain_LeftA1
		"leftb0.tim",  //Spong_ArcMain_LeftB0
		"leftb1.tim",  //Spong_ArcMain_LeftB1
		"downa0.tim",  //Spong_ArcMain_DownA0
		"downa1.tim",  //Spong_ArcMain_DownA1
		"downb0.tim",  //Spong_ArcMain_DownB0
		"downb1.tim",  //Spong_ArcMain_DownB1
		"upa0.tim",    //Spong_ArcMain_UpA0
		"upa1.tim",    //Spong_ArcMain_UpA1
		"upb0.tim",    //Spong_ArcMain_UpB0
		"upb1.tim",    //Spong_ArcMain_UpB1
		"righta0.tim", //Spong_ArcMain_RightA0
		"righta1.tim", //Spong_ArcMain_RightA1
		"rightb0.tim", //Spong_ArcMain_RightB0
		"rightb1.tim", //Spong_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
