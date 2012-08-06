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

#include "global_defines.h"

SDL_Surface *screen;
spFontPointer font = NULL;
pLevel level;

void draw_test( void )
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

int calc_test( Uint32 steps )
{
	rotation+=steps*16;
	if ( spGetInput()->axis[0] < 0)
		level->targetCamera.x -= steps*256;
	if ( spGetInput()->axis[0] > 0)
		level->targetCamera.x += steps*256;
	if ( spGetInput()->axis[1] < 0)
		level->targetCamera.y += steps*256;
	if ( spGetInput()->axis[1] > 0)
		level->targetCamera.y -= steps*256;

	updateLevelSprites(level,steps);	
	calcCamera(level,steps);	

	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	
		
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
	level = loadLevel("./level/tile_test.tmx");
	createPhysicFromLevel(level);

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, NULL );
	
	//Winter Wrap up, Winter Wrap up …
	clearPhysic();
	deleteLevel(level);
	spFontDelete( font );
	spQuitCore();
	return 0;
}
