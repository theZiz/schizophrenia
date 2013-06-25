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

pPhysicsElement createPhysicsElement(Sint32 px,Sint32 py,Sint32 w,Sint32 h,
			int moveable,int moves,int gravitation,int superPower,pLevelObject levelObject)
{
	pPhysicsElement element = (pPhysicsElement)malloc(sizeof(tPhysicsElement));
	element->position.x = px;
	element->position.y = py;
	element->backupPosition.x = px;
	element->backupPosition.y = py;
	element->lastDirection = 1;
	element->w = w;
	element->h = h;
	element->speed.x = 0;
	element->speed.y = 0;
	element->gravitation = gravitation;
	element->permeability = 15; //mask 0b1111
	element->superPower = superPower;
	element->freeFallCounter = 0;
	element->moveable = moveable;
	element->moves = moves;
	element->levelObject = levelObject;
	if (levelObject)
	{
		levelObject->physicsElement = element;
		element->type = levelObject->type;
	}
	else
		element->type = UNKONWN;

	if (moveable || gravitation || moves)
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
			pPhysicsElement element = createPhysicsElement(x,y,SP_ONE,SP_ONE,0,0,0,0,NULL);
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
			int moves = 0;
			int gravitation = 0;
			int superPower = 0;
			switch (obj->type)
			{
				case TROPHIES: case GENERIC: case UNKONWN: case UNSELECTABLE: case LOGICGROUP:
					obj = obj->next;
					continue;
				case PLAYER: case NEGA: case BOX: case BUG:
					moveable = 1;
					moves = 1;
					gravitation = 1;
					break;
				case PLATFORM:
					moves = 1;
					superPower = 1;
					break;
			}
			pPhysicsElement element = createPhysicsElement(obj->x,obj->y,obj->w << SP_ACCURACY-5,obj->h << SP_ACCURACY-5,moveable,moves,gravitation,superPower,obj);
			switch (obj->type)
			{
				case SWITCH: case BUTTON: case DOOR: case COLLECTIBLE:
					element->permeability = 0;
					break;
			}
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
	
void doPhysics(void ( *setSpeed )( pPhysicsElement element ),
               int ( *gravFeedback )( pPhysicsCollision collision ),
               int ( *yFeedback )( pPhysicsCollision collision ),
               int ( *xFeedback )( pPhysicsCollision collision ), pLevel level)
{
	#ifdef COUNT_COLLISION
	collision_tests = 0;
	#endif
	//Settings every elements speed; Backup the position and killing
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		element->speed.x = 0;
		element->speed.y = 0;
		setSpeed(element);
		element->had_collision = 0;
		element->backupPosition.x = element->position.x;
		element->backupPosition.y = element->position.y;
		//DEBUG
		element->position.x += element->speed.x;
		element->position.y += element->speed.y;
		
		element->killed = 0;
		element = element->next;
	}
	while (element != firstMoveableElement);


	/////////////////////////
	// Step I: Gravitation //
	/////////////////////////
	//I: force impact
	/*element = firstMoveableElement;
	if (element)
	do
	{
		if (element->gravitation)
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
	while (element != firstMoveableElement);*/
	
	/////////////////////////
	// Step II: Y-Movement //
	/////////////////////////

	//Removing killed elements (and the level objects if exist)
	int somewhat_killed = 1;
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

	//Setting the free fall counter
	/*element = firstMoveableElement;
	if (element)
	do
	{
		if (element->gravitation)
		{
			if (element->had_collision & 8) //ground collision
			{
				if (element->freeFallCounter > 2)
					element->freeFallCounter = 1;
				else
					element->freeFallCounter = 0;
			}
		}
		element = element->next;
	}
	while (element != firstMoveableElement);*/
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
