/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "old2.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Old2 character structure
enum
{
	Old2_ArcMain_Idle0,
	Old2_ArcMain_Idle1,
	Old2_ArcMain_Idle2,
	Old2_ArcMain_Idle3,
	Old2_ArcMain_Idle4,
	Old2_ArcMain_Idle5,
	Old2_ArcMain_Idle6,
	Old2_ArcMain_Idle7,
	Old2_ArcMain_Left0,
	Old2_ArcMain_Left1,
	Old2_ArcMain_Left2,
	Old2_ArcMain_Left3,
	Old2_ArcMain_Left4,
	Old2_ArcMain_Down0,
	Old2_ArcMain_Down1,
	Old2_ArcMain_Down2,
	Old2_ArcMain_Down3,
	Old2_ArcMain_Down4,
	Old2_ArcMain_Down5,
	Old2_ArcMain_Up0,
	Old2_ArcMain_Up1,
	Old2_ArcMain_Up2,
	Old2_ArcMain_Up3,
	Old2_ArcMain_Up4,
	Old2_ArcMain_Right0,
	Old2_ArcMain_Right1,
	Old2_ArcMain_Right2,
	Old2_ArcMain_Right3,
	
	Old2_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Old2_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Old2;

//Old2 character definitions
static const CharFrame char_old2_frame[] = {
	{Old2_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Old2_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Old2_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Old2_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Old2_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Old2_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{Old2_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	{Old2_ArcMain_Idle7, {  0,   0, 256, 256}, { 125, 250}}, //7 idle 8
	
	{Old2_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //8 left 1
	{Old2_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //9 left 2
	{Old2_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //10 left 3
	{Old2_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //11 left 4
	{Old2_ArcMain_Left4, {  0,   0, 256, 256}, { 125, 250}}, //12 left 5
	
	{Old2_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //13 down 1
	{Old2_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //14 down 2
	{Old2_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //15 down 3
	{Old2_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //16 down 4
	{Old2_ArcMain_Down4, {  0,   0, 256, 256}, { 125, 250}}, //17 down 5
	{Old2_ArcMain_Down5, {  0,   0, 256, 256}, { 125, 250}}, //18 down 6
	
	{Old2_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //19 up 1
	{Old2_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //20 up 2
	{Old2_ArcMain_Up2, {   0,   0,  256, 256}, { 125, 250}}, //21 up 3
	{Old2_ArcMain_Up3, {   0,   0,  256, 256}, { 125, 250}}, //22 up 4
	{Old2_ArcMain_Up4, {   0,   0,  256, 256}, { 125, 250}}, //23 up 5
	
	{Old2_ArcMain_Right0, {  0,   0, 256, 256}, { 104, 250}}, //24 right 1
	{Old2_ArcMain_Right1, {  0,   0, 256, 256}, { 104, 250}}, //25 right 2
	{Old2_ArcMain_Right2, {  0,   0, 256, 256}, { 104, 250}}, //26 right 3
	{Old2_ArcMain_Right3, {  0,   0, 256, 256}, { 104, 250}}, //27 right 4
};

static const Animation char_old2_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7, ASCR_BACK, 1}},           //CharAnim_Idle
	{2, (const u8[]){ 8,  9, 10, 11, 12, ASCR_BACK, 1}},       		   				   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   				   //CharAnim_LeftAlt
	{2, (const u8[]){13, 14, 15, 16, 17, 18, ASCR_BACK, 1}},  		       				   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   				   //CharAnim_DownAlt
	{2, (const u8[]){19, 20, 21, 22, 23, ASCR_BACK, 1}},              	   				   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                				   //CharAnim_UpAlt
	{2, (const u8[]){24, 25, 26, 27, ASCR_BACK, 1}},  		   					   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             				   //CharAnim_RightAlt
};

//Old2 character functions
void Char_Old2_SetFrame(void *user, u8 frame)
{
	Char_Old2 *this = (Char_Old2*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_old2_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Old2_Tick(Character *character)
{
	Char_Old2 *this = (Char_Old2*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Old2_SetFrame);
	Character_Draw(character, &this->tex, &char_old2_frame[this->frame]);
}

void Char_Old2_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Old2_Free(Character *character)
{
	Char_Old2 *this = (Char_Old2*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Old2_New(fixed_t x, fixed_t y)
{
	//Allocate old2 object
	Char_Old2 *this = Mem_Alloc(sizeof(Char_Old2));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Old2_New] Failed to allocate old2 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Old2_Tick;
	this->character.set_anim = Char_Old2_SetAnim;
	this->character.free = Char_Old2_Free;
	
	Animatable_Init(&this->character.animatable, char_old2_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 9;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\OLD2.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Old2_ArcMain_Idle0
		"idle1.tim", //Old2_ArcMain_Idle1
		"idle2.tim", //Old2_ArcMain_Idle2
		"idle3.tim", //Old2_ArcMain_Idle3
		"idle4.tim", //Old2_ArcMain_Idle4
		"idle5.tim", //Old2_ArcMain_Idle5
		"idle6.tim", //Old2_ArcMain_Idle6
		"idle7.tim", //Old2_ArcMain_Idle7
		"left0.tim",  //Old2_ArcMain_Left0
		"left1.tim",  //Old2_ArcMain_Left1
		"left2.tim",  //Old2_ArcMain_Left2
		"left3.tim",  //Old2_ArcMain_Left3
		"left4.tim",  //Old2_ArcMain_Left4
		"down0.tim",  //Old2_ArcMain_Down0
		"down1.tim",  //Old2_ArcMain_Down1
		"down2.tim",  //Old2_ArcMain_Down2
		"down3.tim",  //Old2_ArcMain_Down3
		"down4.tim",  //Old2_ArcMain_Down4
		"down5.tim",  //Old2_ArcMain_Down5
		"up0.tim",    //Old2_ArcMain_Up0
		"up1.tim",    //Old2_ArcMain_Up1
		"up2.tim",    //Old2_ArcMain_Up2
		"up3.tim",    //Old2_ArcMain_Up3
		"up4.tim",    //Old2_ArcMain_Up4
		"right0.tim", //Old2_ArcMain_Right0
		"right1.tim", //Old2_ArcMain_Right1
		"right2.tim", //Old2_ArcMain_Right2
		"right3.tim", //Old2_ArcMain_Right3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
