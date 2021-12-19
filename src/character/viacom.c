/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "viacom.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Viacom character structure
enum
{
	Viacom_ArcMain_Idle,
	Viacom_ArcMain_Left,
	Viacom_ArcMain_Down,
	Viacom_ArcMain_Up,
	Viacom_ArcMain_Right,
	
	Viacom_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Viacom_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Viacom;

//Viacom character definitions
static const CharFrame char_viacom_frame[] = {
	{Viacom_ArcMain_Idle, {  0,   0, 256, 256}, { 125, 212}}, //0 idle 1

	{Viacom_ArcMain_Left, {  0,   0, 256, 256}, { 125, 212}}, //1 left 1

	{Viacom_ArcMain_Down, {  0,   0, 256, 256}, { 125, 212}}, //2 down 1

	{Viacom_ArcMain_Up, {   0,   0,  256, 256}, { 125, 212}}, //3 up 1

	{Viacom_ArcMain_Right, {  0,   0, 256, 256}, { 125, 212}}, //4 right 1
};

static const Animation char_viacom_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}},           			   //CharAnim_Idle
	{2, (const u8[]){ 1, ASCR_BACK, 1}},       		           //CharAnim_Left
	{2, (const u8[]){ 1, ASCR_BACK, 1}},                     	   //CharAnim_LeftAlt
	{2, (const u8[]){ 2, ASCR_BACK, 1}},  		                   //CharAnim_Down
	{2, (const u8[]){ 2, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAlt
	{2, (const u8[]){ 3, ASCR_BACK, 1}},              		   //CharAnim_Up
	{2, (const u8[]){ 3, ASCR_BACK, 1}},                               //CharAnim_UpAlt
	{2, (const u8[]){ 4, ASCR_BACK, 1}},  				   //CharAnim_Right
	{2, (const u8[]){ 4, ASCR_BACK, 1}},                               //CharAnim_RightAlt
};

//Viacom character functions
void Char_Viacom_SetFrame(void *user, u8 frame)
{
	Char_Viacom *this = (Char_Viacom*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_viacom_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Viacom_Tick(Character *character)
{
	Char_Viacom *this = (Char_Viacom*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Viacom_SetFrame);
	Character_Draw(character, &this->tex, &char_viacom_frame[this->frame]);
}

void Char_Viacom_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Viacom_Free(Character *character)
{
	Char_Viacom *this = (Char_Viacom*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Viacom_New(fixed_t x, fixed_t y)
{
	//Allocate viacom object
	Char_Viacom *this = Mem_Alloc(sizeof(Char_Viacom));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Viacom_New] Failed to allocate viacom object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Viacom_Tick;
	this->character.set_anim = Char_Viacom_SetAnim;
	this->character.free = Char_Viacom_Free;
	
	Animatable_Init(&this->character.animatable, char_viacom_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 14;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-125,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\VIACOM.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",  //Viacom_ArcMain_Idle
		"left.tim",  //Viacom_ArcMain_Left
		"down.tim",  //Viacom_ArcMain_Down
		"up.tim",    //Viacom_ArcMain_Up
		"right.tim", //Viacom_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
