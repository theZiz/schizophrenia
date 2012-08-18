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
			element->speed.x = element->levelObject->speed.v1.x;
			element->speed.y = element->levelObject->speed.v1.y;
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
		if ( spGetInput()->button[SP_BUTTON_LEFT] )
		{
			if ( in_jump == 0 && can_jump && (element->had_collision & 8) ) // start the jump
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

		if (element->had_collision & 2 && can_jump) //collision on top
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
		if (element->had_collision & 8) //ground collision
		{
			in_jump = 0;
		}

		//Moving the player X
		if (spGetInput()->axis[0] < 0)
		{
			if (in_jump || element->freeFallCounter)
				spSelectSprite(level->choosenPlayer->animation,"jump left");
			else
				spSelectSprite(level->choosenPlayer->animation,"run left");
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
			if (in_jump || element->freeFallCounter)
				spSelectSprite(level->choosenPlayer->animation,"jump right");
			else
				spSelectSprite(level->choosenPlayer->animation,"run right");
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
			if (element->lastDirection < 0)
			{
				if (in_jump || element->freeFallCounter)
					spSelectSprite(element->levelObject->animation,"jump left");
				else
					spSelectSprite(element->levelObject->animation,"stand left");
			}
			else
			if (element->lastDirection > 0)
			{
				if (in_jump || element->freeFallCounter)
					spSelectSprite(element->levelObject->animation,"jump right");
				else
					spSelectSprite(element->levelObject->animation,"stand right");
			}
			if (last_run < 0)
				last_run = 0;
			else
			if (last_run > 0)
				last_run = 0;
		}
	}
	//Setting the correct sprite
	else
	if (element->levelObject->type == PLAYER)
	{
		if (element->lastDirection < 0)
		{
			if (element->freeFallCounter)
				spSelectSprite(element->levelObject->animation,"jump left");
			else
				spSelectSprite(element->levelObject->animation,"stand left");
		}
		else
		if (element->lastDirection > 0)
		{
			if (element->freeFallCounter)
				spSelectSprite(element->levelObject->animation,"jump right");
			else
				spSelectSprite(element->levelObject->animation,"stand right");
		}
	}
}

void removeCollision( pPhysicsCollision collision )
{
	if (collision != collision->next)
	{
		collision->prev->next = collision->next;
		collision->next->prev = collision->prev;
	}
	free(collision);
}

int gravFeedback( pPhysicsCollision collision )
{
	if (collision->element[0]->type == PLAYER && collision->hitPosition[0] == 2) //TOP hit on player
		collision->element[0]->killed = 1;
	if (collision->element[1]->type == PLAYER && collision->hitPosition[1] == 2) //TOP hit on player
		collision->element[1]->killed = 1;
	return 0;
}

int has_unmoveable_child_in_direction(pPhysicsElement element,int direction)
{
	if (element->moveable == 0)
		return 1;
	pPhysicsCollisionChain chain = element->collisionChain[direction];
	while (chain)
	{
		if (has_unmoveable_child_in_direction(chain->element,direction))
			return 1;
		chain = chain->next;
	}
	if (direction == 3)
	{
		chain = element->collisionChain[5];
		while (chain)
		{
			if (has_unmoveable_child_in_direction(chain->element,5))
				return 1;
			chain = chain->next;
		}
	}
	return 0;
}

int yFeedback( pPhysicsCollision collision )
{
	if (collision->element[0]->type == PLATFORM)
	{
		if ((collision->element[0]->speed.y > 0 &&
		     collision->hitPosition[0] == 8 &&
		     has_unmoveable_child_in_direction(collision->element[1],3)) || //Moving down and collision on downside
				(collision->element[0]->speed.y < 0 &&
		     collision->hitPosition[0] == 2 && 
		     has_unmoveable_child_in_direction(collision->element[1],1))) //Moving up and collision on upside
		{
			collision->element[0]->levelObject->direction = 1 - collision->element[0]->levelObject->direction;
			collision->element[0]->speed.x = 0;
			collision->element[0]->speed.y = 0;
			collision->element[0]->position.x = collision->element[0]->backupPosition.x;
			collision->element[0]->position.y = collision->element[0]->backupPosition.y;
		}
	}
	if (collision->element[1]->type == PLATFORM)
	{
		if ((collision->element[1]->speed.y > 0 &&
		     collision->hitPosition[1] == 8 &&
		     has_unmoveable_child_in_direction(collision->element[0],3)) || //Moving down and collision on downside
				(collision->element[1]->speed.y < 0 &&
		     collision->hitPosition[1] == 2 && 
		     has_unmoveable_child_in_direction(collision->element[0],1))) //Moving up and collision on upside
		{
			collision->element[1]->levelObject->direction = 1 - collision->element[1]->levelObject->direction;
			collision->element[1]->speed.x = 0;
			collision->element[1]->speed.y = 0;
			collision->element[1]->position.x = collision->element[1]->backupPosition.x;
			collision->element[1]->position.y = collision->element[1]->backupPosition.y;
		}
	}
	return 0;
}

int xFeedback( pPhysicsCollision collision )
{
	if (collision->element[0]->type == PLATFORM)
	{
		if ((collision->element[0]->speed.x > 0 &&
		     collision->hitPosition[0] == 4 &&
		     has_unmoveable_child_in_direction(collision->element[1],2)) || //Moving right and collision on rightside
				(collision->element[0]->speed.x < 0 &&
		     collision->hitPosition[0] == 1 && 
		     has_unmoveable_child_in_direction(collision->element[1],0))) //Moving left and collision on leftside
		{
			collision->element[0]->levelObject->direction = 1 - collision->element[0]->levelObject->direction;
			collision->element[0]->speed.x = 0;
			collision->element[0]->speed.y = 0;
			collision->element[0]->position.x = collision->element[0]->backupPosition.x;
			collision->element[0]->position.y = collision->element[0]->backupPosition.y;
		}
	}
	if (collision->element[1]->type == PLATFORM)
	{
		if ((collision->element[1]->speed.x > 0 &&
		     collision->hitPosition[1] == 4 &&
		     has_unmoveable_child_in_direction(collision->element[0],2)) || //Moving right and collision on rightside
				(collision->element[1]->speed.x < 0 &&
		     collision->hitPosition[1] == 1 && 
		     has_unmoveable_child_in_direction(collision->element[0],0))) //Moving left and collision on leftside
		{
			collision->element[1]->levelObject->direction = 1 - collision->element[1]->levelObject->direction;
			collision->element[1]->speed.x = 0;
			collision->element[1]->speed.y = 0;
			collision->element[1]->position.x = collision->element[1]->backupPosition.x;
			collision->element[1]->position.y = collision->element[1]->backupPosition.y;
		}
	}
	return 0;
}
