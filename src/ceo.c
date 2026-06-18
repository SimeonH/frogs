//----------------------------------------------------------------------------
//  The Frogs Of War By Linus Sphinx (c)Copyright 2005 all rights reserved
//----------------------------------------------------------------------------
//
//    This file is part of Linus Sphinx's Frogs Of War II
//
//    Frogs Of War II is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Frogs Of War II is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Frogs Of War II; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//--------------------------------------------------------------------------------
// the frog ceo is a spectre placed on the background when a player wins,
// appears until another player is reborn
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "frogs.h"
#include "racket.h"
#include "util.h"
#include "sprite.h"
#include "player.h"
#include "tank.h"
#include "room.h"
#include "helm.h"
#include "chardisp.h"
#include "magi.h"
#include "ceo.h"

extern SDL_Surface *Screen;
extern PLAYER Player[];
extern OPTIONS Option;
extern MAGI Magi;
extern CEO Ceo;
extern short Me; // my place in player array

//-----------------------------------------------------------------
// ceo constructor
//-----------------------------------------------------------------
void ceoinit( void )
{
	Ceo.lpCeo = NULL;
	Ceo.lpCeod = NULL;
	Ceo.show = 0;
	Ceo.status = 0;
	Ceo.delay = 90;
	Ceo.dest.x = HALFSCREENWIDTH - ( CEOBMPWIDTH / 2 ); // center
	Ceo.dest.y = HALFSCREENHEIGHT - ( CEOBMPHEIGHT / 2 );
	Ceo.dest.x = 0;
	Ceo.dest.y = 0;
    Ceo.sport.x = 0;
	Ceo.sport.y = 0;
	Ceo.sport.w = CEOBMPWIDTH;
	Ceo.sport.h = CEOBMPHEIGHT;
	strcpy( Ceo.name, "ceo" ); // default name
    ceochangeCeo();
    return;
}
//-----------------------------------------------------------------
// ceo destructor
//-----------------------------------------------------------------
void ceodestroy( void )
{
    if ( Ceo.lpCeo != NULL )
		SDL_FreeSurface( Ceo.lpCeo );
    if ( Ceo.lpCeod != NULL )
		SDL_FreeSurface( Ceo.lpCeod );
}
//-----------------------------------------------------------------
// restoreSurfaces
//-----------------------------------------------------------------
void ceorestoreSurfaces( void )
{
	ceodestroy();
	ceochangeCeo();
}
//-----------------------------------------------------------------
// ceo switch bitmaps - takes name of bmp or resource label
//-----------------------------------------------------------------
void ceochangeCeo( void )
{
	char dname[ 32 ] = "";

	sprintf( dname, "bmp/%s.bmp", Ceo.name );
	Ceo.lpCeo = SDL_LoadBMP( dname );
	SDL_SetSurfacePalette( Ceo.lpCeo, Screen->format->palette );
	SDL_SetColorKey( Ceo.lpCeo, SDL_TRUE, 185 );
	sprintf( dname, "bmp/%sd.bmp", Ceo.name );
	Ceo.lpCeod = SDL_LoadBMP( dname );
	SDL_SetSurfacePalette( Ceo.lpCeod, Screen->format->palette );
	SDL_SetColorKey( Ceo.lpCeod, SDL_TRUE, 185 );
}
//-----------------------------------------------------------------
// Draw the ceo 
//-----------------------------------------------------------------
void ceodraw( SDL_Surface *screen )
{
	RECT eye = { 348,35, 0,0 };
	RECT tounge = { 292,228, 0,0 };
    RECT tounger[ 5 ] = {{ 0,131,57,107}, {59,131,57,107}, {117,131,57,107}, {175,131,57,107}, {233,131,57,107}};
	RECT eyelid[ 5 ] = {{0,0,56,74}, {57,0,56,74}, {114,0,56,74}, {171,0,56,74}, {228,0,56,74}};
	short eyeslides[ 20 ] = { 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	short toungeslides[ 20 ] = { 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0, 1, 2, 3 };
	static short g=0;
	static short g2 = 0;
	
	if ( g >= 20 )
	{
		g = 0;
		g2++;
		if ( g2 == 10 )
		{
			racketplay( 17, 0 );
			g2 = 0;
		}
	}
	
	Ceo.status = SDL_BlitSurface( Ceo.lpCeo, &Ceo.sport, screen, &Ceo.dest );
	if ( eyeslides[ g ]< 5 && Ceo.status != -2 )
		Ceo.status = SDL_BlitSurface( Ceo.lpCeod, &eyelid[ eyeslides[ g ]], screen, &eye );
	if ( toungeslides[ g ]< 5 && Ceo.status != -2 )
		Ceo.status = SDL_BlitSurface( Ceo.lpCeod, &tounger[ toungeslides[ g ]], screen, &tounge );

	if ( Ceo.status < 0 )
		ceorestoreSurfaces();

	g++;
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------

