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

typedef struct sTile *pTile;
typedef struct sLayer *pLayer;
typedef struct sLevelObject *pLevelObject;
typedef struct sLevelObjectGroup *pLevelObjectGroup;
typedef struct sLevel *pLevel;

typedef enum {
	PLAYER,      //  0
	BUG,         //  1
	NEGA,        //  2
	BOX,         //  3
	SWITCH,      //  4
	BUTTON,      //  5
	DOOR,        //  6
	PLATFORM,    //  7
	TROPHIES,    //  8
	GENERIC,     //  9
	COLLECTIBLE, // 10
	UNSELECTABLE,// 11
	LOGICGROUP,  // 12
	UNKONWN
} LevelObjectType;

typedef enum {
	LEFT,
	UP,
	RIGHT,
	DOWN
} LevelDirection;

typedef enum {
	OFF = 0,
	ON = 1
} LevelState;


#include "physics.h"

typedef struct sTile {int nr;spSpritePointer sprite;} tTile;

typedef struct sLayer {
	int width,height;
	pTile tile;
} tLayer;

typedef struct sLevelObject {
	spSpriteCollectionPointer animation;
	LevelObjectType type;
	Sint32 x,y; //one tile == SP_ONE, so (because of 32x32 tiles) x>>SP_ACCURACY-5 is the pixel position
	int w,h; //pixel! not used by all objects
	struct {struct {Sint32 x,y;} v1,v2;} speed; //speed in positionchange / 1 ms. Not used by all objects
	LevelDirection direction; //direction. Not used by all objects
	char* some_char; //some objects need to save a string as e.g. destiny
	int kind; //some objects have different kinds. Saved here.
	LevelState state; //on and off
	pPhysicsElement physicsElement;
	pLevelObject prev,next; //ring of objects in one group
} tLevelObject;

typedef struct sLevelObjectGroup {
	LevelObjectType type;
	pLevelObject firstObject;
	pLevelObjectGroup prev,next; //ring of objects in one group
} tLevelObjectGroup;

typedef struct sLevel {
	spSpritePointer *spriteTable;
	int spriteTableCount;
	struct {tLayer physics,background,player,foreground;} layer;
	struct {Sint32 x,y;} targetCamera, actualCamera;
	Uint16 backgroundColor;
	pLevelObjectGroup firstObjectGroup;
	pLevelObject choosenPlayer;
} tLevel;

pLevel loadLevel(char* filename);
void drawLevel(pLevel level);
void calcCamera(pLevel level,Sint32 steps);
void updateLevelSprites(pLevel level,int steps);
#endif
