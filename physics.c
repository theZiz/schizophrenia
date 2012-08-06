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

pPhysicElement firstMoveableElement = NULL;
pPhysicElement firstStaticElement = NULL;

pPhysicElement createPhysicElement(Sint32 px,Sint32 py,Sint32 w,Sint32 h,int moveable,int gravitation,int superPower,pLevelObject levelObject)
{
	pPhysicElement element = (pPhysicElement)malloc(sizeof(tPhysicElement));
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
		levelObject->physicElement = element;
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

void createPhysicFromLevel(pLevel level)
{
	//Tiles
	int i;
	for (i = 0; i < level->layer.physic.height * level->layer.physic.width; i++)
	{

	}
	//Objects

}

void clearPhysic()
{
	pPhysicElement element = firstStaticElement;
	if (element)
	do
	{
		pPhysicElement next = element->next;
		if (element->levelObject)
			element->levelObject->physicElement = NULL;
		free(element);
		element = next;
	}
	while (element != firstStaticElement);
	element = firstMoveableElement;
	if (element)
	do
	{
		pPhysicElement next = element->next;
		if (element->levelObject)
			element->levelObject->physicElement = NULL;
		free(element);
		element = next;
	}
	while (element != firstMoveableElement);
}
