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

typedef struct
{
	int x;
	int y;
} POINT;

int nextCommand( char *, char * );
void loadbmp( SDL_Surface *, char *, int );
void paintscreen( SDL_Surface *, short );
void setPaletteColor( SDL_Surface *, short );
short pointinrect( POINT *, RECT * );
short intersectrect(RECT *, RECT * );
short loadRobotsFile( char * );
short youWon( void );
void setRobotsXy( void );
unsigned int timeleft(unsigned int);
void SDL_BlitFlop(SDL_Surface *, RECT *, SDL_Surface *, RECT * );
void whowon( void );
short clip( RECT *, RECT * );
#ifndef _WIN32
void strlwr( char* );
#endif
