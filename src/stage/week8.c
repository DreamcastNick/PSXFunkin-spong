/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week8.h"

#include "../mem.h"
#include "../archive.h"

//Week 8 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background
} Back_Week8;

//Week 8 background functions
void Back_Week8_DrawBG(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw background
	fx = stage.camera.x;
	fy = stage.camera.y;

	
	RECT back_src = {0, 0, 256, 256};
	RECT_FIXED back_dst = {
		FIXED_DEC(-295,1) - fx,
		FIXED_DEC(-260,1) - fy,
		FIXED_DEC(593,1),
		FIXED_DEC(407,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
}

void Back_Week8_Free(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week8_New(void)
{
	//Allocate background structure
	Back_Week8 *this = (Back_Week8*)Mem_Alloc(sizeof(Back_Week8));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week8_DrawBG;
	this->back.free = Back_Week8_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK8\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
