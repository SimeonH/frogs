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
// the magi is a spectre placed on the background when a player dies,
// It appears delay / frame rate seconds after death, 
// stays for delay / frame rate seconds vanishes appears until player reborn
//----------------------
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

extern SDL_Surface *Screen;
extern PLAYER Player[];
extern OPTIONS Option;
extern MAGI Magi;
extern short Me; // my place in player array

//-----------------------------------------------------------------
// magi switch bitmaps - takes name of bmp no path or ext.
//-----------------------------------------------------------------
void magichangeMagi( char *newbmp )
{
	magidestroy();
	sprintf( Magi.name, PKGDATADIR "/bmp/%s.bmp", newbmp );
	Magi.lpMagi = SDL_LoadBMP( Magi.name );
	SDL_SetSurfacePalette( Magi.lpMagi, Screen->format->palette );
	SDL_SetColorKey( Magi.lpMagi, SDL_TRUE, 185 );
}

//-----------------------------------------------------------------
// magi constructor
//-----------------------------------------------------------------
void magiinit( void )
{
    Magi.show = 0;
	Magi.status = 0;
	Magi.delay = 90;
	Magi.dest.x = HALFSCREENWIDTH - ( MAGIBMPWIDTH / 2 ); // center
	Magi.dest.y = 50;

	Magi.sport.x = 0;
    Magi.sport.y = 0;
	Magi.sport.w = MAGIBMPWIDTH;
	Magi.sport.h = MAGIBMPHEIGHT;

    magichangeMagi( "magi" );
    return;
}
//-----------------------------------------------------------------
// magi destructor
//-----------------------------------------------------------------
void magidestroy( void )
{
	if ( Magi.lpMagi != NULL )
		SDL_FreeSurface( Magi.lpMagi );
}
//-----------------------------------------------------------------
// restoreSurfaces
//-----------------------------------------------------------------
void magirestoreSurfaces( void )
{
	magidestroy();
	Magi.lpMagi = SDL_LoadBMP( Magi.name );
	SDL_SetSurfacePalette( Magi.lpMagi, Screen->format->palette );
	SDL_SetColorKey( Magi.lpMagi, SDL_TRUE, 185 );
}
//-----------------------------------------------------------------
// Draw the magi if need be
//-----------------------------------------------------------------
void magidraw( SDL_Surface *screen )
{
	static int timer = 0;
	if ( Player[ Me ].guy == DUSTTOAD )
	{
		timer++;
		if ( timer > Magi.delay ) // showtime 
		{
			Magi.status = SDL_BlitSurface( Magi.lpMagi, &Magi.sport, screen, &Magi.dest );
			if ( Magi.status < 0)
				magirestoreSurfaces();
			if ( timer > ( Magi.delay * 2 ))
			{
				timer = 0;
			}
		}
	}
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------

