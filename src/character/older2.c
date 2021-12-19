/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "older2.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Older2 character structure
enum
{
	Older2_ArcMain_Idle0,

	Older2_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Older2_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Older2;

//Older2 character definitions
static const CharFrame char_older2_frame[] = {
	{Older2_ArcMain_Idle0,  {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1

};

static const Animation char_older2_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, ASCR_REPEAT, 1}},           		   					   //CharAnim_Idle
	{2, (const u8[]){ 0, ASCR_BACK, 1}},       		   						   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},  		       				   			   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   				   //CharAnim_DownAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},              	   				   		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                				   //CharAnim_UpAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},  		   					   		   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             				   //CharAnim_RightAlt
};

//Older2 character functions
void Char_Older2_SetFrame(void *user, u8 frame)
{
	Char_Older2 *this = (Char_Older2*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_older2_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Older2_Tick(Character *character)
{
	Char_Older2 *this = (Char_Older2*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Older2_SetFrame);
	Character_Draw(character, &this->tex, &char_older2_frame[this->frame]);
}

void Char_Older2_SetAnim(Character *character, u8 anim)
{
	if (anim != CharAnim_Idle)
    	return;
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Older2_Free(Character *character)
{
	Char_Older2 *this = (Char_Older2*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Older2_New(fixed_t x, fixed_t y)
{
	//Allocate older2 object
	Char_Older2 *this = Mem_Alloc(sizeof(Char_Older2));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Older2_New] Failed to allocate older2 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Older2_Tick;
	this->character.set_anim = Char_Older2_SetAnim;
	this->character.free = Char_Older2_Free;
	
	Animatable_Init(&this->character.animatable, char_older2_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 12;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\OLDER2.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Older2_ArcMain_Idle0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
