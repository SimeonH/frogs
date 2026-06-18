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
// bitmap sizes
#define BACKTILESWIDTH   256
#define BACKTILESHEIGHT  256
#define BACKDECORWIDTH  640
#define BACKDECORHEIGHT 480
#define FAKEFLOOR 45

typedef struct 
{
   int x;			// this defines a line or edge of a
   int y;			// horizontal walkable surface 
   int len;		  // in the room
} STEP;

typedef struct
{
	SDL_Surface *backtile;		 // basic tile
	char tilename[ 32 ];
	SDL_Surface *backdecor; // decorative background graphics
	char decorname[ 32 ];
	char spritename[ 32 ];
	short show; // turn on/off drawing
	short status; // if !0 fatal error occurred in constructor
	short change; // load a new tile
	short maxx;		   // col count 
	short maxy;		   // row count
	STEP *step;   // array of objects we can walk on
	int stepCount;
	SPRITE *decor;
	int spriteCount; // number of background sprite
	SPRITE *decortop; // array of level nine sprite
	int nineCount; // number of foreground level nine sprite
	RECT *actionZone; // target rects from sprites w/types !0
	int actionCount; // number of action zones
	short *actionType; // corresponding action type maps to rect
	short edgecolor; // color to paint the edges outside viewport
} ROOM;

void roominit( void );
void roomdestroy( void );
void roomdraw( SDL_Surface * );
void roomtileBackground( POINT *, RECT *, SDL_Surface * );
void roomdrawtop( SDL_Surface * );
void roomforeground( POINT *, RECT *, SDL_Surface * );
void roomrestoreSurfaces( void );
void roomchangeBackground( char * );
int roomnextCommand( char *, char * );
int roomparseSpriteFile( char * );
void roomsetStep( int, int, int, int );
short roomsolidGround( int, int );
int roomclear( int, SDL_Surface * );
int roomcollide( void );
void roomdump( void );

