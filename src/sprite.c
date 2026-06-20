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
// associate a set of rectangles and a bitmap with it's characteristics
//--------------------------------------------------------------------------------
#include "compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "frogs.h"
#include "sprite.h"

//-----------------------------------------------------------------
// void spriteinit( void ) - initialize sprite
//-----------------------------------------------------------------
void spriteinit( SPRITE *s )
{
	s->frame = NULL;
	s->wide = 0;
	s->high = 0;
	s->speed = 0;
	s->framenum=0;
	s->curframe=0;
	s->opaque=1;
   	s->dist=0;
	s->odom=2;
	s->type=0;
	s->noise=0;
	s->pal=0;
}
//-----------------------------------------------------------------
// void sizeFrames(void)
//      build the frameset for this sprite - be sure to set framenum first     
// input:
//          none
// returns:
//          none
//-----------------------------------------------------------------
void spritesizeFrames( SPRITE *s )
{
	if ( s->frame != NULL )
    	free( s->frame );
    s->frame = (RECT *)calloc( s->framenum, sizeof( RECT ));
}
//-----------------------------------------------------------------
// void setFrame( int, int, int, int, int )
//      build the frameset for this sprite - be sure to set framenum first     
// input:
//          index of frame to set
//          rectangle coordinates
// returns:
//          none
//-----------------------------------------------------------------
void spritesetFrame( SPRITE *s, int i, int x, int y, int w, int h )
{
    if ( i < s->framenum )
    {
        s->frame[ i ].x = x;
        s->frame[ i ].y = y;
        s->frame[ i ].w = w;
        s->frame[ i ].h = h;
    }
}
//-----------------------------------------------------------------
void spew(SPRITE *s, int c)
{
	int i;

	for ( i=0; i < c; i++ )
	{
		printf( "sprite: %d x:%d y:%d z:%d p:%d w:%d h:%d o:%d t:%d f:%d r:%d s:%d d:%d o:%d c:%d t:%d n:%d p:%d\n", i, s[i].x, s[i].y, s[i].z, s[i].pindex, s[i].wide, s[i].high, s[i].opaque, s[i].type, s[i].framenum, s[i].rection, s[i].speed, s[i].dist, s[i].odom, s[i].curframe, s[i].ttl, s[i].noise, s[i].pal );
	 }
}

