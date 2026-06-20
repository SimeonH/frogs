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
// miscellaneous utility functions 
//--------------------------------------------------------------------------------
#include "compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "frogs.h"
#include "util.h"
#include "player.h"
#include "sprite.h"
#include "room.h"
#include "chardisp.h"
#include "racket.h"

extern PLAYER Player[];
extern ROSTER Roster[];
extern OPTIONS Option;
extern ROOM Room;
extern short Me;
extern short Winner;

SDL_Color Colors[ 4 ][ 256 ];

//----------------------------------------------------------------
// int nextCommand( char *instr, char *outstr, char *place )
//      find and retrieve a command string from instr starting at place
//      strip cr/lf/comments, add a terminating comma and a null
// input:
//      char *instr - compound string full of comments
//      char *outstr - null terminated command string a-z:#,#,#,\0
// returns:
//      0 - success
//      !0 - failure
//----------------------------------------------------------------
int nextCommand( char *place, char *outstr )
{
   char *x = place;
   char *y = outstr;
   int found = 0; // increment if command found
        
    while( *x++ && !found ) // find start of a command
    {
        if ( *x == ':' )
        {
            --x;
            if ( strchr( "01234567", (int)*x ) != NULL ) // got one
            {
                found++;
                while( *x && strchr( " \t\r\n\\", (int)*x ) == NULL )  // fill output
                    *y++ = *x++;
                *y++ = '\0';
            }
            else
                while( *x != '\n' )  // skip it
                    x++;
        }
    }
    return found;
}//eo nextCommand

//---------------------------------------------------------------------------
// void loadbmp( SDL_Surface* s, char* f)
// 		given s Screen and f filename loads a bitmap and installs its palette
//----------------------------------------------------------------------------
void loadbmp( SDL_Surface *surface, char *fname, int load_palette )
{
	int i;
	SDL_Surface *image = NULL;

	image = SDL_LoadBMP( fname );

	if (image == NULL)
	{
		fprintf(stderr, "Error loading %s: %s\n", fname, SDL_GetError());
		exit( 0 );
	}

	if (image->format->palette && surface->format->palette)
	{
		if ( load_palette ) // set up palette effects
		{
			for ( i = 0; i < 256; i++ )
			{
				// default
				Colors[ 0 ][ i ].r = image->format->palette->colors[ i ].r;
				Colors[ 0 ][ i ].g = image->format->palette->colors[ i ].g;
				Colors[ 0 ][ i ].b = image->format->palette->colors[ i ].b;

				// red
				Colors[ 1 ][ i ].r = image->format->palette->colors[ i ].r;
				Colors[ 1 ][ i ].g = image->format->palette->colors[ i ].g / 4;
				Colors[ 1 ][ i ].b = image->format->palette->colors[ i ].b / 4;

				// green
				Colors[ 2 ][ i ].r = image->format->palette->colors[ i ].r / 4;
				Colors[ 2 ][ i ].g = image->format->palette->colors[ i ].g;
				Colors[ 2 ][ i ].b = image->format->palette->colors[ i ].b / 4;

				// blue
				Colors[ 3 ][ i ].r = image->format->palette->colors[ i ].r / 4;
				Colors[ 3 ][ i ].g = image->format->palette->colors[ i ].g / 4;
				Colors[ 3 ][ i ].b = image->format->palette->colors[ i ].b;
			}
		}
		SDL_SetPaletteColors(surface->format->palette,
			image->format->palette->colors, 0, image->format->palette->ncolors);
		SDL_SetSurfacePalette(image, surface->format->palette);
	}

	if ( SDL_BlitSurface(image, NULL, surface, NULL) < 0 )
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());

	SDL_FreeSurface(image);
}// eo loadbmp

//-----------------------------------------------------------------
// void setPaletteColor( short pallete )
// 		set the pallete of surface to one of:
// 		0 = default
// 		1 = red effect
// 		2 = green effect
// 		3 = blue effect
//-----------------------------------------------------------------
void setPaletteColor( SDL_Surface *surface, short pallete )
{
	SDL_SetPaletteColors(surface->format->palette, Colors[ pallete ], 0, 256);
}

//-----------------------------------------------------------------
// void paintscreen( SDL_Surface surface, short color )
// 		flood screen with a given color
//-----------------------------------------------------------------
void paintscreen( SDL_Surface *Screen, short color )
{
	SDL_FillRect( Screen, NULL, color );
	return;
}// eo paintscreen

//-----------------------------------------------------------------
// short pointinrect( POINT *pt RECT *rect )
// 		return true if pt is within rect
//-----------------------------------------------------------------

short pointinrect( POINT *pt, RECT *rect )
{
	if ( pt->x > rect->x && pt->x < rect->w && pt->y > rect->y && pt->y < rect->h )
	{
		// fprintf( stderr, "px:%d py:%d rx:%d ry:%d rw:%d rh%d\n", pt->x, pt->y, rect->x, rect->y, rect->w, rect->h);
		return( 1 );
	}
	return( 0 );
} //eo pointinrect

//-----------------------------------------------------------------
// short intersectrect( RECT *me, RECT *ze )
// 		return true if rect is within rect
//-----------------------------------------------------------------
short intersectrect( RECT *me, RECT *ze )
{
	short ret = 0;
	if (( me->w >= ze->x && me->w <= ze->w )&&( me->h >= ze->y && me->h <= ze->h ))
		ret++;
	else if (( me->x >= ze->x && me->x <= ze->w )&&( me->h >= ze->y && me->h <= ze->h ))
		ret++;
	else if (( me->y >= ze->y && me->y <= ze->h )&&( me->w >= ze->x && me->w <= ze->w ))
		ret++;
	else if (( me->x >= ze->x && me->x <= ze->w )&&( me->y >= ze->y && me->y <= ze->h ))
		ret++;
	return( ret );
}

//-----------------------------------------------------------------
// void loadRobotsFile( void )
//	  open and read file of player/robot x and y starting points
// returns:
//		  0 = success
//
//-----------------------------------------------------------------
short loadRobotsFile( char *bots )
{
	
	int hfile, sizeofile, actual = 0; 
	short retval=0, robotnum=0, robotx=0, roboty=0;
	char comstr[256] = "";
	char *text = 0;
	char *place;

	hfile = open( bots, O_RDONLY );
	if ( hfile > 0 )
	{
		sizeofile = lseek( hfile, 0L, SEEK_END );
		lseek( hfile, 0L, SEEK_SET );
		text = (char *)calloc( sizeofile + 1, sizeof(char));
		if ( text != NULL )
		{
			text[ sizeofile ] = '\0';
			place = text;
			actual = read( hfile, text, sizeofile );
			if ( actual == sizeofile )
			{
				while( !retval && nextCommand( place, comstr ))
				{
					place += strlen( comstr );
					while ( *place && *place != '\n' ) // strip empty lines
						place++;
					robotnum = atoi( strtok( comstr, ":" ));
					if ( robotnum < 0 )
						retval = -1;
					if ( robotnum < PLAYERMAX - 1 && robotnum > -1 )
					{
						playerrefresh(robotnum);
						Player[ robotnum ].id = -1;
						strcpy( Roster[ robotnum ].name, "robotoad" );
						robotx = atoi( strtok( NULL, "," ));
						roboty = atoi( strtok( NULL, "\n" ));
						Player[ robotnum ].x = robotx;
						Player[ robotnum ].y = roboty;
					}
				}
				for ( robotnum++; robotnum < PLAYERMAX; robotnum++ )
				{
					Player[ robotnum ].id = -2;
					Player[ robotnum ].guy = DUSTTOAD;
				}
			}
			else 
				retval = -1;
			free( text );
		}
		else 
			retval = -1;
		close( hfile );	
	}	
	else
		retval = -1;
	return( retval );
}

//----------------------------------------------------------------
// setRobotsXy - crude arrangement of players in case
//                              no robots.txt found     and playing solo
// input:
//              none
// return:
//              void
//----------------------------------------------------------------
void setRobotsXy( void ) 
{
	int i, xx = 0, yy = 0;

	for ( i = 0; i < PLAYERMAX - 1; i++ ) 
	{
    	playerrefresh(i);
    	if ( i ) 
			Player[ i ].id = -1;
		Player[ i ].x = xx += Room.maxx / 8;
    	Player[ i ].y = yy += Room.maxy / 8;
	}
}
//----------------------------------------------------------------
// timeleft to take in the scenery
//----------------------------------------------------------------

unsigned int timeleft(unsigned int x)
{
		static unsigned int next_time = 0;
		unsigned int now;

		now = SDL_GetTicks();
		if ( next_time <= now ) {
			next_time = now + x;
		return(0);
	}
	return(next_time-now);
}

//----------------------------------------------------------------
// void SDL_BlitFlop(SDL_Surface *, RECT *, SDL_Surface *, RECT * )
// 		mirrors right to left blit one vertical stripe at a time
//----------------------------------------------------------------

void SDL_BlitFlop(SDL_Surface *Source, RECT *trect, SDL_Surface *Dest, RECT *drect )
{
	RECT sslice, dslice;
	int i;

	sslice.y = trect->y;
	sslice.h = trect->h;
	sslice.w = 1;
	dslice.y = drect->y;
	dslice.h = trect->h;
	dslice.w = 1;

	i = trect->w;

	while( i )
	{
		sslice.x = trect->x + i;
		dslice.x = drect->x + trect->w - i;
		// printf("0 sx:%d-dx:%d\n", sslice.x, dslice.x );
		SDL_BlitSurface( Source, &sslice, Dest, &dslice );
		--i;
	}
}
//----------------------------------------------------------------
// void whowon( void )
//----------------------------------------------------------------
void whowon( void )
{
	short i, dude, playercount = 0, dustcount = 0; // reset
	char *outstr = NULL;

	Winner = -1; // reset

    for ( i = 0; i < PLAYERMAX; i++ )  // check for winner
	{
		if ( Player[ i ].id > -2 ) // robots too
			playercount++;
		if ( Player[ i ].guy == DUSTTOAD ) // count dusters
			dustcount++;
		else if ( Player[ i ].id > -2 ) // robots too
			dude = i;
	}
    if ( playercount > 1 && dustcount == ( playercount - 1 ))  // no winnah if evahbody dies
	{
		Winner = dude; // must be this one
		outstr = calloc( 80, 1 );
		if ( outstr != NULL )
		{
			sprintf( outstr, "TOAD %d %s HAS WON WITH %d", Winner + 1, Roster[ Winner ].name, Player[ Winner ].bc );
			if ( chardisphas( outstr ))
	   			chardispaddaline( outstr );
			free( outstr );
		}
	}
	if ( Me == Winner ) // show the ceo
	{
		if ( chardisphas( "YOU HAVE WON" ))
		{
			chardispaddaline( "ESC QUITS" );
			chardispaddaline( "F8 RESETS" );
			chardispaddaline( "YOU HAVE WON" );
		}
	}
}

//----------------------------------------------------------------
// void clip( RECT *d, RECT *t ) 
// 		prune t for placement at d if d.x or d.y and adjust if negative else do nothing
// 		return true if clipped results are nonsense
//----------------------------------------------------------------
short clip( RECT *d, RECT *t )
{
	int dify = 0; // , difx = 0;  
	if ( d->y < 0 )
	{
		dify = abs( d->y );
		// printf( "b - dify:%d ty:%d th:%d\n", dify, t->y, t->h );
		t->y += dify;
		if ( dify < t->h )
			t->h -= dify;
		else 
			dify = -1;
		d->y = 0;
		// printf( "a - dif:%d ty:%d th:%d\n", dify, t->y, t->h );
	}

	return (dify < 0)?-1:0;
}

//----------------------------------------------------------------
// strlwr - to lower case string (MinGW provides its own strlwr)
//----------------------------------------------------------------
#ifndef _WIN32
void strlwr( char *s )
{
	while( *s )
	{
		*s = (char)tolower((int)*s );
		*s++;
	}
}
#endif
//----------------------------------------------------------------
// eof
//----------------------------------------------------------------
