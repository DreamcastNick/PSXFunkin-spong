/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "old3.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Old3 character structure
enum
{
	Old3_ArcMain_Idle0,
	Old3_ArcMain_Idle1,
	Old3_ArcMain_Idle2,
	Old3_ArcMain_Idle3,
	Old3_ArcMain_Idle4,
	Old3_ArcMain_Idle5,
	Old3_ArcMain_Idle6,
	Old3_ArcMain_Idle7,
	Old3_ArcMain_Idle8,
	Old3_ArcMain_Idle9,
	Old3_ArcMain_Idle10,
	Old3_ArcMain_Idle11,
	Old3_ArcMain_Idle12,
	Old3_ArcMain_Idle13,
	Old3_ArcMain_Idle14,
	Old3_ArcMain_Idle15,
	
	Old3_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Old3_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Old3;

//Old3 character definitions
static const CharFrame char_old3_frame[] = {
	{Old3_ArcMain_Idle0,  {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Old3_ArcMain_Idle1,  {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Old3_ArcMain_Idle2,  {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Old3_ArcMain_Idle3,  {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Old3_ArcMain_Idle4,  {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Old3_ArcMain_Idle5,  {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{Old3_ArcMain_Idle6,  {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	{Old3_ArcMain_Idle7,  {  0,   0, 256, 256}, { 125, 250}}, //7 idle 8
	{Old3_ArcMain_Idle8,  {  0,   0, 256, 256}, { 125, 250}}, //8 idle 9
	{Old3_ArcMain_Idle9,  {  0,   0, 256, 256}, { 125, 250}}, //9 idle 10
	{Old3_ArcMain_Idle10, {  0,   0, 256, 256}, { 125, 250}}, //10 idle 11
	{Old3_ArcMain_Idle11, {  0,   0, 256, 256}, { 125, 250}}, //11 idle 12
	{Old3_ArcMain_Idle12, {  0,   0, 256, 256}, { 125, 250}}, //12 idle 13
	{Old3_ArcMain_Idle13, {  0,   0, 256, 256}, { 125, 250}}, //13 idle 14
	{Old3_ArcMain_Idle14, {  0,   0, 256, 256}, { 125, 250}}, //14 idle 15
	{Old3_ArcMain_Idle15, {  0,   0, 256, 256}, { 125, 250}}, //15 idle 16

};

static const Animation char_old3_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, ASCR_REPEAT, 1}},           //CharAnim_Idle
	{2, (const u8[]){ 0, ASCR_BACK, 1}},       		   						   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},  		       				   			   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   				   //CharAnim_DownAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},              	   				   		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                				   //CharAnim_UpAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},  		   					   		   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             				   //CharAnim_RightAlt
};

//Old3 character functions
void Char_Old3_SetFrame(void *user, u8 frame)
{
	Char_Old3 *this = (Char_Old3*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_old3_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Old3_Tick(Character *character)
{
	Char_Old3 *this = (Char_Old3*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Old3_SetFrame);
	Character_Draw(character, &this->tex, &char_old3_frame[this->frame]);
}

void Char_Old3_SetAnim(Character *character, u8 anim)
{
	if (anim != CharAnim_Idle)
    	return;
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Old3_Free(Character *character)
{
	Char_Old3 *this = (Char_Old3*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Old3_New(fixed_t x, fixed_t y)
{
	//Allocate old3 object
	Char_Old3 *this = Mem_Alloc(sizeof(Char_Old3));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Old3_New] Failed to allocate old3 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Old3_Tick;
	this->character.set_anim = Char_Old3_SetAnim;
	this->character.free = Char_Old3_Free;
	
	Animatable_Init(&this->character.animatable, char_old3_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 10;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\OLD3.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Old3_ArcMain_Idle0
		"idle1.tim", //Old3_ArcMain_Idle1
		"idle2.tim", //Old3_ArcMain_Idle2
		"idle3.tim", //Old3_ArcMain_Idle3
		"idle4.tim", //Old3_ArcMain_Idle4
		"idle5.tim", //Old3_ArcMain_Idle5
		"idle6.tim", //Old3_ArcMain_Idle6
		"idle7.tim", //Old3_ArcMain_Idle7
		"idle8.tim", //Old3_ArcMain_Idle8
		"idle9.tim", //Old3_ArcMain_Idle9
		"idle10.tim", //Old3_ArcMain_Idle10
		"idle11.tim", //Old3_ArcMain_Idle11
		"idle12.tim", //Old3_ArcMain_Idle12
		"idle13.tim", //Old3_ArcMain_Idle13
		"idle14.tim", //Old3_ArcMain_Idle14
		"idle15.tim", //Old3_ArcMain_Idle15
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
