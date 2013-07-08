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
#include "feedback.h"

#define A_BIT 1024

pPhysicsElement firstMoveableElement = NULL;
pPhysicsElement firstStaticElement = NULL;
pPhysicsElement *staticElementLookUp = NULL;
int staticElementLookUpX,staticElementLookUpY;

pPhysicsElement createPhysicsElement(Sint32 x,Sint32 y,Sint32 w,Sint32 h,
			int permeability,int floating,int moveable,int platform,int background, int mover,pLevelObject levelObject, int static_)
{
	pPhysicsElement element = (pPhysicsElement)malloc(sizeof(tPhysicsElement));
	element->position.x = x;
	element->position.y = y;
	element->speed.x = 0;
	element->speed.y = 0;
	element->w = w;
	element->h = h;
	element->permeability = permeability; //mask 0b1111
	element->floating = floating;
	element->moveable = moveable;
	element->platform = platform;
	element->background = background;
	element->mover = mover;
	element->freeFallCounter = 0;
	element->player.can_jump = 0;
	element->player.in_jump = 0;
	element->player.last_run = 0;
	element->player.pushes = 0;
	element->had_collision_with_choosen_player = 0;
	element->levelObject = levelObject;
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

#define PIXEL_SIZE (SP_ONE/32)

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
			pPhysicsElement element = createPhysicsElement(x,y,SP_ONE,SP_ONE,level->layer.physics.tile[i].nr,0,0,0,0,0,NULL,1);
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
			int mover = 0;
			int w = obj->w << SP_ACCURACY-5;
			int h = obj->h << SP_ACCURACY-5;
			int x = obj->x;
			int y = obj->y;
			switch (obj->type)
			{
				case TROPHIES: case GENERIC: case UNKONWN: case UNSELECTABLE: case LOGICGROUP:
					obj = obj->next;
					continue;
				case PLAYER:
					mover = 1;
					x+=PIXEL_SIZE * 2;
					w-=PIXEL_SIZE * 4;
					break;
				case NEGA:
					x+=PIXEL_SIZE * 2;
					w-=PIXEL_SIZE * 4;
				case BUG:
					break;
				case BOX: 
 					moveable = 1;
					break;
				case PLATFORM:
					floating = 1;
					platform = 1;
					break;
				case BUTTON:
					x+=PIXEL_SIZE * 6;
					w-=PIXEL_SIZE * 12;
				case SWITCH:case DOOR: case COLLECTIBLE:
					background = 1;
					floating = 1;
					break;
			}
			pPhysicsElement element = createPhysicsElement(x,y,w,h,15,floating,moveable,platform,background,mover,obj,0);
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
			element->levelObject->x = element->position.x-((element->levelObject->w << SP_ACCURACY-5)-element->w)/2;
			element->levelObject->y = element->position.y-((element->levelObject->h << SP_ACCURACY-5)-element->h)/2;
		}
		element = element->next;
	}
	while (element != firstMoveableElement);
}

#define COUNT_COLLISION
#ifdef COUNT_COLLISION
int collision_tests;
#endif

static int check_X_collision(pPhysicsElement e1,pPhysicsElement e2,int backup) //-1 left, 0 no collision, 1 collision right as seen from e1
{
	#ifdef COUNT_COLLISION
	collision_tests+=2;
	#endif
	if (e1->position.y + e1->h > e2->position.y &&
	    e1->position.y         < e2->position.y + e2->h)
	{
		//e1 left of e2
		if ((e1->permeability & 4) &&
		    (e2->permeability & 1) &&
		    e1->speed.x > 0 && //movement to the right
		    !(backup + e1->w >  e2->position.x && //no collision in before
		      backup + e1->w <= e2->position.x + e2->w) && //no collision in before
		    e1->position.x + e1->w >  e2->position.x &&
		    e1->position.x + e1->w <= e2->position.x + e2->w)
			return -1;
		//e1 right of e2
		if ((e1->permeability & 1) &&
		    (e2->permeability & 4) &&
		    e1->speed.x < 0 && //movement to the left
		    !(e2->position.x + e2->w >  backup && //no collision in before
		      e2->position.x + e2->w <= backup + e1->w) && //no collision in before
		    e2->position.x + e2->w >  e1->position.x &&
		    e2->position.x + e2->w <= e1->position.x + e1->w)
		    return 1;
	}
	return 0;
}

static int check_Y_collision(pPhysicsElement e1,pPhysicsElement e2,int backup) //-1 top, 0 no collision, 1 collision bottom as seen from e1
{
	#ifdef COUNT_COLLISION
	collision_tests+=2;
	#endif
	if (e1->position.x + e1->w > e2->position.x &&
	    e1->position.x         < e2->position.x + e2->w)
	{
		//e1 above e2
		if ((e1->permeability & 8) &&
		    (e2->permeability & 2) &&
		    e1->speed.y > 0 && //movement down
		    !(backup + e1->h >  e2->position.y && //no collision in before
		      backup + e1->h <= e2->position.y + e2->h) && //no collision in before
		    e1->position.y + e1->h >  e2->position.y &&
		    e1->position.y + e1->h <= e2->position.y + e2->h)
			return -1;
		//e2 above e1
		if ((e1->permeability & 2) &&
		    (e2->permeability & 8) &&
		    e1->speed.y < 0 && //movement up
		    !(e2->position.y + e2->h >  backup && //no collision in before
		      e2->position.y + e2->h <= backup + e1->h) && //no collision in before
		    e2->position.y + e2->h >  e1->position.y &&
		    e2->position.y + e2->h <= e1->position.y + e1->h)
		    return 1;
	}
	return 0;
}

int check_background_collision(pPhysicsElement e1,pPhysicsElement e2)
{
	if (e1->position.x + e1->w > e2->position.x &&
	    e1->position.x         < e2->position.x + e2->w &&
	    e1->position.y + e1->h > e2->position.y &&
	    e1->position.y         < e2->position.y + e2->h)
	    return 1;
	return 0;
}

void movementAndCollision(pPhysicsElement element);

int collision_check_y(pPhysicsElement element,pPhysicsElement partner,int backupX,int backupY)
{
	int pos = check_Y_collision(element,partner,backupY);
	if (pos)
	{
		element->had_collision = 1;
		if (element->platform)
		{
			element->position.x = backupX;
			element->position.y = backupY;
		}
		else
		{
			int diff = backupY - element->position.y;
			if (pos ==-1) //element over partner
			{
				element->position.y = partner->position.y - element->h;
				if (!element->floating)
					element->freeFallCounter = 0;
			}
			else //element under partner
				element->position.y = partner->position.y + partner->h;
			int newDiff = backupY - element->position.y;
			if (diff*newDiff < 0 || //different signs
				abs(diff) < abs (newDiff) ) //distance is bigger!
				element->position.y = backupY;
		}
	}
	return pos;
}

int collision_check_x(pPhysicsElement element,pPhysicsElement partner,int backupX,int backupY)
{
	int pos = check_X_collision(element,partner,backupX);
	if (pos)
	{
		element->had_collision = 1;
		if (element->mover && partner->moveable)
		{
			partner->speed.x = element->speed.x/2;
			partner->speed.y = element->speed.y/2;
			movementAndCollision(partner);
			pos = check_X_collision(element,partner,backupX);
			if (pos == 0)
				return 0;
		}
		if (element->platform)
		{
			element->position.x = backupX;
			element->position.y = backupY;
		}
		else
		{
			int diff = backupX - element->position.x;
			if (pos ==-1) //element left of partner
				element->position.x = partner->position.x - element->w;
			else //element right of partner
				element->position.x = partner->position.x + partner->w;
			int newDiff = backupX - element->position.x;
			if (diff*newDiff < 0 || //different signs
				abs(diff) < abs (newDiff) ) //distance is bigger!
				element->position.x = backupX;
		}
	}
	return pos;
}

void seek_and_move_a_bit_above(pPhysicsElement element)
{
	pPhysicsElement partner = firstMoveableElement;
	do
	{
		if (!(partner->moveable || partner->mover) || element == partner)
		{
			partner = partner->next;
			continue;
		}
		#ifdef COUNT_COLLISION
		collision_tests++;
		#endif
		if (element->position.x + element->w >  partner->position.x &&
			element->position.x              <  partner->position.x + partner->w &&
		    partner->position.y + partner->h >  element->position.y - A_BIT &&
		    partner->position.y + partner->h <= element->position.y - A_BIT + element->h)
		{
			partner->speed.x = element->speed.x;
			partner->speed.y = element->speed.y;
			seek_and_move_a_bit_above(partner);
			movementAndCollision(partner);
		}
		partner = partner->next;
	}
	while (partner != firstMoveableElement);
}

static void check_all_collisions(pPhysicsElement element, int (*collision_check) (pPhysicsElement element,pPhysicsElement partner,int backupX,int backupY),int backupX,int backupY,void (*hitFeedback) ( pPhysicsElement element , int pos))
{
	//Collision with moveable stuff
	pPhysicsElement partner = firstMoveableElement;
	do
	{
		if (element == partner || partner->background)
		{
			partner = partner->next;
			continue;
		}
		int pos = collision_check(element,partner,backupX,backupY);
		if (pos)
			hitFeedback(element,pos);
		partner = partner->next;
	}
	while (partner != firstMoveableElement);
	//Collision with static stuff
	int x1 = spMin(spMax(spFixedToInt(element->position.x)-1,0),staticElementLookUpX);
	int y1 = spMin(spMax(spFixedToInt(element->position.y)-1,0),staticElementLookUpY);
	int x2 = spMin(spMax(spFixedToInt(element->position.x+element->w)+1,0),staticElementLookUpX);
	int y2 = spMin(spMax(spFixedToInt(element->position.y+element->h)+1,0),staticElementLookUpY);
	int x,y;
	for (x = x1; x < x2; x++)
		for (y = y1; y < y2; y++)
		{
			partner = staticElementLookUp[x+y*staticElementLookUpX];
			if (partner)
			{
				int pos = collision_check(element,partner,backupX,backupY);
				if (pos)
					hitFeedback(element,pos);
			}
		}
}

void movementAndCollision(pPhysicsElement element)
{
		//Movement + Backup
		int backupX = element->position.x;
		int backupY = element->position.y;
		if (element->platform)
			seek_and_move_a_bit_above(element);
		element->had_collision = 0;
		//Y
		element->position.y += element->speed.y;
		check_all_collisions(element,collision_check_y,backupX,backupY,yHit);
		//X
		if (!(element->platform && element->had_collision))
		{
			element->position.x += element->speed.x;
			check_all_collisions(element,collision_check_x,backupX,backupY,xHit);
		}	
		element->speed.x = 0;
		element->speed.y = 0;
}

int backgroundCollision(pPhysicsElement element, pLevel level)
{
	//Collision with moveable stuff
	element->had_collision_with_choosen_player = 0;
	int result = 0;
	pPhysicsElement partner = firstMoveableElement;
	do
	{
		if (element == partner || partner->background)
		{
			partner = partner->next;
			continue;
		}
		if (check_background_collision(element,partner))
		{
			result = 1;
			if (partner->levelObject == level->choosenPlayer)
				element->had_collision_with_choosen_player = 1;
		}
		partner = partner->next;
	}
	while (partner != firstMoveableElement);
	return result;
}
	
void doPhysics(pLevel level)
{
	#ifdef COUNT_COLLISION
	collision_tests = 0;
	#endif
	
	pPhysicsElement element = firstMoveableElement;
	if (element)
	do
	{
		element->speed.x = 0;
		element->speed.y = 0;
		setSpeed(element);
		//Gravitation
		if (!element->floating)
		{
			#if (GRAVITY_COUNTER > 0)
			if (element->freeFallCounter < GRAVITY_COUNTER)
				element->speed.y += element->freeFallCounter * ( GRAVITY_MAX / GRAVITY_COUNTER );
			else
			#endif
				element->speed.y += GRAVITY_MAX;
			element->freeFallCounter++;
		}
		//The physics itself:
		if (element->background)
			element->had_collision = backgroundCollision(element,level);
		else
		{
			movementAndCollision(element);
			if (spFixedToInt(element->position.y+element->h) >= staticElementLookUpY)
			{
				printf("Knife! Hehehehe...\n");
				//Removing Element
				if (element->levelObject)
					removeObject(element->levelObject,level);
				pPhysicsElement prev = element->prev;
				pPhysicsElement next = element->next;
				next->prev = prev;
				prev->next = next;
				if (element == firstMoveableElement)
					firstMoveableElement = next;
				free(element);
				element = prev;
			}
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

pPhysicsElement getFirstMoveableElement()
{
	return firstMoveableElement;
}
