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
 Janek Sch�fer (foxblock) , foxblock_gmail_com
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
	element->permeability = 15;
	element->superPower = superPower;
	element->freeFallCounter = 0;
	element->moveable = moveable;
	element->levelObject = levelObject;
	if (levelObject)
	{
		levelObject->physicsElement = element;
		element->type = levelObject->type;
	}
	else
		element->type = UNKONWN;

	if (moveable || gravitation)
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
		Sint32 x = i % level->layer.physics.width << SP_ACCURACY;
		Sint32 y = i / level->layer.physics.width << SP_ACCURACY;
		if (level->layer.physics.tile[i].nr)
		{
			pPhysicsElement element = createPhysicsElement(x,y,SP_ONE,SP_ONE,0,0,0,NULL);
			element->permeability = level->layer.physics.tile[i].nr;
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
				case TROPHIES: case GENERIC: case UNKONWN: case UNSELECTABLE: case LOGICGROUP:
					obj = obj->next;
					continue;
				case PLAYER: case NEGA: case BOX: case BUG:
					moveable = 1;
					gravitation = 1;
					break;
				case SWITCH: case BUTTON: case DOOR:
					gravitation = 0;
					break;
				case PLATFORM:
					moveable = 1;
					superPower = 1;
					break;
			}
			pPhysicsElement element = createPhysicsElement(obj->x,obj->y,obj->w << SP_ACCURACY-5,obj->h << SP_ACCURACY-5,moveable,gravitation,superPower,obj);
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

pPhysicsCollision getYCollision(pPhysicsElement element)
{
	pPhysicsCollision firstCollision = NULL;
	pPhysicsElement partner = firstStaticElement;
	while (partner != firstMoveableElement)
	{
		if (element->position.x >= partner->position.x && element->position.x < partner->position.x + partner->w ||
			  partner->position.x >= element->position.x && partner->position.x < element->position.x + element->w)
		{
			pPhysicsCollision collision = NULL;
			//partner over element
			if (element->position.y >= partner->position.y && element->position.y < partner->position.y + partner->h)
			{
				if ((element->permeability & 2) && (partner->permeability & 8))
				{
					collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollision));
					collision->element[0] = element;
					collision->hitPosition[0] = 2; //UP
					collision->element[1] = partner;
					collision->hitPosition[1] = 8; //DOWN
				}
			}
			else
			if (partner->position.y >= element->position.y && partner->position.y < element->position.y + element->h)
			{
				if ((element->permeability & 8) && (partner->permeability & 2))
				{
					collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollision));
					collision->element[0] = element;
					collision->hitPosition[0] = 8; //DOWN
					collision->element[1] = partner;
					collision->hitPosition[1] = 2; //UP
				}
			}
			if (collision)
			{
				if (firstCollision)
				{
					collision->next = firstCollision->next;
					collision->prev = firstCollision->prev;
					firstCollision->prev->next = collision;
					firstCollision->next->prev = collision;
				}
				else
				{
					collision->next = collision;
					collision->prev = collision;
				}
				firstCollision = collision;
			}
		}
		partner = partner->next;
		if (partner == firstStaticElement)
			partner = element->next;
	}
	return firstCollision;
}

pPhysicsCollision getYCollisionChain()
{
	pPhysicsCollision firstCollision = NULL;
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		if (element->gravitation)
		{
			//Searching the possible collision partners:
			pPhysicsCollision first = getYCollision(element);
			if (first)
			{
				pPhysicsCollision last = first->prev;
				if (firstCollision)
				{
					firstCollision->prev->next = first;
					first->prev = firstCollision->prev;
					last->next = firstCollision;
					firstCollision->prev = last;
					firstCollision = first;
				}
				else
					firstCollision = first;
			}
		}
		element = element->next;
	}
	while (element != firstMoveableElement);	
	return firstCollision;
}

pPhysicsCollision getXCollision(pPhysicsElement element)
{
	pPhysicsCollision firstCollision = NULL;
	pPhysicsElement partner = firstStaticElement;
	while (partner != firstMoveableElement)
	{
		if (element->position.y >= partner->position.y && element->position.y < partner->position.y + partner->h ||
			  partner->position.y >= element->position.y && partner->position.y < element->position.y + element->h)
		{
			pPhysicsCollision collision = NULL;
			//partner left of element
			if (element->position.x >= partner->position.x && element->position.x < partner->position.x + partner->w)
			{
				if ((element->permeability & 1) && (partner->permeability & 4))
				{
					collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollision));
					collision->element[0] = element;
					collision->hitPosition[0] = 1; //LEFT
					collision->element[1] = partner;
					collision->hitPosition[1] = 4; //RIGHT
				}
			}
			else
			if (partner->position.x >= element->position.x && partner->position.x < element->position.x + element->w)
			{
				if ((element->permeability & 4) && (partner->permeability & 1))
				{
					collision = (pPhysicsCollision)malloc(sizeof(tPhysicsCollision));
					collision->element[0] = element;
					collision->hitPosition[0] = 4; //RIGHT
					collision->element[1] = partner;
					collision->hitPosition[1] = 1; //LEFT
				}
			}
			if (collision)
			{
				if (firstCollision)
				{
					collision->next = firstCollision->next;
					collision->prev = firstCollision->prev;
					firstCollision->prev->next = collision;
					firstCollision->next->prev = collision;
				}
				else
				{
					collision->next = collision;
					collision->prev = collision;
				}
				firstCollision = collision;
			}
		}
		partner = partner->next;
		if (partner == firstStaticElement)
			partner = element->next;
	}
	return firstCollision;
}

pPhysicsCollision getXCollisionChain()
{
	pPhysicsCollision firstCollision = NULL;
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		if (element->gravitation)
		{
			//Searching the possible collision partners:
			pPhysicsCollision first = getXCollision(element);
			if (first)
			{
				pPhysicsCollision last = first->prev;
				if (firstCollision)
				{
					firstCollision->prev->next = first;
					first->prev = firstCollision->prev;
					last->next = firstCollision;
					firstCollision->prev = last;
					firstCollision = first;
				}
				else
					firstCollision = first;
			}
		}
		element = element->next;
	}
	while (element != firstMoveableElement);	
	return firstCollision;
}

void clearCollisionChain(pPhysicsCollision firstCollision)
{
	pPhysicsCollision collision = firstCollision;
	if (collision)
	do
	{
		pPhysicsCollision next = collision->next;
		free(collision);
		collision = next;
	}
	while (collision != firstCollision);
}

void doPhysics(int TimeForOneStep,void ( *setSpeed )( pPhysicsElement element ),
               int ( *gravFeedback )( pPhysicsCollision collision ),
               int ( *yFeedback )( pPhysicsCollision collision ),
               int ( *xFeedback )( pPhysicsCollision collision ), pLevel level)
{
	//Settings every elements speed.
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		setSpeed(element);
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	//Backup the position and killing
	element = firstMoveableElement;
	if (element)
	do
	{
		element->backupPosition.x = element->position.x;
		element->backupPosition.y = element->position.y;
		element->killed = 0;
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	/////////////////////////
	// Step I: Gravitation //
	/////////////////////////
	
	//I: force impact
	element = firstMoveableElement;
	if (element)
	do
	{
		if (element->gravitation)
			element->position.y += 128; //TODO: real gravitation!
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	//I: collision detecting
	pPhysicsCollision firstCollision = getYCollisionChain();
	
	//I: external collision handling
	pPhysicsCollision collision = firstCollision;
	if (collision)
	do
	{
		pPhysicsCollision next = collision->next; //Saving because maybe it will be freeed by the feedback function
		if (gravFeedback(collision) && collision == next) //deleted AND only one collision
		{
			firstCollision = NULL;
			break;
		}
		collision = next;
	}
	while (collision != firstCollision);	
	
	//I: internal collision handling
	collision = firstCollision;
	if (collision)
	do
	{
		if (collision->hitPosition[0] == 8) //DOWN
			collision->element[0]->position.y = collision->element[0]->backupPosition.y;
		if (collision->hitPosition[1] == 8) //DOWN
			collision->element[1]->position.y = collision->element[1]->backupPosition.y;
		collision = collision->next;
	}
	while (collision != firstCollision);	

	//I: Removing killed elements (and the level objects if exist)	
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
	
	//I: Removing the chain
	clearCollisionChain(firstCollision);
	
	/////////////////////////
	// Step II: Y-Movement //
	/////////////////////////
	
	
	//////////////////////////
	// Step III: X-Movement //
	//////////////////////////
	
	//III: force impact
	element = firstMoveableElement;
	if (element)
	do
	{
		if (element->moveable)
			element->position.x += element->speed.x;
		element = element->next;
	}
	while (element != firstMoveableElement);
	
	//III: collision detecting
	firstCollision = getXCollisionChain();
	
	//III: external collision handling
	collision = firstCollision;
	if (collision)
	do
	{
		pPhysicsCollision next = collision->next; //Saving because maybe it will be freeed by the feedback function
		if (xFeedback(collision) && collision == next) //deleted AND only one collision
		{
			firstCollision = NULL;
			break;
		}
		collision = next;
	}
	while (collision != firstCollision);	
	
	//III: internal collision handling
	/*collision = firstCollision;
	if (collision)
	do
	{
		if (collision->hitPosition[0] == 8) //DOWN
			collision->element[0]->position.y = collision->element[0]->backupPosition.y;
		if (collision->hitPosition[1] == 8) //DOWN
			collision->element[1]->position.y = collision->element[1]->backupPosition.y;
		collision = collision->next;
	}
	while (collision != firstCollision);	*/

	//III: Removing killed elements (and the level objects if exist)	
	somewhat_killed = 1;
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
	
	//III: Removing the chain
	clearCollisionChain(firstCollision);
	
	//Reseting every elements speed.
	element = firstMoveableElement;
	if (element)
	do
	{
		element->speed.x = 0;
		element->speed.y = 0;
		element = element->next;
	}
	while (element != firstMoveableElement);	
}