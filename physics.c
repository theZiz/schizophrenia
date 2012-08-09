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

pPhysicsElement createPhysicsElement(Sint32 px,Sint32 py,Sint32 w,Sint32 h,
			int moveable,int gravitation,int superPower,pLevelObject levelObject)
{
	pPhysicsElement element = (pPhysicsElement)malloc(sizeof(tPhysicsElement));
	element->position.x = px;
	element->position.y = py;
	element->w = w;
	element->h = h;
	element->sqSize = spMulHigh(w,w)+spMulHigh(h,h);
	element->speed.x = 0;
	element->speed.y = 0;
	element->gravitation = gravitation;
	element->permeability = 0;
	element->superPower = superPower;
	element->freeFallCounter = 0;
	element->levelObject = levelObject;
	if (levelObject)
	{
		levelObject->physicsElement = element;
		element->type = levelObject->type;
	}
	else
		element->type = UNKONWN;

	if (moveable)
	{
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
		}
		firstMoveableElement = element;
	}
	else
	{
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
		}
		firstStaticElement = element;
	}
	return element;
}

void createPhysicsFromLevel(pLevel level)
{
	//Tiles
	int i;
	for (i = 0; i < level->layer.physics.height * level->layer.physics.width; i++)
	{
		Sint32 x = i % level->layer.physics.width;
		Sint32 y = i / level->layer.physics.width;
		if (level->layer.physics.tile[i].nr)
		{
			createPhysicsElement(x,y,SP_ONE,SP_ONE,0,0,0,NULL);
			//TODO: Semitransparenz!
		}
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
			int gravitation = 0;
			int superPower = 0;
			switch (obj->type)
			{
				case PLAYER: case NEGA: case BOX: case BUG:
					moveable = 1;
					gravitation = 1;
					break;
				case SWITCH: case BUTTON: case DOOR:
					gravitation = 1;
					break;
				case PLATFORM:
					moveable = 1;
					superPower = 1;
					break;
			}
			createPhysicsElement(obj->x,obj->y,obj->w << SP_ACCURACY-5,obj->h << SP_ACCURACY-5,moveable,gravitation,superPower,obj);
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

void doPhysics(int TimeForOneStep,void ( *setSpeed )( pPhysicsElement element ),
               void ( *gravFeedback )( pPhysicsCollision collision ),
               void ( *yFeedback )( pPhysicsCollision collision ),
               void ( *xFeedback )( pPhysicsCollision collision ))
{
	
}
