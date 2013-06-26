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

#include "physics.h"

pPhysicsElement firstMoveableElement = NULL;
pPhysicsElement firstStaticElement = NULL;
pPhysicsElement *staticElementLookUp = NULL;
int staticElementLookUpX,staticElementLookUpY;

pPhysicsElement createPhysicsElement(Sint32 x,Sint32 y,Sint32 w,Sint32 h,
			int permeability,int floating,int moveable,int platform,int background, pLevelObject levelObject, int static_)
{
	pPhysicsElement element = (pPhysicsElement)malloc(sizeof(tPhysicsElement));
	element->position.x = x;
	element->position.y = y;
	element->backupPosition.x = x;
	element->backupPosition.y = y;
	element->speed.x = 0;
	element->speed.y = 0;
	element->newSpeed.x = 0;
	element->newSpeed.y = 0;
	element->w = w;
	element->h = h;
	element->permeability = permeability; //mask 0b1111
	element->floating = floating;
	element->moveable = moveable;
	element->platform = platform;
	element->background = background;
	element->killed = 0;
	element->freeFallCounter = 0;
	element->lastDirection = 1;
	element->grav_hit = 0;
	element->levelObject = levelObject;
	element->collision = NULL;
	if (levelObject)
	{
		levelObject->physicsElement = element;
		element->type = levelObject->type;
	}
	else
		element->type = UNKONWN;

	if (!static_)
	{
		element->is_static = 0;
		if (firstMoveableElement)
		{
			element->prev = firstMoveableElement->prev;
			firstMoveableElement->prev->next = element;
			element->next = firstMoveableElement;
			firstMoveableElement->prev = element;
		}
		else
		{
			element->next = element;
			element->prev = element;
			firstMoveableElement = element;
		}
	}
	else
	{
		element->is_static = 1;
		if (firstStaticElement)
		{
			element->prev = firstStaticElement->prev;
			firstStaticElement->prev->next = element;
			element->next = firstStaticElement;
			firstStaticElement->prev = element;
		}
		else
		{
			element->next = element;
			element->prev = element;
			firstStaticElement = element;
		}
	}
	return element;
}

void createPhysicsFromLevel(pLevel level)
{
	//Tiles
	staticElementLookUpX = level->layer.physics.width;
	staticElementLookUpY = level->layer.physics.height;
	staticElementLookUp = (pPhysicsElement*)malloc(sizeof(pPhysicsElement)*staticElementLookUpX*staticElementLookUpY);
	int i;
	for (i = 0; i < level->layer.physics.height * level->layer.physics.width; i++)
	{
		Sint32 x = spIntToFixed(i % level->layer.physics.width);
		Sint32 y = spIntToFixed(i / level->layer.physics.width);
		if (level->layer.physics.tile[i].nr)
		{
			pPhysicsElement element = createPhysicsElement(x,y,SP_ONE,SP_ONE,15,0,0,0,0,NULL,1);
			element->permeability = level->layer.physics.tile[i].nr;
			staticElementLookUp[i] = element;
		}
		else
			staticElementLookUp[i] = NULL;
	}
	//Objects
	pLevelObjectGroup group = level->firstObjectGroup;
	if (group)
	do
	{
		pLevelObject obj = group->firstObject;
		if (obj)
		do
		{
			int moveable = 0;
			int floating = 0;
			int platform = 0;
			int background = 0;
			switch (obj->type)
			{
				case TROPHIES: case GENERIC: case UNKONWN: case UNSELECTABLE: case LOGICGROUP:
					obj = obj->next;
					continue;
				case PLAYER: case NEGA: case BOX: case BUG:
					moveable = 1;
					break;
				case PLATFORM:
					floating = 1;
					platform = 1;
					break;
				case SWITCH: case BUTTON: case DOOR: case COLLECTIBLE:
					background = 1;
					floating = 1;
					break;
			}
			pPhysicsElement element = createPhysicsElement(obj->x,obj->y,obj->w << SP_ACCURACY-5,obj->h << SP_ACCURACY-5,15,floating,moveable,platform,background,obj,0);
			obj = obj->next;
		}
		while (obj != group->firstObject);
		group = group->next;
	}
	while (group != level->firstObjectGroup);

}

void clearPhysics()
{
	pPhysicsElement element = firstStaticElement;
	if (element)
	do
	{
		pPhysicsElement next = element->next;
		if (element->levelObject)
			element->levelObject->physicsElement = NULL;
		free(element);
		element = next;
	}
	while (element != firstStaticElement);
	firstStaticElement = NULL;
	element = firstMoveableElement;
	if (element)
	do
	{
		pPhysicsElement next = element->next;
		if (element->levelObject)
			element->levelObject->physicsElement = NULL;
		free(element);
		element = next;
	}
	while (element != firstMoveableElement);
	firstMoveableElement = NULL;
	if (staticElementLookUp)
		free(staticElementLookUp);
}

void updateLevelObjects()
{
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		if (element->levelObject)
		{
			element->levelObject->x = element->position.x;
			element->levelObject->y = element->position.y;
		}
		element = element->next;
	}
	while (element != firstMoveableElement);
}

#define COUNT_COLLISION
#ifdef COUNT_COLLISION
int collision_tests;
#endif

int check_for_collision_and_insert_holy_shit_long_function_name(pPhysicsElement e1,pPhysicsElement e2,int only_grav)
{
	#ifdef COUNT_COLLISION
	if (only_grav)
		collision_tests++;
	else
		collision_tests+=4;
	#endif
	if (only_grav)
	{
		if (e1->position.x + e1->w >= e2->position.x &&
				e1->position.x         <  e2->position.x + e2->w)
		{
			//e1 above e2
			if (e1->position.y + e1->h >= e2->position.y &&
					e1->position.y + e1->h <  e2->position.y + e2->h)
				e1->grav_hit = 1;

			//e2 above e1
			if (e2->position.y + e2->h >= e1->position.y &&
					e2->position.y + e2->h <  e1->position.y + e1->h)
				e2->grav_hit = 1;
		}
	}
	else
	{
		int position1 = 0;
		int position2 = 0;
		if (e1->position.x + e1->w >= e2->position.x &&
				e1->position.x         <  e2->position.x + e2->w)
		{
			//e1 above e2
			if (e1->position.y + e1->h >= e2->position.y &&
					e1->position.y + e1->h <  e2->position.y + e2->h)
			{
				e1->collision_at_position[3] = 1;
				position1 |= 8;
				e2->collision_at_position[1] = 1;
				position2 |= 2;
			}

			//e2 above e1
			if (e2->position.y + e2->h >= e1->position.y &&
					e2->position.y + e2->h <  e1->position.y + e1->h)
			{
				e2->collision_at_position[3] = 1;
				position2 |= 8;
				e1->collision_at_position[1] = 1;
				position1 |= 2;
			}
		}
		if (e1->position.y + e1->h >= e2->position.y &&
				e1->position.y         <  e2->position.y + e2->h)
		{
			//e1 left of e2
			if (e1->position.x + e1->w >= e2->position.x &&
					e1->position.x + e1->w <  e2->position.x + e2->w)
			{
				e1->collision_at_position[2] = 1;
				position1 |= 4;
				e2->collision_at_position[0] = 1;
				position2 |= 0;
			}

			//e2 left of e1
			if (e2->position.x + e2->w >= e1->position.x &&
					e2->position.x + e2->w <  e1->position.x + e1->w)
			{
				e2->collision_at_position[2] = 1;
				position2 |= 4;
				e1->collision_at_position[0] = 1;
				position1 |= 0;
			}
		}
		if (position1)
		{
			pPhysicsCollision collision;
			if (!e1->is_static)
			{
				collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollsision));
				collision->me = e1;
				collision->partner = e2;
				collision->position = position1;
				collision->next = e1->collision;
				e1->collision = collision;
			}
			if (!e2->is_static)
			{
				collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollsision));
				collision->me = e2;
				collision->partner = e1;
				collision->position = position2;
				collision->next = e2->collision;
				e2->collision = collision;
			}
		}
	}
}
	
void doPhysics(void ( *setSpeed )( pPhysicsElement element ),
               int ( *gravFeedback )( pPhysicsCollision collision ),
               int ( *yFeedback )( pPhysicsCollision collision ),
               int ( *xFeedback )( pPhysicsCollision collision ), pLevel level)
{
	#ifdef COUNT_COLLISION
	collision_tests = 0;
	#endif
	//Setting up every elements speed; Backup the position and killing
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		setSpeed(element);
		element->speed.x = element->newSpeed.x;
		element->speed.y = element->newSpeed.y;
		element->newSpeed.x = 0;
		element->newSpeed.y = 0;
		element->backupPosition.x = element->position.x;
		element->backupPosition.y = element->position.y;
		element->killed = 0;
		element = element->next;
		//Deleting all old collisions
		while (element->collision)
		{
			pPhysicsCollision next = element->collision->next;
			free(element->collision);
			element->collision = next;
		}
		int i;
		for (i = 0; i < 4; i++)
			element->collision_at_position[i] = 0;
		element->grav_hit = 0;
	}
	while (element != firstMoveableElement);


	/////////////////////////
	// Step I: Gravitation //
	/////////////////////////
	element = firstMoveableElement;
	if (element)
	do
	{
		if (!element->floating)
		{
			#if (GRAVITY_COUNTER > 0)
			if (element->freeFallCounter < GRAVITY_COUNTER)
				element->position.y += element->freeFallCounter * ( GRAVITY_MAX / GRAVITY_COUNTER );
			else
			#endif
				element->position.y += GRAVITY_MAX;
			element->freeFallCounter++;
		}
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	/////////////////////////////////////////////////////////
	// Testing for collision and saving in collision chain //
	/////////////////////////////////////////////////////////
	element = firstMoveableElement;
	if (element)
	do
	{
		pPhysicsElement partner = element->next;
		while (partner != firstMoveableElement)
		{
			check_for_collision_and_insert_holy_shit_long_function_name(element,partner,1);
			partner = partner->next;
		}
		partner = firstStaticElement;
		do
		{
			check_for_collision_and_insert_holy_shit_long_function_name(element,partner,1);
			partner = partner->next;
		}
		while (partner != firstStaticElement);
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	/////////////////////////////
	// Reaction to Gravitation //
	/////////////////////////////
	if (element)
	do
	{
		if (element->grav_hit)
			element->position.y = spIntToFixed(spFixedToInt(element->backupPosition.y*32+1))/32;
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	
	element = firstMoveableElement;
	if (element)
	do
	{
		element->position.x += element->speed.x;
		element->position.y += element->speed.y;
		element = element->next;
	}
	while (element != firstMoveableElement);

	///////////////////////////////////////////////////////////////
	// Removing killed elements (and the level objects if exist) //
	///////////////////////////////////////////////////////////////
	int somewhat_killed = 0;
	while (somewhat_killed)
	{
		somewhat_killed = 0;
		element = firstMoveableElement;
		if (element)
		do
		{
			pPhysicsElement next = element->next;
			if (element->killed)
			{
				if (element == element->next)
					firstMoveableElement = NULL;
				else
				{
					element->prev->next = element->next;
					element->next->prev = element->prev;
					if (firstMoveableElement == element)
						firstMoveableElement = element->next;
				}
				if (element->levelObject)
				{
					removeObject(element->levelObject,level);
				}
				free(element);
				somewhat_killed = 1;
				break;
			}
			element = next;
		}
		while (element != firstMoveableElement);
	}

	/////////////////////////////////////////////////////////
	// Testing for collision and saving in collision chain //
	/////////////////////////////////////////////////////////
	element = firstMoveableElement;
	if (element)
	do
	{
		pPhysicsElement partner = element-> next;
		while (partner != firstMoveableElement)
		{
			check_for_collision_and_insert_holy_shit_long_function_name(element,partner,0);
			partner = partner->next;
		}
		partner = firstStaticElement;
		do
		{
			check_for_collision_and_insert_holy_shit_long_function_name(element,partner,0);
			partner = partner->next;
		}
		while (partner != firstStaticElement);
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	///////////////////////////
	// Reaction to Collision //
	///////////////////////////
	element = firstMoveableElement;
	if (element)
	do
	{
		if ((element->collision_at_position[1] || element->collision_at_position[3]))
			element->position.y = element->backupPosition.y;
		if (element->collision_at_position[0] || element->collision_at_position[2])
			element->position.x = element->backupPosition.x;
		element = element->next;
	}
	while (element != firstMoveableElement);

	////////////////////////////////////
	// Reseting the Free Fall Counter //
	////////////////////////////////////
	element = firstMoveableElement;
	if (element)
	do
	{
		if (!element->floating)
		{
			if (element->grav_hit) //ground collision
				element->freeFallCounter = 0;
		}
		element = element->next;
	}
	while (element != firstMoveableElement);
}

#ifdef COUNT_COLLISION
int getCollisionCount()
{
	return collision_tests;
}
#else
int getCollisionCount()
{
	return 42;
}
#endif
