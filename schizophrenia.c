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
#include "feedback.h"
#include "settings.h"

#include "global_defines.h"

SDL_Surface *screen = NULL;
#if defined ZOOMUP || defined ZOOMDOWN
	#if defined ZOOMUP && defined ZOOMDOWN
		SDL_Surface *dummy_screen;
	#else
		SDL_Surface *real_screen;
	#endif
#endif
spFontPointer font = NULL;

pLevel* levelPointer;

void draw_schizo( void )
{
	RESET_ZBUFFER
	CLEAN_TARGET((*levelPointer)->backgroundColor)
	spSetZSet( Z_SORTING );
	spSetZTest( Z_SORTING );
	spSetAlphaTest( 1 );

	drawLevel((*levelPointer));

	//HUD (for debug reasons)
	spSetZSet( 0 );
	spSetZTest( 0 );
	spSetAlphaTest( 1 );
	char buffer[256];
	sprintf( buffer, "camera X: %i\ncamera Y: %i\ncollisiontests: %i\nfps: %i\nspeed:%.5i,%.5i",
			(*levelPointer)->currentCamera.x >> SP_ACCURACY-5,(*levelPointer)->currentCamera.y >> SP_ACCURACY-5,
			getCollisionCount(),spGetFPS(),
			(*levelPointer)->choosenPlayer->physicsElement->speed.x,
			(*levelPointer)->choosenPlayer->physicsElement->speed.y);
	spFontDrawRight( screen->w-1, screen->h-font->maxheight*5, -1, buffer, font );
	#if defined ZOOMUP && defined ZOOMDOWN
		#ifdef FIRSTUP
			spScale2XSmooth(screen,dummy_screen);
			spScaleDownSmooth(dummy_screen,screen);
		#else
			spScaleDownSmooth(screen,dummy_screen);
			spScale2XSmooth(dummy_screen,screen);
		#endif
	#elif defined ZOOMUP
		spScale2XSmooth(screen,real_screen);
	#elif defined ZOOMDOWN
		spScaleDownSmooth(screen,real_screen);
	#endif
	spFlip();
}

Sint32 rotation = 0;

int calc_schizo( Uint32 steps )
{
	//Finish?
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;

	//Ingame controls
	if (do_control_stuff()) //result is non zero => level change
	{
		clearPhysics();
		deleteLevel((*levelPointer));		
		(*levelPointer) = loadLevel(get_saved_level());
		createPhysicsFromLevel((*levelPointer));
		return 0;
	}
	
	//Physics
	int i;
	for (i = 0; i < steps; i++)
		doPhysics(*levelPointer);

	//Visualization stuff
	rotation+=steps*16;
	updateLevelObjects();
	updateLevelSprites((*levelPointer),steps);
	calcCamera((*levelPointer),steps);
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	#if defined ZOOMUP || defined ZOOMDOWN
		#if defined ZOOMUP && defined ZOOMDOWN
			if (dummy_screen)
				spDeleteSurface(dummy_screen);
			#ifdef FIRSTUP
				dummy_screen = spCreateSurface(screen->w*2,screen->h*2);
			#else
				dummy_screen = spCreateSurface(screen->w/2,screen->h/2);
			#endif
		#else
			if (screen)
				spDeleteSurface(screen);
			#ifdef ZOOMUP
				screen = spCreateSurface(real_screen->w/2,real_screen->h/2);
			#else
				screen = spCreateSurface(real_screen->w*2,real_screen->h*2);
			#endif
		#endif
	#endif
	spSelectRenderTarget(screen);
	//Font Loading
	if ( font )
		spFontDelete( font );
	int zoom = spMin( ( screen->w << SP_ACCURACY ) / 320, ( screen->h << SP_ACCURACY ) / 240 ); //at 320x240 == 1.0
	font = spFontLoad( "./font/LiberationMono-Regular.ttf", 10 * zoom >> SP_ACCURACY );
	spFontAdd( font,SP_FONT_GROUP_ASCII, 0 ); //Just for debug stuff
	spFontAddBorder( font, spGetFastRGB(127,127,127) );
}


int main( int argc, char **argv )
{
	//sparrow3D Init
	spInitCore();
	
	//spSetDefaultWindowSize(800,480);

	//Setup
	#if (defined ZOOMUP || defined ZOOMDOWN) && !(defined ZOOMUP && defined ZOOMDOWN)
		real_screen = spCreateDefaultWindow();
		resize( real_screen->w, real_screen->h );
	#else
		screen = spCreateDefaultWindow();
		resize( screen->w, screen->h );
	#endif

	//Loading the first (*levelPointer):
	levelPointer = getLevelOverPointer();
	if (argc < 2)
		(*levelPointer) = loadLevel("./level/tile_test.tmx");
	else
		(*levelPointer) = loadLevel(argv[1]);
	createPhysicsFromLevel((*levelPointer));

	//All glory the main loop
	spLoop( draw_schizo, calc_schizo, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up …
	clearPhysics();
	deleteLevel((*levelPointer));
	spFontDelete( font );
	spDeleteSurface(screen);
	spQuitCore();
	return 0;
}
