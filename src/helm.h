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
#define CONTROLSWIDTH   640
#define CONTROLSHEIGHT  110
#define HANDSWIDTH 416
#define HANDSHEIGHT 110

// frog scope coordinates
#define SCOPECENTERX CONTROLSWIDTH/2 // where I am on scope in center -row
#define SCOPECENTERY SCREENHEIGHT-(CONTROLSHEIGHT/2) //422 // physically on display -column

#define SCOPEXRATIO 73 // map x / scope wide 
#define SCOPEYRATIO 64 // map y / scope high

#define SCOPELEFT 284 // physical display edges from center x and y
#define SCOPERIGHT 354
#define SCOPETOP 387
#define SCOPEBOTTOM 447
#define SCOPEWIDE   70 // right - left
#define SCOPEHALF 35 // right - left / 2
 
typedef struct
{
	SDL_Surface *controls;		 // control panel 
	SDL_Surface *hands;			// on the wheel
	short curlever;
	short curtool;
	short show; // turn on off drawing
	short status; // if !0 fatal error occurred in constructor
} HELM;

void helminit( void );
void helmdestroy( void );
void helmdraw( SDL_Surface * );
void helmrestoreSurfaces( void );
