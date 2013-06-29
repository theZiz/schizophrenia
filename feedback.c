/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about our Files and Projects please mail us,
 Janek Schäfer (foxblock) , foxblock_gmail_com
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "feedback.h"

Sint32 in_jump = 0;
int can_jump = 1;
Sint32 last_run = 0;
pLevel level;

pLevel* getLevelOverPointer()
{
	return &level;
}

void do_control_stuff()
{
	//Controls and some Logic (more in feedback.c)
	if (spGetInput()->button[SP_BUTTON_R])
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		level->choosenPlayer = level->choosenPlayer->prev;
		in_jump = 0;
	}
	if (spGetInput()->button[SP_BUTTON_L])
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		level->choosenPlayer = level->choosenPlayer->next;
		in_jump = 0;
	}
}

void setSpeed( pPhysicsElement element )
{
	if (element->levelObject == NULL)
		return;
	if (element->type == PLATFORM)
	{
		if (element->levelObject->direction == 0)
		{
			element->speed.x += element->levelObject->speed.v1.x;
			element->speed.y += element->levelObject->speed.v1.y;
		}
		else
		{
			element->speed.x = element->levelObject->speed.v2.x;
			element->speed.y = element->levelObject->speed.v2.y;
		}	
	}
	else
	if (element->levelObject == level->choosenPlayer)
	{
		//Moving the player Y
		/*if ( spGetInput()->button[SP_BUTTON_LEFT] )
		{
			if ( in_jump == 0 && can_jump && element->grav_hit ) // start the jump
				in_jump = 1;
		}
		else
		{
			if ( in_jump >= JUMP_MIN_TIME && can_jump ) // start turn-around
			{
				in_jump = JUMP_UPWARDS_TIME;
				can_jump = 0;
			}
			if ( in_jump == 0 ) // allow another jump only after another press of the button
				can_jump = 1;
		}

		if (element->collision_at_position[1] && can_jump) //collision on top
		{
			// start turn-around
			in_jump = JUMP_UPWARDS_TIME;
			can_jump = 0;
		}
		else
		if (in_jump)
		{
			if (in_jump < JUMP_UPWARDS_TIME) // moving upwards
			{
				in_jump++;
				element->speed.y-=JUMP_FORCE;
				element->freeFallCounter = 0;
			}
			else
			if (in_jump < JUMP_END_TIME) // smooth turn-around (peak of jump)
			{
				in_jump++;
				element->speed.y-=GRAVITY_MAX*(JUMP_END_TIME-in_jump)/(JUMP_END_TIME-JUMP_UPWARDS_TIME);
				element->freeFallCounter = 0;
				can_jump = 0;
			}
			else // end jump
			{
				in_jump = 0;
				can_jump = 0;
			}
		}
		else
		if (element->grav_hit) //ground collision
		{
			in_jump = 0;
		}*/

		//Moving the player X
		if (spGetInput()->axis[0] < 0)
		{
			if (last_run >= 0)
				last_run = 0;
			last_run-=1;
			if (last_run > -(MAX_MOVEMENT_FORCE / MOVEMENT_ACCEL))
				element->speed.x = last_run*MOVEMENT_ACCEL;
			else
				element->speed.x = -MAX_MOVEMENT_FORCE;
		}
		else
		if (spGetInput()->axis[0] > 0)
		{
			if (last_run <= 0)
				last_run = 0;
			last_run+=1;
			if (last_run < (MAX_MOVEMENT_FORCE / MOVEMENT_ACCEL))
				element->speed.x = last_run*MOVEMENT_ACCEL;
			else
				element->speed.x = MAX_MOVEMENT_FORCE;
		}
		else
		{
			if (last_run < 0)
				last_run = 0;
			else
			if (last_run > 0)
				last_run = 0;
		}
	}
}
