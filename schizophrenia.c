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
 Janek Schäfer (foxblock) , foxblock_at_gmail_com
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrow3d.h>
#include <SDL_image.h>
#include "level.h"
#include "physics.h"

#include "global_defines.h"

SDL_Surface *screen;
spFontPointer font = NULL;
pLevel level;

void draw_schizo( void )
{
	RESET_ZBUFFER
	CLEAN_TARGET(level->backgroundColor)
	spSetZSet( Z_SORTING );
	spSetZTest( Z_SORTING );
	spSetAlphaTest( 1 );

	drawLevel(level);

	//HUD (for debug reasons)
	spSetZSet( 0 );
	spSetZTest( 0 );
	spSetAlphaTest( 1 );
	char buffer[256];
	sprintf( buffer, "camera X: %i\ncamera Y: %i\nfps: %i", level->actualCamera.x >> SP_ACCURACY-5,level->actualCamera.y >> SP_ACCURACY-5,spGetFPS() );
	spFontDrawRight( screen->w-1, screen->h-font->maxheight*3, -1, buffer, font );

	spFlip();
}

Sint32 rotation = 0;
Sint32 last_run = 0;
#define PHYSICS_STEP 10
int rest = 0;

void setSpeed( pPhysicsElement element )
{
	if (element->levelObject == NULL)
		return;
	if (element->levelObject == level->choosenPlayer)
	{
		//Moving the player
		if (spGetInput()->axis[0] < 0)
		{
			if (last_run >= 0)
			{
				spSelectSprite(level->choosenPlayer->animation,"run left");
				last_run = 0;
			}
			last_run-=PHYSICS_STEP;
			if (last_run > -128)
				element->speed.x = last_run*2;
			else
				element->speed.x = -256;
		}
		else
		if (spGetInput()->axis[0] > 0)
		{
			if (last_run <= 0)
			{
				spSelectSprite(level->choosenPlayer->animation,"run right");
				last_run = 0;
			}
			last_run+=PHYSICS_STEP;
			if (last_run < 128)
				element->speed.x = last_run*2;
			else
				element->speed.x = 256;
		}
		else
		if (last_run < 0)
		{
			spSelectSprite(level->choosenPlayer->animation,"stand left");
			last_run = 0;
		}
		else
		if (last_run > 0)
		{
			spSelectSprite(level->choosenPlayer->animation,"stand right");
			last_run = 0;
		}
	}
}

void removeCollision( pPhysicsCollision collision )
{
	if (collision != collision->next)
	{
		collision->prev->next = collision->next;
		collision->next->prev = collision->prev;
	}
	free(collision);
}

int gravFeedback( pPhysicsCollision collision )
{
	if (collision->element[0]->type == PLAYER && collision->hitPosition[0] == 2) //TOP hit on player
	{
		collision->element[0]->killed = 1;
		removeCollision(collision);
		return 1;
	}
	if (collision->element[1]->type == PLAYER && collision->hitPosition[1] == 2) //TOP hit on player
	{
		collision->element[1]->killed = 1;
		removeCollision(collision);
		return 1;
	}
	return 0;
}

int yFeedback( pPhysicsCollision collision )
{
	return 0;
}

int xFeedback( pPhysicsCollision collision )
{
	return 0;
}

int calc_schizo( Uint32 steps )
{
	if (steps > 20)
	printf("%i\n",steps);
	//Controls and some Logic (more in setSpeed)
	if (spGetInput()->button[SP_BUTTON_R])
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		level->choosenPlayer = level->choosenPlayer->prev;
	}
	if (spGetInput()->button[SP_BUTTON_L])
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		level->choosenPlayer = level->choosenPlayer->next;
	}
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	
	//Physics
	int i;
	int physics_steps = (steps + rest) / PHYSICS_STEP;
	for (i = 0; i < physics_steps; i++)
			doPhysics(PHYSICS_STEP,setSpeed,gravFeedback,yFeedback,xFeedback,level);
	rest = (steps + rest) % PHYSICS_STEP;
	//Visualization stuff
	rotation+=steps*16;
	updateLevelObjects();
	updateLevelSprites(level,steps);
	calcCamera(level,steps);
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/LiberationMono-Regular.ttf", 10 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font,SP_FONT_GROUP_ASCII, 0 ); //Just for debug stuff
	spFontAddBorder( font, spGetFastRGB(127,127,127) );
}


int main( int argc, char **argv )
{
	//sparrow3D Init
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize( screen->w, screen->h );

	//Loading the first level:
	if (argc < 2)
		level = loadLevel("./level/tile_test.tmx");
	else
		level = loadLevel(argv[1]);
	createPhysicsFromLevel(level);

	//All glory the main loop
	spLoop( draw_schizo, calc_schizo, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up …
	clearPhysics();
	deleteLevel(level);
	spFontDelete( font );
	spQuitCore();
	return 0;
}
