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

#ifndef _PHYSIC_H
#define _PHYSIC_H

#include <sparrow3d.h>

typedef struct sPhysicElement *pPhysicElement;
typedef struct sPhysicCollision *pPhysicCollision;

#include "level.h"

typedef struct sPhysicElement {
	struct {Sint32 x,y;} position,backupPosition,speed;
	Sint32 w,h; //32 pixel == SP_ONE
	Sint32 sqSize; //The square of the bounding circle of the rectangle; for distance
	int gravitation;
	int permeability; //bit,direction: 0,left 1,top 2,right 3,down (0 means solid)
	int superPower;
	int freeFallCounter;
	LevelObjectType type;
	pLevelObject levelObject;
	pPhysicElement prev,next;
} tPhysicElement;

typedef struct sPhysicCollision {
	pPhysicElement element[2];
	int hitPosition[2]; //like permeability; always pairs like left/right or top/down
	pPhysicCollision prev,next;
} tPhysicCollision;

pPhysicElement createPhysicElement(Sint32 px,Sint32 py,Sint32 w,Sint32 h,int moveable,int gravitation,int superPower,pLevelObject levelObject);
void createPhysicFromLevel(pLevel level);
void clearPhysic(); //Deletes the whole scene
void doPhysic(int TimeForOneStep,void ( *setSpeed )( pPhysicElement element ),void ( *gravFeedback )( pPhysicCollision collision ),void ( *yFeedback )( pPhysicCollision collision ),void ( *xFeedback )( pPhysicCollision collision ));
void updateLevelObjects();

#endif
