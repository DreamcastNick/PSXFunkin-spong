/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "billy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Billy character structure
enum
{
	Billy_ArcMain_Idle0,
	Billy_ArcMain_Idle1,
	Billy_ArcMain_Idle2,
	Billy_ArcMain_Idle3,
	Billy_ArcMain_Idle4,
	Billy_ArcMain_Idle5,
	Billy_ArcMain_Left0,
	Billy_ArcMain_Left1,
	Billy_ArcMain_Down0,
	Billy_ArcMain_Down1,
	Billy_ArcMain_Up0,
	Billy_ArcMain_Up1,
	Billy_ArcMain_Right0,
	Billy_ArcMain_Right1,
	
	Billy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Billy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Billy;

//Billy character definitions
static const CharFrame char_billy_frame[] = {
	{Billy_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Billy_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Billy_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Billy_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Billy_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Billy_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	
	{Billy_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //6 left 1
	{Billy_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //7 left 2
	
	{Billy_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //8 down 1
	{Billy_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //9 down 2
	
	{Billy_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //10 up 1
	{Billy_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //11 up 2
	
	{Billy_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //12 right 1
	{Billy_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //13 right 2
};

static const Animation char_billy_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  5,  5, ASCR_BACK, 1}},           //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  6, ASCR_BACK, 1}},       		           //CharAnim_Left
	{2, (const u8[]){ 6,  7,  6, ASCR_BACK, 1}},                     	   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9,  8, ASCR_BACK, 1}},  		                   //CharAnim_Down
	{2, (const u8[]){ 8,  9,  8, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, 10, ASCR_BACK, 1}},              		   //CharAnim_Up
	{2, (const u8[]){10, 11, 10, ASCR_BACK, 1}},                               //CharAnim_UpAlt
	{2, (const u8[]){12, 13, 12, ASCR_BACK, 1}},  				   //CharAnim_Right
	{2, (const u8[]){12, 13, 12, ASCR_BACK, 1}},                               //CharAnim_RightAlt
};

//Billy character functions
void Char_Billy_SetFrame(void *user, u8 frame)
{
	Char_Billy *this = (Char_Billy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_billy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Billy_Tick(Character *character)
{
	Char_Billy *this = (Char_Billy*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Billy_SetFrame);
	Character_Draw(character, &this->tex, &char_billy_frame[this->frame]);
}

void Char_Billy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Billy_Free(Character *character)
{
	Char_Billy *this = (Char_Billy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Billy_New(fixed_t x, fixed_t y)
{
	//Allocate billy object
	Char_Billy *this = Mem_Alloc(sizeof(Char_Billy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Billy_New] Failed to allocate billy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Billy_Tick;
	this->character.set_anim = Char_Billy_SetAnim;
	this->character.free = Char_Billy_Free;
	
	Animatable_Init(&this->character.animatable, char_billy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 16;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BILLY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Billy_ArcMain_Idle0
		"idle1.tim",  //Billy_ArcMain_Idle1
		"idle2.tim",  //Billy_ArcMain_Idle2
		"idle3.tim",  //Billy_ArcMain_Idle3
		"idle4.tim",  //Billy_ArcMain_Idle4
		"idle5.tim",  //Billy_ArcMain_Idle5
		"left0.tim",  //Billy_ArcMain_Left0
		"left1.tim",  //Billy_ArcMain_Left1
		"down0.tim",  //Billy_ArcMain_Down0
		"down1.tim",  //Billy_ArcMain_Down1
		"up0.tim",    //Billy_ArcMain_Up0
		"up1.tim",    //Billy_ArcMain_Up1
		"right0.tim", //Billy_ArcMain_Right0
		"right1.tim", //Billy_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
