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
#define TANKSWIDTH  640
#define TANKSHEIGHT 480

// max number of active missles/sprites
#define MISSLECOUNT 70

// sprite types
#define TANK 5
#define MISSLE 6
#define DECOR 7

typedef struct    
{
    SDL_Surface *tanktile;   
    char bmpname[ 32 ];
    char spritename[ 32 ];
    short show;
    short status; // if !0 fatal error occurred in constructor
    short change; // load new sprites
    SPRITE *sprite;
    int spritecount;
    SPRITE *missle;
    int misslecount;
} TANKS;

void tankinit( void );
void tankdestroy( void );
void tankdraw( SDL_Surface * );
void tankdrawPane( POINT *, RECT *, SDL_Surface * );
void tankrestoreSurfaces( void );
void tankchangeTanks( char * );
int tanknextCommand( char *, char * );
int tankparseSpriteFile( char * );
void tankaddMissle( int, int, int, int, int );
int tankhalfwide( int );
int tankhowwide( int );
int tankhalfhigh( int );
int tankhowhigh( int );
void tankadjustMissles( void );
int tankclipToView( POINT *, RECT * );
int tankclipToViewLeft( RECT *, RECT * );
int tankcollide( void );
void tankmove( void );
void tanksetPlayerSprite( int, int );
void tankadjustPlayers( void );
void tankadjustRobots( void );
