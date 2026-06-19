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
// battle frog helm menu class
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "frogs.h"
#include "util.h"
#include "sprite.h"
#include "player.h"
#include "tank.h"
#include "room.h"
#include "helm.h"

extern SDL_Surface *Screen;
extern PLAYER Player[];
extern TANKS Tank; // for missles on scope
extern HELM Helm;
extern short Me; // my place in player array

//-----------------------------------------------------------------
// helm initializer
//-----------------------------------------------------------------
void helminit( void )
{
	Helm.curlever = 0;
	Helm.show = 1;
	Helm.curtool = 0;

	Helm.controls = SDL_LoadBMP( PKGDATADIR "/bmp/control.bmp" );
	Helm.hands = SDL_LoadBMP( PKGDATADIR "/bmp/hands.bmp" );
	SDL_SetSurfacePalette(Helm.controls, Screen->format->palette);
	SDL_SetSurfacePalette(Helm.hands, Screen->format->palette);
	SDL_SetColorKey(Helm.controls, SDL_TRUE, 185 );
	SDL_SetColorKey(Helm.hands, SDL_TRUE, 185 );
	
}
//-----------------------------------------------------------------
// destructor
//-----------------------------------------------------------------
void helmdestroy( void )
{
	if ( Helm.controls != NULL )
		SDL_FreeSurface( Helm.controls );
	if ( Helm.hands != NULL )
		SDL_FreeSurface( Helm.hands);
}
//-----------------------------------------------------------------
// draw
//-----------------------------------------------------------------
void helmdraw( SDL_Surface *surface )
{
	int i;
	short bliptouse = 0, diffx, vleft, myscopex;
	POINT nextblip;
	short ox = (SCREENWIDTH - CONTROLSWIDTH) / 2;
	short oy = (SCREENHEIGHT - CONTROLSHEIGHT);
	RECT hands[ 3 ];
   	hands[ 0 ].x = 0;
	hands[ 0 ].y = 0;
	hands[ 0 ].w = 85;
	hands[ 0 ].h = 110;

   	hands[ 1 ].x = 87;
	hands[ 1 ].y = 0;
	hands[ 1 ].w = 82;
	hands[ 1 ].h = 110;

   	hands[ 2 ].x = 294;
	hands[ 2 ].y = 0;
	hands[ 2 ].w = 82;
	hands[ 2 ].h = 110;

	POINT lever[ 4 ]; 
	lever[ 0 ].x = 135;
	lever[ 0 ].y = 15;
   	lever[ 1 ].x = 187;
	lever[ 1 ].y = 18;
	lever[ 2 ].x = 371;
	lever[ 2 ].y = 14;
   	lever[ 3 ].x = 423;
	lever[ 3 ].y = 0;
	RECT controls;
	controls.x = 0;
	controls.y = 0;
	controls.w = 640;
	controls.h = 110;
	RECT tools[ 3 ];
    tools[ 0 ].x = 170;
	tools[ 0 ].y = 0;
	tools[ 0 ].w = 37;
	tools[ 0 ].h = 29;
	tools[ 1 ].x = 170;
	tools[ 1 ].y = 31;
	tools[ 1 ].w = 37;
	tools[ 1 ].h = 28;
	tools[ 2 ].x = 170;
	tools[ 2 ].y = 61;
	tools[ 2 ].w = 37;
	tools[ 2 ].h = 28;
	RECT gauge[ 2 ];
   	gauge[ 0 ].x = 543;
	gauge[ 0 ].y = 39;
	gauge[ 0 ].w = 60;
	gauge[ 0 ].h = 8;
	gauge[ 1 ].x = 551;
	gauge[ 1 ].y = 79;
	gauge[ 1 ].w = 60;
	gauge[ 1 ].h = 8;
	RECT firehand;  
	RECT blip[ 5 ]; // frog states on scope
   	blip[ 0 ].x = 209; // normal
	blip[ 0 ].y = 31;
	blip[ 0 ].w = 4;
	blip[ 0 ].h = 4;
	blip[ 1 ].x = 209; // green    
	blip[ 1 ].y = 36;
	blip[ 1 ].w = 4;
	blip[ 1 ].h = 4;
	blip[ 2 ].x = 209; // blue
	blip[ 2 ].y = 41;
	blip[ 2 ].w = 4;
	blip[ 2 ].h = 4;
	blip[ 3 ].x = 209; // red 
	blip[ 3 ].y = 46;
	blip[ 3 ].w = 4;
	blip[ 3 ].h = 4;
	blip[ 4 ].x = 140; // dust  
	blip[ 4 ].y = 21;
	blip[ 4 ].w = 1;
	blip[ 4 ].h = 1;
	RECT mlip[ 3 ];
   	mlip[ 0 ].x = 198;
	mlip[ 0 ].y = 81;
	mlip[ 0 ].w = 2;
	mlip[ 0 ].h = 1; 
   	mlip[ 1 ].x = 191;
	mlip[ 1 ].y = 41;
	mlip[ 1 ].w = 1;
	mlip[ 1 ].h = 1; 
   	mlip[ 2 ].x = 203;
	mlip[ 2 ].y = 16;
	mlip[ 2 ].w = 1;
	mlip[ 2 ].h = 1; 
	RECT tdest;

	if ( ! Helm.show )
		return;
	// bleet the controls
	tdest.x = ox;
	tdest.y = oy;
	if (SDL_BlitSurface( Helm.controls, NULL, surface, &tdest ) < 0 )
		helmrestoreSurfaces();
	// blt the current tool
	if ( Helm.curtool > 2 ) // tool selection rotates
		Helm.curtool = 0;
	if ( Helm.curtool < 0 )
		Helm.curtool = 2;
	tdest.x = 57 + ox;
	tdest.y = oy + 38;
	if (SDL_BlitSurface( Helm.hands, &tools[ Helm.curtool ], surface, &tdest ) < 0)
		helmrestoreSurfaces();
	//Blt the hands according to current lever
	switch( Helm.curlever )
	{
		case 0:
			hands[ 0 ].h = HANDSHEIGHT - lever[ Helm.curlever ].y;
			tdest.x = lever[ Helm.curlever ].x + ox;
			tdest.y = oy + lever[ Helm.curlever ].y;
			if (SDL_BlitSurface( Helm.hands, &hands[0], surface, &tdest ) < 0)
				helmrestoreSurfaces();
			break;
		case 1:
			hands[ 0 ].h = HANDSHEIGHT - lever[ Helm.curlever ].y;
			tdest.x = lever[ Helm.curlever ].x + ox;
			tdest.y = oy + lever[ Helm.curlever ].y;
			if (SDL_BlitSurface( Helm.hands, &hands[0], surface, &tdest ) < 0)
				helmrestoreSurfaces();
			break;
		case 2:
			hands[ 0 ].h = HANDSHEIGHT - lever[ Helm.curlever ].y;
			firehand.x = lever[ Helm.curlever ].x + ox;
			firehand.y = oy + lever[ Helm.curlever ].y;
			if (SDL_BlitSurface( Helm.hands, &hands[2], surface, &firehand ) < 0)
				helmrestoreSurfaces();
			break;
		case 3:
			firehand.x = lever[ Helm.curlever ].x + ox;
			firehand.y = oy + lever[ Helm.curlever ].y;
			hands[ 0 ].h = HANDSHEIGHT - lever[ 0 ].y;
			if (SDL_BlitSurface( Helm.hands, &hands[1], surface, &firehand ) < 0)
				helmrestoreSurfaces();
			break;
		default:
			Helm.curlever = 0;
	}

	// adjust the gauges
	if ( Player[ Me ].power > 0 )
	{
		gauge[ 0 ].x += ox;
		gauge[ 0 ].y += oy;
		gauge[ 0 ].w = ( Player[ Me ].power / 100 );
		SDL_FillRect( surface, &gauge[0], 139);
	}
	if ( Player[ Me ].shield > 0 )
	{
		gauge[ 1 ].x += ox;
		gauge[ 1 ].y += oy;
		gauge[ 1 ].w = ( Player[ Me ].shield / 100 );
		SDL_FillRect( surface, &gauge[1], 228);
	}

	// frog scope online
	myscopex = Player[ Me ].x / SCOPEXRATIO;

	for ( i=0; i < PLAYERMAX; i++ )
	{
		if ( Player[ i ].id == -2 ) // nobot
			continue;

		nextblip.x = Player[ i ].x / SCOPEXRATIO;

		if ( myscopex > SCOPEHALF ) // horizon in front
		{
			vleft = myscopex - SCOPEHALF;
			diffx = SCOPEWIDE - vleft;		 
			if ( nextblip.x > vleft )
				nextblip.x -= vleft;
			else
				nextblip.x += diffx;
		}
		else if ( myscopex < SCOPEHALF ) // horizon behind
		{
			vleft = myscopex + 35;
			diffx = SCOPEWIDE - vleft;

			if ( nextblip.x > vleft )
				nextblip.x -= vleft;
			else
				nextblip.x += diffx;
		}
		nextblip.x += SCOPELEFT;

		if ( Player[ i ].y <= Player[ Me ].y ) // this player sits high says I
			nextblip.y = SCOPECENTERY - (( Player[ Me ].y - Player[ i ].y ) / SCOPEYRATIO ); 
		if ( Player[ i ].y > Player[ Me ].y )
			nextblip.y = SCOPECENTERY + (( Player[ i ].y - Player[ Me ].y ) / SCOPEYRATIO ); 
				// bottom doesn't wrap
		if ( nextblip.y < SCOPETOP || nextblip.y > SCOPEBOTTOM ) // outta range
			continue;
		switch( Player[ i ].guy ) // show player states on scope
		{
				case SPLATOAD:
					bliptouse = 1;
					break;
				case ZAPZTOAD:
					bliptouse = 2;
					break;
				case REDTOAD1:
				case REDTOAD2:
					bliptouse = 3;
					break;
				case DUSTTOAD:
					bliptouse = 4;
					break;
				default:
					bliptouse = 0;
					break;
			}
			firehand.x = nextblip.x + ox;
			firehand.y = nextblip.y;
			if (SDL_BlitSurface( Helm.hands, &blip[ bliptouse ], surface, &firehand ) < 0)
				helmrestoreSurfaces();
	}

	for ( i=0; i < MISSLECOUNT; i++ )
	{
		if ( Tank.missle[ i ].type == 23 || Tank.missle[ i ].type == 4 || Tank.missle[ i ].type == 8 )
		{				
			nextblip.x = Tank.missle[ i ].x / SCOPEXRATIO;

			if ( myscopex > SCOPEHALF ) // horizon in front
			{
				vleft = myscopex - SCOPEHALF;
				diffx = SCOPEWIDE - vleft;		 
				if ( nextblip.x > vleft )
					nextblip.x -= vleft;
				else
					nextblip.x += diffx;
			}
			else if ( myscopex < SCOPEHALF ) // horizon behind
			{
				vleft = myscopex + 35;
				diffx = SCOPEWIDE - vleft;
	
				if ( nextblip.x > vleft )
					nextblip.x -= vleft;
				else
					nextblip.x += diffx;
			}
			nextblip.x += SCOPELEFT;
	
			if (Tank.missle[ i ].y <= Player[ Me ].y ) 
				nextblip.y = SCOPECENTERY - (( Player[Me].y - Tank.missle[i].y ) / SCOPEYRATIO); 
			if ( Tank.missle[ i ].y > Player[ Me ].y )
				nextblip.y = SCOPECENTERY + (( Tank.missle[i].y - Player[ Me ].y ) / SCOPEYRATIO); 
					// bottom doesn't wrap
			if ( nextblip.y < SCOPETOP || nextblip.y > SCOPEBOTTOM ) // out of range
				continue;

			switch( Tank.missle[ i ].type )
			{
				case 23: // red
					bliptouse = 0;
					break;
				case 4: // green 
					bliptouse = 1;
				case 8: // blue
					bliptouse = 2;
					break;
				default:
					break;
			}
			firehand.x = nextblip.x + ox;
			firehand.y = nextblip.y;
			if (SDL_BlitSurface( Helm.hands, &mlip[ bliptouse ], surface, &firehand ) < 0)
				helmrestoreSurfaces();
		}
	}
}
//-----------------------------------------------------------------
// RestoreSurfaces
//-----------------------------------------------------------------
void helmrestoreSurfaces( void )
{
	loadbmp( Helm.controls, PKGDATADIR "/bmp/control.bmp", 0 );
	loadbmp( Helm.hands, PKGDATADIR "/bmp/hands.bmp", 0 );
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------

