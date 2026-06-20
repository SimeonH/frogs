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
// the scenery, background, foreground, and walkable surfaces manager
//--------------------------------------------------------------------------------
#include "compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "frogs.h"
#include "util.h"
#include "player.h"
#include "sprite.h"
#include "tank.h"
#include "helm.h"
#include "room.h"
#include "racket.h" // add sounds for room decor

extern SDL_Surface *Screen;
extern PLAYER Player[];
extern ROOM Room;
extern OPTIONS Option;
extern short Me; // my place in player array

//-----------------------------------------------------------------
// room initializer
//-----------------------------------------------------------------
void roominit( void )
{
	Room.show=0;
	Room.change=1;
	Room.maxx=5120;
	Room.maxy=3840;
	Room.decor=0;
	Room.step=0;
	Room.stepCount=0;
	Room.nineCount=0;
	Room.decortop=NULL;
	Room.actionCount=0;
	Room.actionZone=NULL;
	Room.backtile = NULL;
	Room.spriteCount=0;
	Room.backdecor = NULL;
	Room.edgecolor = 252;
	Room.status = 0;
	roomchangeBackground( "mine" );
	return;
}

//-----------------------------------------------------------------
// room destructor
//-----------------------------------------------------------------
void roomdestroy( void )
{
	if ( Room.backtile != NULL )
		SDL_FreeSurface( Room.backtile );
	if ( Room.backdecor != NULL )
		SDL_FreeSurface( Room.backdecor );
	if ( Room.decor != NULL )  
		free( Room.decor );
	if ( Room.step != NULL ) 
		free( Room.step );
	if ( Room.decortop != NULL ) 
		free( Room.decortop );
	if ( Room.actionType != NULL ) 
		free( Room.actionType );
	if ( Room.actionZone != NULL ) 
		free( Room.actionZone );
}

//-----------------------------------------------------------------
// roomdraw
//-----------------------------------------------------------------
void roomdraw( SDL_Surface *surface )
{
	POINT dest = { 0, 0 };  // dest points for clipped tile
	RECT vport; // me's viewport in world coords
	RECT cover; // cover up the ragged edges
	
	if ( Room.change ) // reset changed room flag
	{
		Room.change = 0;
		if ( Room.backtile != NULL )
			SDL_FreeSurface( Room.backtile );
		Room.backtile = SDL_LoadBMP( Room.tilename );
		if ( Room.backdecor != NULL )
			SDL_FreeSurface( Room.backdecor );
		Room.backdecor = SDL_LoadBMP( Room.decorname );
		SDL_SetSurfacePalette(Room.backtile, Screen->format->palette);
		SDL_SetSurfacePalette(Room.backdecor, Screen->format->palette);
		SDL_SetColorKey(Room.backtile, SDL_TRUE, 185 );
		SDL_SetColorKey(Room.backdecor, SDL_TRUE, 185 );
		roomparseSpriteFile( Room.spritename );
	}
	
	vport.h = SCREENHEIGHT - CONTROLSHEIGHT + OVERLAP; // little overlap for effect
	vport.y = Player[ Me ].y - (vport.h / 2);
	vport.x = Player[ Me ].x - HALFSCREENWIDTH;
	vport.w = SCREENWIDTH;
	// printf ( "player x:%d y:%d\n", Player[ Me ].x, Player[ Me ].y);
	// printf( "world vport = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
	if ( vport.y < 0 )
	{
		vport.y = 0;
		dest.y = (( SCREENHEIGHT - CONTROLSHEIGHT ) / 2 ) - abs(Player[ Me ].y);
		dest.y = dest.y < 0 ? 0 : dest.y;
		cover.x = 0;
		cover.w = SCREENWIDTH;
		cover.y = 0;
		cover.h = dest.y;
		//printf("dest - x:%d y:%d\n", dest.x, dest.y );
		SDL_FillRect( surface, &cover, Room.edgecolor );
	}
	else if ( vport.y + vport.h > Room.maxy )
	{
		vport.h = Room.maxy - vport.y;
		cover.x = 0;
		cover.w = SCREENWIDTH;
		cover.y = vport.h;
		cover.h = ( SCREENHEIGHT - CONTROLSHEIGHT ) - cover.y;
		SDL_FillRect( surface, &cover, Room.edgecolor );
	}

	if ( vport.x < 0 ) // do right side
	{
		dest.x = HALFSCREENWIDTH - Player[ Me ].x;
		vport.w =  SCREENWIDTH + vport.x;
		vport.x = 0;
		// printf( "2 x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		roomtileBackground( &dest, &vport, surface );
		vport.w = dest.x; 
		vport.x = Room.maxx - dest.x; // switch to left
	}
	else if ( vport.x + vport.w >= Room.maxx ) // do the right thing
	{
		dest.x = Room.maxx - vport.x;
		vport.w = Room.maxx - vport.x;
		vport.x = 0;
		// printf( "3 x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		roomtileBackground( &dest, &vport, surface );
		vport.x = Room.maxx - dest.x; // switch to left
	}
	dest.x = 0;	  
	// printf( "4 x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
	// printf( " dest = x:%d y:%d\n", dest.x, dest.y); 
	roomtileBackground( &dest, &vport, surface );
}   

//-----------------------------------------------------------------
// tileBackground
//-----------------------------------------------------------------
void roomtileBackground( POINT *split, RECT *vport, SDL_Surface *surface )
{   
	int i, j, txoffset, tyoffset, vxoffset, vyoffset;
	RECT trect = { 0,0,0,0 }; // source tile rect
	RECT drect = { 0,0,0,0 }; // dest tile rect
	POINT dest = { 0,0 }; // dest rect use just x and y - sdl don't know point

	txoffset = vport->x % BACKTILESWIDTH; // offset of top left of tile
	tyoffset = vport->y % BACKTILESHEIGHT;
	vxoffset = BACKTILESWIDTH - txoffset; // offset inside viewport
	vyoffset = BACKTILESHEIGHT - tyoffset;

	dest.y = split->y;
	
	for ( j = 0; (( dest.y + trect.h )<( split->y + vport->h )); j++ ) // for each row
	{
		if ( j > 0 )
			dest.y = vyoffset + split->y;
		if ( j > 1 )
			dest.y = vyoffset + split->y + (( j - 1 ) * BACKTILESHEIGHT );

		dest.x = split->x;
		i = 0;
		do
		{					   
			if ( i > 0 )
				dest.x += vxoffset;
			if ( i > 1 )
				dest.x = vxoffset + (( i - 1 ) * BACKTILESWIDTH ) + split->x;
				
			trect.x = i ? 0 : txoffset;
			trect.w = (( dest.x + BACKTILESWIDTH )>( split->x + vport->w )) ? trect.x + ( split->x + vport->w - dest.x ): BACKTILESWIDTH; 
			trect.y = j ? 0 : tyoffset;
			trect.h = ( dest.y + BACKTILESHEIGHT )>( split->y + vport->h )? trect.y + ( split->y + vport->h - dest.y ) : BACKTILESHEIGHT;

			drect.x = dest.x;
			drect.y = dest.y;
			if ( clip( &drect, &trect ))
				continue;
			SDL_BlitSurface( Room.backtile, &trect, surface, &drect );
			i++;
		} while (( dest.x + trect.w )<( split->x + vport->w ));
	}
	for ( i = 0; i < Room.spriteCount; i++ )
	{
		if ( Room.decor[ i ].x < vport->x + vport->w &&
				Room.decor[ i ].y < vport->y + vport->h &&
				Room.decor[ i ].x + Room.decor[ i ].wide > vport->x &&
				Room.decor[ i ].y + Room.decor[ i ].high  > vport->y )
		{

			if ( Room.decor[ i ].curframe >= Room.decor[ i ].framenum )
				Room.decor[ i ].curframe = 0;
				
			memmove( &trect, &Room.decor[ i ].frame[ Room.decor[ i ].curframe ], sizeof( RECT ));

			trect.w = Room.decor[ i ].wide; 
			trect.h = Room.decor[ i ].high;

			drect.x = ( Room.decor[ i ].x - vport->x ) + split->x;
			drect.y = ( Room.decor[ i ].y - vport->y ) + split->y;
			drect.w = trect.w;
			drect.h = trect.h;

		/*	printf("rdx: %d rdy: %d rdw: %d rdh: %d tx: %d ty: %d tw: %d th: %d dx: %d dy: %d\n",
				Room.decor[ i ].x, Room.decor[ i ].y, 
				Room.decor[ i ].wide, Room.decor[ i ].high,
				trect.x, trect.y, trect.w, trect.h, drect.x, drect.y); */
			if ( clip( &drect, &trect ))
				continue;
		   	SDL_BlitSurface( Room.backdecor, &trect, surface, &drect );

			// start of sprite cycle - make his noise
			if ( Room.decor[ i ].curframe == 0 && Room.decor[ i ].odom == 0 && Room.decor[ i ].noise)
			{
				// printf("d = i:%d c:%d s:%d\n", i, Room.decor[i].curframe, Room.decor[ i ].noise);
				racketplay( Room.decor[ i ].noise, 0 );
			}

			if ( Room.decor[ i ].dist && Room.decor[ i ].odom++ > Room.decor[ i ].dist )
			{
				Room.decor[ i ].curframe++;
				Room.decor[ i ].odom = 0;
			}
			if ( Room.decor[ i ].curframe >= Room.decor[ i ].framenum )
				Room.decor[ i ].curframe = 0;
		}

	}
}

//-----------------------------------------------------------------
// roomdrawtop - add the top level sprites same as roomdraw without changeroom
//-----------------------------------------------------------------
void roomdrawtop( SDL_Surface *surface )
{
	POINT dest = { 0, 0 };  // dest points for clipped tile
	RECT vport; // me's viewport in world coords

	vport.h = SCREENHEIGHT - CONTROLSHEIGHT + OVERLAP; // little overlap for effect
	vport.y = Player[ Me ].y - (vport.h / 2);
	vport.x = Player[ Me ].x - HALFSCREENWIDTH;
	vport.w = SCREENWIDTH;
	// printf ( "player x:%d y:%d\n", Player[ Me ].x, Player[ Me ].y);
	// printf( "world vport = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
	if ( vport.y < 0 )
	{
		vport.y = 0;
		dest.y = ((( SCREENHEIGHT - CONTROLSHEIGHT ) / 2 ) - Player[ Me ].y);
	}
	else if ( vport.y + vport.h > Room.maxy )
	{
		vport.h = Room.maxy - vport.y;
	}

	if ( vport.x < 0 ) // do the right thing
	{
		dest.x = HALFSCREENWIDTH - Player[ Me ].x;
		vport.w =  SCREENWIDTH + vport.x;
		vport.x = 0;

		// printf( "right 1 = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		roomforeground( &dest, &vport, surface );
		vport.w = dest.x; 
		vport.x = Room.maxx - dest.x; // switch to left
	}
	else if ( vport.x + vport.w > Room.maxx ) // because its the right thing to do
	{
		dest.x = Room.maxx - vport.x;
		vport.w = Room.maxx - vport.x;
		vport.x = 0;

		// printf( "right 2 = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		roomforeground( &dest, &vport, surface );
		vport.w = Room.maxx - vport.x;
		vport.x = Room.maxx - dest.x; // switch to left
	}
	dest.x = 0;	  
	 // printf( "left vprt = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
	 // printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
	roomforeground( &dest, &vport, surface );
}   

//-----------------------------------------------------------------
// roomforeground - draw the level nine sprites
//-----------------------------------------------------------------
void roomforeground( POINT *split, RECT *vport, SDL_Surface *surface )
{   
	int i;
	RECT trect = { 0,0,0,0 }; // source tile rect
	RECT drect = { 0,0,0,0 }; // dest tile rect

	for ( i = 0; i < Room.nineCount; i++ )
	{
		if (( Room.decortop[ i ].x < vport->x + vport->w )&&
				( Room.decortop[ i ].y < vport->y + vport->h )&&
				(( Room.decortop[ i ].x + Room.decortop[ i ].wide ) > vport->x )&&
				(( Room.decortop[ i ].y + Room.decortop[ i ].high ) > vport->y ) )
		{
			if ( Room.decortop[ i ].curframe >= Room.decortop[ i ].framenum )
				Room.decortop[ i ].curframe = 0;
				
			memmove( &trect, &Room.decortop[ i ].frame[ Room.decortop[ i ].curframe ], sizeof( RECT ));
			trect.w = Room.decortop[ i ].wide; 
			trect.h = Room.decortop[ i ].high;

			drect.x = ( Room.decortop[ i ].x - vport->x ) + split->x;
			drect.y = ( Room.decortop[ i ].y - vport->y ) + split->y;
			drect.w = trect.w;
			drect.h = trect.h;

	 		// printf( "nine = x:%d y:%d w:%d h:%d\n", trect.x, trect.y, trect.w, trect.h);
	 		// printf( "drect = x:%d y:%d\n", drect.x, drect.y); 

			if ( clip( &drect, &trect ))
				continue;
			SDL_BlitSurface( Room.backdecor, &trect, surface, &drect );

			 // start of sprite cycle - make his noise
			if ( Room.decortop[ i ].noise && Room.decortop[i].curframe == 0 && Room.decortop[i].odom == 0)
			{
		// printf("t = i:%d c:%d s:%d\n", i, Room.decortop[i].curframe, Room.decortop[ i ].noise);
				racketplay( Room.decortop[ i ].noise, 0 );
			}

			if ( Room.decortop[ i ].dist && Room.decortop[ i ].odom++ > Room.decortop[ i ].dist )
			{
				Room.decortop[ i ].curframe++;
				Room.decortop[ i ].odom = 0;
			}
			if ( Room.decortop[ i ].curframe >= Room.decortop[ i ].framenum )
				Room.decortop[ i ].curframe = 0;
		}
	}
}

//-----------------------------------------------------------------
// changeBackground - takes room name and adds prefixes for room parts
// usage: changeBackground("sea");
//-----------------------------------------------------------------
void roomchangeBackground( char *newtilepath )
{
	sprintf( Room.tilename, PKGDATADIR "/bmp/%st.bmp", newtilepath );
	sprintf( Room.decorname, PKGDATADIR "/bmp/%sd.bmp", newtilepath );
	sprintf( Room.spritename, PKGDATADIR "/bmp/%s.elf", newtilepath );
	Room.change = 1;
}

//-----------------------------------------------------------------
// solidGround( int, int ) check steps to see if gravity should take over
//-----------------------------------------------------------------
short roomsolidGround( int x, int y )
{
	short i, hit = 0;

	if ( y >= Room.maxy - FAKEFLOOR ) // just for looks - see also playeradjust
		hit++;
	for ( i = 0; i < Room.stepCount && !hit; i++ )
	{
		if ( y == Room.step[ i ].y && x >= Room.step[ i ].x && x <= ( Room.step[ i ].x + Room.step[ i ].len ))
			 hit++;
	}
	return hit;
}

//-----------------------------------------------------------------
// setStep - declare a walking surface of z length at x and y coords
// at index i
//-----------------------------------------------------------------
void roomsetStep( int i, int x, int y, int z )
{
	Room.step[ i ].x = x;
	Room.step[ i ].y = y;
	Room.step[ i ].len = z;
}

//-----------------------------------------------------------------
// restoreSurfaces
//-----------------------------------------------------------------
void roomrestoreSurfaces( void )
{
   Room.change = 1;
}

//-----------------------------------------------------------------
// int nextCommand( char *instr, char *outstr, char *place, int len )
//	  find and retrieve a command string from instr starting at place
//	  strip cr/lf/comments, add a terminating comma and a null
// input:
//	  char *instr - compound string full of comments
//	  char *outstr - null terminated command string a-z:#,#,#,\0
// returns:
//	  0 - success
//	  !0 - failure
//----------------------------------------------------------------
int roomnextCommand( char *place, char *outstr )
{
	char *x = place;
	char *y = outstr;
	int found = 0; // increment if command found
		
	while( *x++ && !found ) // find start of a command
	{
		if ( *x == ':' )
		{
			--x;
			if ( strchr( "eq#sf", (int)*x ) != NULL ) // got one
			{
				found++;
				while( *x && strchr( " \t\r\n\\", (int)*x ) == NULL )  // fill output
					*y++ = *x++;
				*y++ = ',';
				*y++ = '\0';
			}
			else
				while( *x != '\n' )  // skip it
					;
		}
	}
			
	return found;
}
//eo nextCommand

//-----------------------------------------------------------------
// parseSpriteFile - opens, reads and builds a sprite list from a sprite description file
//input:
//	  sprite *Decor- Decor of sprite array
//	  char *fname - sprite config filename as defined in sprite.h
//returnS:
//	  int 0 - success
//	  !0 - failure - bad value encountered etc.
//-----------------------------------------------------------------
int roomparseSpriteFile( char *elfname )
{
	int rfile;
	off_t sizeofile, actual;
	char comstr[256] = "";
	char *text = NULL;
	char *place;
	char *nextarg;
	int retval = 0, elfnum = 0, flooring = 0, topnum = 0, thiself = 0, actnum = 0, lastarg = 0;
	int i, j, cmd, left, top, right, bottom, plusx, plusy, len, x, y, z;
	
	rfile = open( elfname,  O_RDONLY | O_BINARY );
	if ( rfile < 1 )
			retval = 1;
	if ( ! retval )
	{
		sizeofile = lseek( rfile, 0L, SEEK_END );
		text = (char *)calloc( sizeofile + 1, sizeof(char));
		text[ sizeofile ] = '\0';
		place = text;
		lseek( rfile, 0L, SEEK_SET );
		actual = read( rfile, text, sizeofile );
		if ( actual > 0 )
		{
			while( roomnextCommand( place, comstr )) // first how many 9 level and how big a room
			{
				place += strlen( comstr );
				while ( *place && *place != '\n' ) // strip empty lines
					place++;
				nextarg = strtok( comstr, ":" );
				cmd = (int)comstr[0];
				switch( cmd )
				{
					case 35: // # spritecount
						nextarg = strtok( NULL, "," );
						Room.spriteCount = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Room.maxx = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Room.maxy = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Room.stepCount = atoi( nextarg ); // no level nine steps
						fprintf( stderr, "room step count: %d\n", Room.stepCount );
						if ( Room.step != NULL )
							free( Room.step );
						Room.step = (STEP *)calloc( Room.stepCount + 1, sizeof( STEP ));
						break;

					case 101: // e elf
						nextarg = strtok( NULL, "," );
						nextarg = strtok( NULL, "," );
						nextarg = strtok( NULL, "," );
						if ( atoi( nextarg ) == 9 )
							Room.nineCount++;
						nextarg = strtok( NULL, "," );
						nextarg = strtok( NULL, "," );
						lastarg = atoi( nextarg );
						if ( lastarg > 0 )
							Room.actionCount++;							
						break;
						
					case 115: // s  steppit
						nextarg = strtok( NULL, "," );
						len = atoi( nextarg );
						if ( lastarg > 0 )
							Room.actionCount += len;
					default:
						break;
				}
			}
				// now we know how many background and how many foreground level nine

			Room.spriteCount -= Room.nineCount; 
			fprintf( stderr, "room sprite count: %d\n", Room.spriteCount );
			if ( Room.decor != NULL )
				free ( Room.decor );
			Room.decor = (SPRITE *)calloc( Room.spriteCount + 1, sizeof(SPRITE));
			
			fprintf( stderr, "room nine count: %d\n", Room.nineCount );
			if ( Room.decortop != NULL )
				free ( Room.decortop );
			Room.decortop = (SPRITE *)calloc( Room.nineCount + 1, sizeof(SPRITE));

			place = text; // rewind input file

			while( roomnextCommand( place, comstr ))
			{
				place += strlen( comstr );
				while ( *place && *place != '\n' ) // strip empty lines
					place++;
				nextarg = strtok( comstr, ":" );
				cmd = (int)comstr[0];
				
				switch( cmd )
				{
					case 101: // e elf
						nextarg = strtok( NULL, "," );
						Room.decor[ elfnum ].x = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Room.decor[ elfnum ].y = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Room.decor[ elfnum ].z = atoi( nextarg );
						if ( Room.decor[ elfnum ].z == 9 )
						{
							thiself = 9;
							Room.decortop[ topnum ].x = Room.decor[ elfnum ].x;
							Room.decortop[ topnum ].y = Room.decor[ elfnum ].y;
							Room.decortop[ topnum ].z = Room.decor[ elfnum ].z;
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].opaque = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].type = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].framenum = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].rection = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].speed = atoi( nextarg );					  
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].dist = atoi( nextarg );
							Room.decortop[ topnum ].curframe = 0;
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].ttl = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decortop[ topnum ].noise = atoi( nextarg );

							spritesizeFrames( &Room.decortop[ topnum ] );
							topnum++;
						}
						else
						{
							thiself = 0;			 
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].opaque = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].type = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].framenum = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].rection = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].speed = atoi( nextarg );					  
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].dist = atoi( nextarg );
							Room.decor[ elfnum ].curframe = 0;
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].ttl = atoi( nextarg );
							nextarg = strtok( NULL, "," );
							Room.decor[ elfnum ].noise = atoi( nextarg );

							spritesizeFrames( &Room.decor[ elfnum ]);
							elfnum++;
						}
						break;
						
					case 113: // q  quadrant
						nextarg = strtok( NULL, "," );
						left = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						top = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						right = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						bottom = atoi( nextarg );
						
						if ( !thiself )
						{
							Room.decor[ elfnum - 1 ].wide = right - left;
							Room.decor[ elfnum - 1 ].high = bottom - top;
							spritesetFrame( &Room.decor[ elfnum -1 ], Room.decor[ elfnum -1 ].curframe++, left, top, right, bottom );
						}
						else
						{
							Room.decortop[ topnum - 1 ].wide = right - left;
							Room.decortop[ topnum - 1 ].high = bottom - top;
							spritesetFrame( &Room.decortop[ topnum - 1 ],Room.decortop[ topnum - 1 ].curframe++, left, top, right, bottom );
						}
						break;
						
					case 115: // s  steppit
						nextarg = strtok( NULL, "," );
						len = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						plusx = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						plusy = atoi( nextarg );
						for ( i = 0; i < len; i++ )
						{
							Room.decor[ elfnum ].x = Room.decor[ elfnum - 1 ].x + plusx;
							Room.decor[ elfnum ].y = Room.decor[ elfnum - 1 ].y + plusy;
							Room.decor[ elfnum ].z = Room.decor[ elfnum - 1 ].z;
							Room.decor[ elfnum ].opaque = Room.decor[ elfnum - 1 ].opaque;
							Room.decor[ elfnum ].type = Room.decor[ elfnum - 1 ].type;
							Room.decor[ elfnum ].framenum = Room.decor[ elfnum - 1 ].framenum;
							Room.decor[ elfnum ].rection = Room.decor[ elfnum - 1 ].rection;
							Room.decor[ elfnum ].speed = Room.decor[ elfnum - 1 ].speed;
							Room.decor[ elfnum ].dist = Room.decor[ elfnum - 1 ].dist;
							Room.decor[ elfnum ].curframe = Room.decor[ elfnum - 1 ].curframe;
							Room.decor[ elfnum ].ttl = Room.decor[ elfnum - 1 ].ttl;
							spritesizeFrames( &Room.decor[ elfnum ]);
							for ( j = 0; j < Room.decor[ elfnum ].framenum; j++ )
							{
								spritesetFrame( &Room.decor[ elfnum ], j,
												Room.decor[ elfnum -1 ].frame[ j ].x, 
												Room.decor[ elfnum -1 ].frame[ j ].y,
												Room.decor[ elfnum -1 ].frame[ j ].w,
												Room.decor[ elfnum -1 ].frame[ j ].h );
							}
							Room.decor[ elfnum ].wide = Room.decor[ elfnum - 1 ].wide;
							Room.decor[ elfnum ].high = Room.decor[ elfnum - 1 ].high;
							elfnum++;
						}
						break;

					case 102: // f floor
						nextarg = strtok( NULL, "," );
						x = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						y = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						z = atoi( nextarg );
						roomsetStep( flooring, x, y, z );
						flooring++;
						break;

					default:
						break;
				}
			} // eowhile

				// now we're ready to build array of action zones from type > 0 background sprites
				// and a corresponding array of action types 
			if ( Room.actionZone != NULL );  
				free( Room.actionZone );
			Room.actionZone = (RECT *)calloc( Room.actionCount + 1,  sizeof(RECT));
			if ( Room.actionType != NULL );  
				free( Room.actionType );
			Room.actionType = (short*)calloc( Room.actionCount + 1, sizeof(short));
			// safety first
			for ( i = 0, actnum = 0; i < Room.spriteCount && actnum < Room.actionCount; i++ )
			{
				if ( Room.decor[ i ].type > 0 )
				{
					Room.actionType[ actnum ] = Room.decor[ i ].type;
					Room.actionZone[ actnum ].x = Room.decor[ i ].x;
					Room.actionZone[ actnum ].w = Room.decor[ i ].wide;
					Room.actionZone[ actnum ].y = Room.decor[ i ].y;
					Room.actionZone[ actnum ].h = Room.decor[ i ].high;
					actnum++;
				}
			}
				// leave actnum as is to fill rest of zone/types
			for ( i = 0; i < Room.nineCount && actnum < Room.actionCount; i++ )
			{
				if ( Room.decortop[ i ].type > 0 )
				{
					Room.actionType[ actnum ] = Room.decortop[ i ].type;
					Room.actionZone[ actnum ].x = Room.decortop[ i ].x;
					Room.actionZone[ actnum ].w = Room.decortop[ i ].wide;
					Room.actionZone[ actnum ].y = Room.decortop[ i ].y;
					Room.actionZone[ actnum ].h = Room.decortop[ i ].high;
					actnum++;
				}
			}			  
		}	 
		else
		{
			if ( ! retval )
				retval = 2;
		}
		
		free( text );
		close( rfile );
	}
	
	return retval;

}//parseSpriteFile

//-----------------------------------------------------------------
// collide - check and see if we've plowed into any scenery type 3
//		  set guy to proper animation sequence and return 0
//-----------------------------------------------------------------
int roomcollide( void )
{
	POINT hot; // sprite hotspot
	RECT guyrect; // players current space
	int i, j;
						
	for ( j = 0; j < PLAYERMAX; j++ )
	{
		for ( i = 0; i < Room.actionCount; i++ ) // plowed into scenery type
		{
			hot.x = Room.actionZone[ i ].x + ( Room.actionZone[ i ].w / 2 );
			hot.y = Room.actionZone[ i ].y + ( Room.actionZone[ i ].h / 2 ); // center
			guyrect.x = Player[ j ].x - tankhalfwide( j );
			guyrect.w = Player[ j ].x + tankhalfwide( j );
			guyrect.y = Player[ j ].y - tankhalfhigh( j );
			guyrect.h = Player[ j ].y + tankhalfhigh( j );
			
			if ( pointinrect( &hot, &guyrect ))   
			{							   // guy changes 
				switch( Room.actionType[ i ] ) // the effects of hitting scenery
				{
					case 1:	 // the orb!
						tanksetPlayerSprite( j, STOPTOAD ); 
						Player[ j ].shield = SHIELDMAX;
						Player[ j ].speed = 0;
						break;

					case 2: // radioactive lava
						Player[ j ].power = POWERMAX;
						break;

					default:
						break;
				}
			}

		}

	}
	return 0;
}
//-----------------------------------------------------------------
// roomdump - seein what we eatin
//-----------------------------------------------------------------
void roomdump()
{
	int i, j;
	printf( "max: X: %d Y: %d  name: %s\n", Room.maxx, Room.maxy, Room.decorname);
	printf( "sc: %d nc: %d\n", Room.spriteCount, Room.nineCount);
	for ( i=0; i < Room.spriteCount; i++)
	{
		printf( "decor = X: %d Y: %d Z: %d W: %d H: %d T: %d F: %d\n",
			Room.decor[i].x, Room.decor[i].y, Room.decor[i].z, Room.decor[i].wide, 
			Room.decor[i].high, Room.decor[i].type, Room.decor[i].framenum);
		for ( j=0; j < Room.decor[i].framenum; j++ )
		{
			printf( "frame = X: %d Y: %d W: %d H: %d\n", 
				Room.decor[i].frame[j].x, Room.decor[i].frame[j].y,
				Room.decor[i].frame[j].w, Room.decor[i].frame[j].h);
		}
	}
	for ( i=0; i < Room.nineCount; i++)
	{
		printf( "nine = X: %d Y: %d Z: %d W: %d H: %d T: %d F: %d\n",
			Room.decortop[i].x, Room.decortop[i].y, Room.decortop[i].z, Room.decortop[i].wide, 
			Room.decortop[i].high, Room.decor[i].type, Room.decor[i].framenum);
		for ( j=0; j < Room.decor[j].framenum; j++ )
		{
			printf( "X: %d Y: %d W: %d H: %d\n", 
				Room.decortop[i].frame[j].x, Room.decor[i].frame[j].y,
				Room.decortop[i].frame[j].w, Room.decor[i].frame[j].h);
		}
	}
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
