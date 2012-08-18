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

#ifndef _FEEDBACK_H
#define _FEEDBACK_H

#include "physics.h"
#include "level.h"

pLevel* getLevelOverPointer();
void do_control_stuff();
void setSpeed( pPhysicsElement element );
int gravFeedback( pPhysicsCollision collision );
int yFeedback( pPhysicsCollision collision );
int xFeedback( pPhysicsCollision collision );

#endif

