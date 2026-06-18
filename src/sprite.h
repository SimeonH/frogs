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
// types
#define ORNAMENT 0
#define PAIN 1
#define POWER 2
#define MUERTE 4
#define GEEK 5

typedef struct 
{
	short x; // location in room
	short y;
	short z;
	short pindex; // owner
	short type; // used to track the original index after copied into the missle array
	short rection; // according to numpad compass 5 = no rection
	short curframe; // current frame -what is this sprite doing now
	short odom; //  times shown before incrementing curframe
	short ttl; // time to live before death or metamorphosis 0 = never
	//-- static data begin
	short wide;
	short high;
	short opaque; // transparency
	short framenum; // number of frames
	short speed; // number of pixels to jump
	short dist; // when = odom curframe++
	short noise; // index to racket object array of sounds
	short pal; // palette for effect 0 is normal 1 red 2 green 3 blue - type 5 only
	RECT *frame; // framenum sized array of frame rectangles of surface -left top right bottom
} SPRITE;

typedef struct // tank->missle array data changes sent by server in datatoad msg
{			  // the rest is static so we can get tank->sprite[ type ] data
	short x;
	short y;
	short z;
	short pindex;
	short type;
	short rection;
	short curframe;
	short odom;
	short ttl;
} SPRITEDATA;


void spritesizeFrames( SPRITE * ); // allocate framenum rectangles at *frame
void spritesetFrame( SPRITE *, int, int, int, int, int ); // index and four coordinates
void spriteinit( SPRITE * );
void spew(SPRITE *, int);
