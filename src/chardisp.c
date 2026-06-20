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
// battle frog character display
//--------------------------------------------------------------------------------
#include "compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <SDL.h>
#include "frogs.h"
#include "util.h"
#include "player.h" // for score display
#include "chardisp.h"

extern SDL_Surface *Screen;
extern PLAYER Player[];
extern OPTIONS Option;
extern ONSCREENDISPLAY Chardisp;
extern short Me; // my place in player array

//-----------------------------------------------------------------
// constructor
//-----------------------------------------------------------------
void chardispinit( void )

{
	int i;
	Chardisp.show = 1;
	Chardisp.top = MAXLINES;
	
	for ( i=0; i < ALPHANUM; i ++ ) // set up our source rects
	{
		Chardisp.Alphabet[ i ].y = 0;
		Chardisp.Alphabet[ i ].h = CHARHIGH;
		Chardisp.Alphabet[ i ].x = i * CHARWIDE;
		Chardisp.Alphabet[ i ].w = CHARWIDE;
	}
	
	Chardisp.lpAlphabet = SDL_LoadBMP( PKGDATADIR "/bmp/alphabet.bmp" );
	SDL_SetSurfacePalette( Chardisp.lpAlphabet, Screen->format->palette );
	SDL_SetColorKey( Chardisp.lpAlphabet, SDL_TRUE, 185 );
	chardispclear();

}
//-----------------------------------------------------------------
// destructor
//-----------------------------------------------------------------
void chardispdestroy( void )
{
	if ( Chardisp.lpAlphabet != NULL )
		SDL_FreeSurface( Chardisp.lpAlphabet );
}
//-----------------------------------------------------------------
// restoreSurfaces
//-----------------------------------------------------------------
void chardisprestoreSurfaces( void )
{
	Chardisp.lpAlphabet = SDL_LoadBMP( PKGDATADIR "/bmp/alphabet.bmp" );
}
//-----------------------------------------------------------------
// draw( LPDIRECTDRAWSURFACE ) rock through the lines to display and blt our rects
//-----------------------------------------------------------------
void chardispdraw( SDL_Surface *surface )
{
	int i, j, k, scorelen, scoreleft;
	short letter;
	char scorestr[ 12 ]; // 999,999,999
	RECT drect = { 0, 0, 0 , 0 };
	
	if ( Chardisp.show )
	{
		for ( i = Chardisp.top, k = 0; i < MAXLINES; i++,k++ ) // first do from top to bottom
		{
			for ( j = 0; j < MAXLENGTH && Chardisp.display[ i ].life; j++ ) 
			{
				if ( Chardisp.display[i].str[j] == '\0' )
					j = MAXLENGTH;
				else
				{
					if ( Chardisp.display[ i ].str[ j ] > 47 && Chardisp.display[ i ].str[ j ] < 58 ) // isnum
						letter = Chardisp.display[ i ].str[ j ] - 48;
					else if ( Chardisp.display[ i ].str[ j ] > 64 && Chardisp.display[ i ].str[ j ] < 91 ) 
						letter = Chardisp.display[ i ].str[ j ] - 55; // upper case
					else if ( Chardisp.display[ i ].str[ j ] > 96 && Chardisp.display[ i ].str[ j ] < 123 ) // lower case
						letter = Chardisp.display[ i ].str[ j ] - 87;
					else
						continue; // skip it
					drect.x = MARGIN + j * CHARWIDE;
					drect.y = k * (CHARHIGH + LEADING);
					if (SDL_BlitSurface( Chardisp.lpAlphabet, &Chardisp.Alphabet[ letter ], surface, &drect ) < 0)
						chardisprestoreSurfaces();
				}
			}
		}

		for ( i = 0; i < Chardisp.top; i++,k++ ) // then do from 0 to top
		{
			for ( j = 0; j < MAXLENGTH && Chardisp.display[ i ].life; j++ )
			{
				if ( Chardisp.display[i].str[j] == '\0' )
					j = MAXLENGTH;
				else
				{
					if ( Chardisp.display[ i ].str[ j ] > 47 && Chardisp.display[ i ].str[ j ] < 58 )
						letter = Chardisp.display[ i ].str[ j ] - 48;
					else if ( Chardisp.display[ i ].str[ j ] > 64 && Chardisp.display[ i ].str[ j ] < 91 ) 
						letter = Chardisp.display[ i ].str[ j ] - 55; // upper case
					else if ( Chardisp.display[ i ].str[ j ] > 96 && Chardisp.display[ i ].str[ j ] < 123 ) // lower case
						letter = Chardisp.display[ i ].str[ j ] - 87;
					else
						continue; // skip it
					drect.x = MARGIN + j * CHARWIDE;
					drect.y = k * (CHARHIGH + LEADING);
				 	if (SDL_BlitSurface( Chardisp.lpAlphabet, &Chardisp.Alphabet[ letter ], surface, &drect )) 
						chardisprestoreSurfaces();
				}
			}
		}
		for ( i = 0; i < MAXLINES; i++ )
		{
			if ( Chardisp.display[ i ].life )			  // don't revive them
				Chardisp.display[ i ].life--;
			if ( Chardisp.display[ i ].life > LINELIFE )
				Chardisp.display[ i ].life = 0;			// kill the oldest line
		}
	}
	sprintf( scorestr, "%d", Player[ Me ].bc );
	scorelen = strlen( scorestr );
	for ( j = 0; j < scorelen; j++ )
	{
		if ( scorestr[j] == '\0' )
			j = MAXLENGTH;
		else
		{
			if ( scorestr[ j ] > 47 && scorestr[ j ] < 58 )
				letter = scorestr[ j ] - 48;
			else
				continue; // skip it
			scoreleft = ( SCREENWIDTH - MARGIN )-( scorelen * CHARWIDE );
			drect.x = scoreleft + ( j * CHARWIDE );
			drect.y = LEADING;
			if (SDL_BlitSurface( Chardisp.lpAlphabet, &Chardisp.Alphabet[ letter ], surface, &drect ))
				chardisprestoreSurfaces();
		}
	}
}
//-----------------------------------------------------------------
// void addaline( char * ) add a line to display bottom and scroll up
//-----------------------------------------------------------------
void chardispaddaline( char *linetoadd )
{
	--Chardisp.top;
	memset( Chardisp.display[ Chardisp.top ].str, '\0', MAXLENGTH );
	strncpy( Chardisp.display[ Chardisp.top ].str, linetoadd, MAXLENGTH - 1 );
	Chardisp.display[ Chardisp.top ].life = LINELIFE;
	if ( Chardisp.top == 0 )
		Chardisp.top = MAXLINES;
}
//-----------------------------------------------------------------
// void addaword( char * ) add a word to bottom line no scroll
//-----------------------------------------------------------------
void chardispaddaword( char *wordstoadd )
{
	strncat( Chardisp.display[ Chardisp.top ].str, wordstoadd, MAXLENGTH - 1 );
	Chardisp.display[ Chardisp.top ].life = LINELIFE; // refresh life
	if ( Chardisp.top == 0 )
		Chardisp.top = MAXLINES;
}
//-----------------------------------------------------------------
// void clear(void) empty all lines and set top to zero
//-----------------------------------------------------------------
void chardispclear( void )
{
	int i;

	for ( i=0; i < MAXLINES; i++ )
	{								   
		memset( Chardisp.display[ i ].str, '\0', MAXLENGTH );
		Chardisp.display[ i ].life = 0;
	}
	Chardisp.top = MAXLINES;
}
//-----------------------------------------------------------------
// short has(char *) return TRUE if message in display already
//-----------------------------------------------------------------
short chardisphas( char *msg )
{
	int i, retval = 1;
	
	for ( i = 0; retval && i < MAXLINES; i++ )
	{
		if ( Chardisp.display[ i ].life )								
			retval = strcmp( msg, Chardisp.display[ i ].str );
	}
	return( retval );
}
//-----------------------------------------------------------------
// void bull(char *filename) clear, read and display text file
//-----------------------------------------------------------------
void chardispbull( char *fname )
{
	int hfile;
	int i = 0, j = 0;
	int actual = 0, sizeofile;
	char *text = NULL;
	char str[ MAXLENGTH + 1];
	
	hfile = open( fname, O_RDONLY );
	
	if ( hfile > 1 )
	{
		sizeofile = lseek( hfile, 0L, SEEK_END );
		lseek( hfile, 0L, SEEK_SET );
		text = (char *)calloc( sizeofile + 1, sizeof(char));	
		actual = read( hfile, text, sizeofile );
		text[ sizeofile ] = '\0';
		str[ 0 ] = '\0';
		chardispclear();
		if ( actual == sizeofile )
		{
			memset( str, '\0', sizeof( str ));
			while ( i < sizeofile && j < MAXLENGTH ) // if line is too long bail out
			{
				str[ j ] = text[ i ];
				if ( str[ j ] == '\n' )
				{
					str[ j ] = '\0';
					chardispaddaline( str );
					j = 0;
				}
				else
					j++;
				i++;
			}
		}
		if ( text != NULL )
			free( text );
		close( hfile );
	}
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------

