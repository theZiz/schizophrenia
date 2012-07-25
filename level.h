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

#ifndef _LEVEL_H
#define _LEVEL_H

#include <sparrow3d.h>


typedef struct sLayer *pLayer;
typedef struct sLayer {
	int width,height;
	spSpritePointer *tile;
} tLayer;

typedef enum {
	PLAYER,
	ENEMY1,
	ENEMY2,
	BOX,
	SWITCH,
	BUTTON,
	DOOR,
	PLATFORM_V,
	PLATFORM_H
} LevelObjectType;

typedef struct sLevelObject *pLevelObject;
typedef struct sLevelObject {
	spSpriteCollectionPointer animation;
	LevelObjectType type;
	Sint32 x,y; //one tile == SP_ONE, so (because of 32x32 tiles) x>>SP_ACCURACY-5 is the pixel position
	pLevelObject prev,next; //ring of objects in one group
} tLevelObject;

typedef enum {
	PAIR,
	EGOIST //EGOIST groups don't interact with other members of the group
} LevelObjectGroupType;

typedef struct sLevelObjectGroup *pLevelObjectGroup;
typedef struct sLevelObjectGroup {
	LevelObjectGroupType type;
	pLevelObject firstObject;
	pLevelObjectGroup prev,next; //ring of objects in one group
} tLevelObjectGroup;

typedef struct sLevel *pLevel;
typedef struct sLevel {
	struct {tLayer physic,background,player,foreground;} layer;
	struct {Sint32 x,y;} camera;
	Uint16 backgroundColor;
	pLevelObjectGroup firstObjectGroup;
	pLevelObject choosenPlayer;
} tLevel;

pLevel loadLevel(char* filename);
void drawLevel(pLevel level);

#endif
