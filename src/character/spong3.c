/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "spong3.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Spong3 character structure
enum
{
	Spong3_ArcMain_Idle0,
	Spong3_ArcMain_Idle1,
	Spong3_ArcMain_Idle2,
	Spong3_ArcMain_Idle3,
	Spong3_ArcMain_Idle4,
	Spong3_ArcMain_Idle5,
	Spong3_ArcMain_Idle6,
	Spong3_ArcMain_Idle7,
	Spong3_ArcMain_Left0,
	Spong3_ArcMain_Left1,
	Spong3_ArcMain_Left2,
	Spong3_ArcMain_Left3,
	Spong3_ArcMain_Left4,
	Spong3_ArcMain_Down0,
	Spong3_ArcMain_Down1,
	Spong3_ArcMain_Down2,
	Spong3_ArcMain_Down3,
	Spong3_ArcMain_Down4,
	Spong3_ArcMain_Up0,
	Spong3_ArcMain_Up1,
	Spong3_ArcMain_Up2,
	Spong3_ArcMain_Up3,
	Spong3_ArcMain_Up4,
	Spong3_ArcMain_Right0,
	Spong3_ArcMain_Right1,
	Spong3_ArcMain_Right2,
	Spong3_ArcMain_Right3,
	Spong3_ArcMain_Right4,
	
	Spong3_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Spong3_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Spong3;

//Spong3 character definitions
static const CharFrame char_spong3_frame[] = {
	{Spong3_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 212}}, //0 idle 1
	{Spong3_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 212}}, //1 idle 2
	{Spong3_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 212}}, //2 idle 3
	{Spong3_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 212}}, //3 idle 4
	{Spong3_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 212}}, //4 idle 5
	{Spong3_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 212}}, //5 idle 6
	{Spong3_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 212}}, //6 idle 7
	{Spong3_ArcMain_Idle7, {  0,   0, 256, 256}, { 125, 212}}, //7 idle 8
	
	{Spong3_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 212}}, //8 left 1
	{Spong3_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 212}}, //9 left 2
	{Spong3_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 212}}, //10 left 3
	{Spong3_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 212}}, //11 left 4
	{Spong3_ArcMain_Left4, {  0,   0, 256, 256}, { 125, 212}}, //12 left 5
	
	{Spong3_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 212}}, //13 down 1
	{Spong3_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 212}}, //14 down 2
	{Spong3_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 212}}, //15 down 3
	{Spong3_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 212}}, //16 down 4
	{Spong3_ArcMain_Down4, {  0,   0, 256, 256}, { 125, 212}}, //17 down 5
	
	{Spong3_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 212}}, //18 up 1
	{Spong3_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 212}}, //19 up 2
	{Spong3_ArcMain_Up2, {   0,   0,  256, 256}, { 125, 212}}, //20 up 3
	{Spong3_ArcMain_Up3, {   0,   0,  256, 256}, { 125, 212}}, //21 up 4
	{Spong3_ArcMain_Up4, {   0,   0,  256, 256}, { 125, 212}}, //22 up 5
	
	{Spong3_ArcMain_Right0, {  0,   0, 256, 256}, { 104, 212}}, //23 right 1
	{Spong3_ArcMain_Right1, {  0,   0, 256, 256}, { 104, 212}}, //24 right 2
	{Spong3_ArcMain_Right2, {  0,   0, 256, 256}, { 104, 212}}, //25 right 3
	{Spong3_ArcMain_Right3, {  0,   0, 256, 256}, { 104, 212}}, //26 right 4
	{Spong3_ArcMain_Right4, {  0,   0, 256, 256}, { 104, 212}}, //27 right 5
};

static const Animation char_spong3_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},           //CharAnim_Idle
	{2, (const u8[]){ 8,  9, 10, 11, 12, ASCR_BACK, 1}},       		   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAlt
	{2, (const u8[]){13, 14, 15, 16, 17, ASCR_BACK, 1}},  		   	   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAlt
	{2, (const u8[]){18, 19, 20, 21, 22, ASCR_BACK, 1}},              	   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_UpAlt
	{2, (const u8[]){23, 24, 25, 26, 27, ASCR_BACK, 1}},  			   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAlt
};

//Spong3 character functions
void Char_Spong3_SetFrame(void *user, u8 frame)
{
	Char_Spong3 *this = (Char_Spong3*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_spong3_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Spong3_Tick(Character *character)
{
	Char_Spong3 *this = (Char_Spong3*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Spong3_SetFrame);
	Character_Draw(character, &this->tex, &char_spong3_frame[this->frame]);
}

void Char_Spong3_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Spong3_Free(Character *character)
{
	Char_Spong3 *this = (Char_Spong3*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Spong3_New(fixed_t x, fixed_t y)
{
	//Allocate spong3 object
	Char_Spong3 *this = Mem_Alloc(sizeof(Char_Spong3));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Spong3_New] Failed to allocate spong3 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Spong3_Tick;
	this->character.set_anim = Char_Spong3_SetAnim;
	this->character.free = Char_Spong3_Free;
	
	Animatable_Init(&this->character.animatable, char_spong3_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-75,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPONG3.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Spong3_ArcMain_Idle0
		"idle1.tim", //Spong3_ArcMain_Idle1
		"idle2.tim", //Spong3_ArcMain_Idle2
		"idle3.tim", //Spong3_ArcMain_Idle3
		"idle4.tim", //Spong3_ArcMain_Idle4
		"idle5.tim", //Spong3_ArcMain_Idle5
		"idle6.tim", //Spong3_ArcMain_Idle6
		"idle7.tim", //Spong3_ArcMain_Idle7
		"left0.tim",  //Spong3_ArcMain_Left0
		"left1.tim",  //Spong3_ArcMain_Left1
		"left2.tim",  //Spong3_ArcMain_Left2
		"left3.tim",  //Spong3_ArcMain_Left3
		"left4.tim",  //Spong3_ArcMain_Left4
		"down0.tim",  //Spong3_ArcMain_Down0
		"down1.tim",  //Spong3_ArcMain_Down1
		"down2.tim",  //Spong3_ArcMain_Down2
		"down3.tim",  //Spong3_ArcMain_Down3
		"down4.tim",  //Spong3_ArcMain_Down4
		"up0.tim",    //Spong3_ArcMain_Up0
		"up1.tim",    //Spong3_ArcMain_Up1
		"up2.tim",    //Spong3_ArcMain_Up2
		"up3.tim",    //Spong3_ArcMain_Up3
		"up4.tim",    //Spong3_ArcMain_Up4
		"right0.tim", //Spong3_ArcMain_Right0
		"right1.tim", //Spong3_ArcMain_Right1
		"right2.tim", //Spong3_ArcMain_Right2
		"right3.tim", //Spong3_ArcMain_Right3
		"right4.tim", //Spong3_ArcMain_Right4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
