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
#ifndef PKGDATADIR
#define PKGDATADIR "."
#endif

#define FROGNET 			0
#define CURSOR				1
#define COLORDEPTH		 	8

#define SCREENHEIGHT   		480
#define HALFSCREENHEIGHT	SCREENHEIGHT/2
#define SCREENWIDTH			640
#define HALFSCREENWIDTH		SCREENWIDTH/2

// the alien mouse interface - another sphinx innovation
// divides the viewport into tic-tac-toe square 3x3 evenly
// mouse cursor in center square stops - rest move directionally 
#define LEFTTHIRD			SCREENWIDTH/3
#define RIGHTTHIRD			(SCREENWIDTH/3)*2
#define TOPTHIRD			SCREENHEIGHT/3
#define BOTTOMTHIRD			(SCREENHEIGHT/3)*2

#define OVERLAP 			40 // area between dash and background
#define CEILING				60 // bounce off here for better effect

#define MAXNAMELEN		  	40  // max size of session or frogname
#define RECT SDL_Rect
#define PLAYERMAX 8 // how many can play - sizeof player array

typedef struct  // filled out in dialog.cpp
{
	signed short Sound; // sound on off 1 0
	signed short Speed; 
	signed short NoNet; // flying solo 1
	signed short Host; // I am the man
	signed short OldMenu; // non gesture mouse menu
	unsigned int Port;
	char FrogName[ MAXNAMELEN ]; // my name passed
	char Server[ 20 ]; // server address passed
	unsigned int Soundbufsz;
} OPTIONS;

