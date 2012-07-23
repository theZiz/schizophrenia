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
SDL_Surface *scientist;
spSpritePointer sprite;
Sint32 rotation = 0;
spFontPointer font = NULL;

void draw_test( void )
{
	RESET_ZBUFFER
	CLEAN_TARGET
	spSetZSet( Z_SORTING );
	spSetZTest( Z_SORTING );
	spSetAlphaTest( 1 );

	sprite->rotation = 0;
	spDrawSprite( screen->w / 5, 5 * screen->h / 8, -1, sprite );
	sprite->zoomX = spSin( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 );
	sprite->zoomY = spCos( rotation * 6 ) + ( 3 << SP_ACCURACY - 1 );
	spDrawSprite( 2 * screen->w / 5, 5 * screen->h / 8, -1, sprite );
	sprite->rotation = rotation * 4;
	spDrawSprite( 3 * screen->w / 5, 5 * screen->h / 8, -1, sprite );
	sprite->zoomX = SP_ONE;
	sprite->zoomY = SP_ONE;
	spDrawSprite( 4 * screen->w / 5, 5 * screen->h / 8, -1, sprite );


	//HUD (for debug reasons)
	spSetZSet( 0 );
	spSetZTest( 0 );
	spSetAlphaTest( 1 );
	char buffer[16];
	sprintf( buffer, "fps: %i", spGetFPS() );
	spFontDrawRight( screen->w-1, screen->h-font->maxheight, -1, buffer, font );

	spFlip();
}


int calc_test( Uint32 steps )
{
	spUpdateSprite( sprite, steps );

	rotation += steps << SP_ACCURACY - 11;

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
	spFontAdd( font,SP_FONT_GROUP_NUMBERS, 0 ); //Just for fps
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

	//Textures loading
	scientist = spLoadSurface( "./data/science_guy_frames01.png" );

	//Sprite Creating
	sprite = spNewSprite(NULL);
	spNewSubSpriteTilingRow( sprite, scientist, 1, 1, 22, 46, 24, 48, 9 ,100);

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up …
	spFontDelete( font );
	spDeleteSprite(sprite);
	spDeleteSurface( scientist );
	spQuitCore();
	return 0;
}
