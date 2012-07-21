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

#include "level.h"

pLevelObjectGroup createGroup(pLevelObjectGroup *firstGroup, LevelObjectGroupType type)
{
	pLevelObjectGroup group = (pLevelObjectGroup)malloc(sizeof(tLevelObjectGroup));
	group->type = type;
	group->firstObject = NULL;
	if (*firstGroup)
	{
		(*firstGroup)->prev->next = group;
		group->prev = (*firstGroup)->prev;
		group->next = (*firstGroup);
		(*firstGroup)->prev = group;
	}
	else
	{
		group->next = group;
		group->prev = group;
	}
	(*firstGroup) = group;
	return group;
}

pLevelObject createObject(pLevelObjectGroup group,LevelObjectType type)
{
	pLevelObject obj = (pLevelObject)malloc(sizeof(tLevelObject));
	obj->type = type;
	obj->animation = NULL;
	if (group->firstObject)
	{
		group->firstObject->prev->next = obj;
		obj->prev = group->firstObject->prev;
		obj->next = group->firstObject;
		group->firstObject->prev = obj;
	}
	else
	{
		obj->next = obj;
		obj->prev = obj;
	}
	group->firstObject = obj;
	return obj;
}

void allocLayer(pLayer layer,int width, int height)
{
	layer->tile = (spSpritePointer*)malloc(sizeof(spSpritePointer)*width*height);
	layer->width = width;
	layer->height = height;
}

pLevel loadLevel(char* filename)
{
	pLevel level = (pLevel)malloc(sizeof(tLevel));
	level->layer.physic.tile = NULL;
	level->layer.background.tile = NULL;
	level->layer.player.tile = NULL;
	level->layer.foreground.tile = NULL;
	//TODO: Implement :P
	return level;
}

void deleteLevel(pLevel level)
{
	if (level->layer.physic.tile)
		free(level->layer.physic.tile);
	if (level->layer.background.tile)
		free(level->layer.background.tile);
	if (level->layer.player.tile)
		free(level->layer.player.tile);
	if (level->layer.foreground.tile)
		free(level->layer.foreground.tile);
	
}
