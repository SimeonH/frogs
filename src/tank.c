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
// ties together player, room, sounds, images as active sprites
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
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
#include "helm.h"
#include "sprite.h"
#include "room.h"
#include "tank.h"
#include "racket.h"

extern PLAYER Player[];
extern ROOM Room;
extern TANKS Tank;
extern OPTIONS Option;
extern short Me; // my place in player array
extern SDL_Surface *Screen;
extern short CurrentPalette;

//-----------------------------------------------------------------
// tankinit - called only once
//-----------------------------------------------------------------
void tankinit( void )
{
	Tank.sprite = NULL;
	Tank.spritecount = 0;
	Tank.missle = NULL;
	Tank.misslecount = 0;
	Tank.tanktile = NULL;
	strcpy( Tank.bmpname, PKGDATADIR "/bmp/tank.bmp" );
	strcpy( Tank.spritename, PKGDATADIR "/bmp/tank.elf" );
	/* see also tank change */
	Tank.missle = (SPRITE *)calloc( MISSLECOUNT + 1, sizeof(SPRITE));
	Tank.tanktile = SDL_LoadBMP( Tank.bmpname );
	SDL_SetSurfacePalette(Tank.tanktile, Screen->format->palette);
	SDL_SetColorKey(Tank.tanktile, SDL_TRUE, 185 );
	tankparseSpriteFile( Tank.spritename );
	fprintf( stderr, "tank count: %d\n", Tank.spritecount);
	Tank.change = 0;
	Tank.show = 1;
}

//-----------------------------------------------------------------
// cleanup sprite arrays
//-----------------------------------------------------------------
void tankdestroy( void )
{
	if ( Tank.tanktile != NULL )
	{
		SDL_FreeSurface( Tank.tanktile );
		Tank.tanktile = NULL;
	}

	if ( Tank.missle != NULL )
	{
		free( Tank.missle );
		Tank.missle = NULL;
		Tank.misslecount = 0;
	}

	if ( Tank.sprite != NULL )
	{
		free( Tank.sprite );
		Tank.sprite = NULL;
		Tank.spritecount = 0;
	}
}

//-----------------------------------------------------------------
// draw the players and missles
//-----------------------------------------------------------------
void tankdraw( SDL_Surface *surface )
{
	RECT vport = { 0,0,0,0 }; // my viewport
	POINT dest = { 0,0 };  // dest points to split in screen if !0
   
	if ( Tank.change ) // reset changed tanks flag 
	{
		tankdestroy();
		Tank.missle = (SPRITE *)calloc( MISSLECOUNT + 1, sizeof(SPRITE));
		Tank.tanktile = SDL_LoadBMP( Tank.bmpname );
		SDL_SetSurfacePalette(Tank.tanktile, Screen->format->palette);
		SDL_SetColorKey(Tank.tanktile, SDL_TRUE, 185 );
		tankparseSpriteFile( Tank.spritename );
		fprintf( stderr, "tank count: %d\n", Tank.spritecount);
		Tank.change = 0;
	}

	vport.h = SCREENHEIGHT - CONTROLSHEIGHT + OVERLAP; // little overlap for effect
	vport.y = Player[ Me ].y - (vport.h / 2);
	vport.x = Player[ Me ].x - HALFSCREENWIDTH;
	vport.w = SCREENWIDTH;
	// printf ( "player x:%d y:%d\n", Player[ Me ].x, Player[ Me ].y);
	// printf( "world vport = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
//----------------
	if ( vport.y < 0 )
	{
		vport.y = 0;
		dest.y = (( SCREENHEIGHT - CONTROLSHEIGHT ) / 2 ) - abs(Player[ Me ].y);
	}
	if ( vport.y + vport.h > Room.maxy )
	{
		vport.h = Room.maxy - vport.y;
	}
	if ( vport.x < 0 ) // do right side
	{
		dest.x = HALFSCREENWIDTH - Player[ Me ].x;
		vport.w =  SCREENWIDTH + vport.x;
		vport.x = 0;

        //printf( "2 = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		//printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		tankdrawPane( &dest, &vport, surface );
		vport.w = dest.x;
		vport.x = Room.maxx - dest.x;
	}

	if ( vport.x + vport.w >= Room.maxx ) // ach again with the right thing
	{
		dest.x = Room.maxx - vport.x;
		vport.x = 0;
		vport.w = vport.x + vport.w - Room.maxx;

        // printf( "3 = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
		// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
		tankdrawPane( &dest, &vport, surface );
		vport.w = Room.maxx - vport.x;
		vport.x = Room.maxx - dest.x;
	}
	dest.x = 0;
	// printf( "5 = x:%d y:%d w:%d h:%d\n", vport.x, vport.y, vport.w, vport.h);
	// printf( "dest = x:%d y:%d\n", dest.x, dest.y); 
	tankdrawPane( &dest, &vport, surface );
}

//-----------------------------------------------------------------
// drawpane - tile background and clips, flops missles as needed
//-----------------------------------------------------------------
void tankdrawPane( POINT *split, RECT *vport, SDL_Surface *surface )
{
	int i;
	RECT trect = { 0,0,0,0 }; // source tile rect
	RECT drect = { 0,0,0,0 }; // destination viewport
	
	// printf( "vport = x:%d y:%d w:%d h:%d\n", vport->x, vport->y, vport->w, vport->h);
	// printf( "split = x:%d y:%d\n", split->x, split->y); 
	//
	for ( i = 0; i < PLAYERMAX; i++ )
	{
		if ( Player[ i ].id == -2 ) // net player not filled yet
			continue;
		if ( Player[ i ].curframe == 0 ) // start of sprite cycle 
		{

			if ( Tank.sprite[ Player[ i ].guy ].noise ) // make his noise
			{
				// printf( "p = i:%d c:%d s:%d\n", i, Player[i].curframe, Tank.sprite[Player[i].guy].noise); 
				racketplay( Tank.sprite[ Player[ i ].guy ].noise, 0 );
			}

			if ( i == Me && CurrentPalette != Tank.sprite[ Player[ i ].guy ].pal )
			{
				CurrentPalette = Tank.sprite[ Player[ i ].guy ].pal;
				setPaletteColor( surface, CurrentPalette ); // palette effect
			}
		}

		if ( Player[ i ].x < vport->x + vport->w &&
			 Player[ i ].y < vport->y + vport->h &&
			 Player[ i ].x  + Tank.sprite[ Player[ i ].guy ].wide > vport->x &&
			 Player[ i ].y + Tank.sprite[ Player[ i ].guy ].high > vport->y )
		{

			memmove( &trect, &Tank.sprite[ Player[ i ].guy ].frame[ (short)Player[ i ].curframe ], sizeof( RECT ));
			drect.x = Player[ i ].x  - vport->x + split->x - tankhalfwide( i );
			drect.y = Player[ i ].y - vport->y + split->y - tankhalfhigh( i );
			drect.w = trect.w;
			drect.h = trect.h;
			if ( clip( &drect, &trect ))
				continue;

			if ( Player[ i ].rection == 7 || Player[ i ].rection == 4 || Player[ i ].rection == 1 )
			{
				SDL_BlitFlop( Tank.tanktile, &trect, surface, &drect);
			}
			else
			{
				SDL_BlitSurface( Tank.tanktile, &trect, surface, &drect);
			}
		}
	}

	for ( i = 0; i < MISSLECOUNT; i++ )
	{
		if ( Tank.missle[ i ].type == 0 )
			continue;

		if (( Tank.missle[ i ].x < vport->x + vport->w && 
				Tank.missle[ i ].y < vport->y + vport->h )&&
				(( Tank.missle[ i ].y + Tank.sprite[ Tank.missle[ i ].type ].high ) > vport->y &&
				( Tank.missle[ i ].x + Tank.sprite[ Tank.missle[ i ].type ].wide ) > vport->x ))
		{
			memmove( &trect, &Tank.sprite[ Tank.missle[ i ].type ].frame[ Tank.missle[ i ].curframe ], sizeof( RECT ));	  
			
			drect.x = Tank.missle[ i ].x - vport->x + split->x - (trect.w / 2 );
			drect.y = Tank.missle[ i ].y - vport->y + split->y - (trect.h / 2 );
			drect.w = trect.w;
			drect.h = trect.h;
			if ( clip( &drect, &trect ))
				continue;
		if ( Tank.missle[ i ].rection == 4 )
			{
				SDL_BlitFlop( Tank.tanktile, &trect, surface, &drect);
			}
			else
			{
				SDL_BlitSurface( Tank.tanktile, &trect, surface, &drect );
			}
			if ( Tank.sprite[ Tank.missle[ i ].type ].noise && Tank.missle[ i ].curframe == 0 ) 
			{
			// printf( "r = t:%d c:%d s:%d\n", Tank.missle[ i ].type, Tank.missle[i].curframe, Tank.sprite[Tank.missle[i].type].noise ); 
				racketplay( Tank.sprite[ Tank.missle[ i ].type ].noise, 0 );
			}
		}
	}
}

//-----------------------------------------------------------------
// addMissle - add a missle sprite and make it active
//-----------------------------------------------------------------
void tankaddMissle( int numb, int dx, int dy, int drection, int pindex )
{
	int i;
	
	// fprintf( stderr, "am = n:%d x:%d y:%d r:%d p:%d\n", numb, dx, dy, drection, pindex);
	for ( i = 0; i < MISSLECOUNT; i++ )
	{
		if ( Tank.missle[ i ].type == 0 )
		{
			if ( drection == 7 || drection == 4 || drection == 1 ) // left
				Tank.missle[ i ].x = dx - Tank.sprite[ numb ].x;
			else
				Tank.missle[ i ].x = dx + Tank.sprite[ numb ].x;
			if ( drection == 7 || drection == 8 || drection == 9 ) // up
				Tank.missle[ i ].y = dy - Tank.sprite[ numb ].y;
			else
				Tank.missle[ i ].y = dy + Tank.sprite[ numb ].y;
			Tank.missle[ i ].z = Tank.sprite[ numb ].z;
			Tank.missle[ i ].pindex = pindex; // he who smelt it
			Tank.missle[ i ].type = numb;
			Tank.missle[ i ].rection = drection;
			Tank.missle[ i ].curframe = 0;
			Tank.missle[ i ].odom = Tank.sprite[ numb ].odom;
			Tank.missle[ i ].ttl = Tank.sprite[ numb ].ttl;
			return;
		}
	}
}
//-----------------------------------------------------------------
// adjustMissle - missle move/change/morph/fade/vanish - life cycle
//-----------------------------------------------------------------
void tankadjustMissles( void )
{
	int i;

	Tank.misslecount = 0;	
	for ( i = 0; i < MISSLECOUNT; i++ )
	{
		if ( Tank.missle[ i ].type )
		{
			Tank.misslecount++;	 
			if ( Tank.missle[ i ].ttl-- < 1 ) 
			{
				switch( Tank.missle[ i ].type ) // what kind of tool am i
				{
					case 3: // small slime turn to big slime
						tankaddMissle( 4, Tank.missle[ i ].x, Tank.missle[ i ].y, Tank.missle[ i ].rection, Tank.missle[ i ].pindex );
						Tank.missle[ i ].type = 0;
						break;
					case 4: // big slime turn to dust
						tankaddMissle( 16, Tank.missle[ i ].x, Tank.missle[ i ].y, Tank.missle[ i ].rection, Tank.missle[ i ].pindex );
						Tank.missle[ i ].type = 0;
						break;
					case 8: // blue zap turn to smoke
						tankaddMissle( 17, Tank.missle[ i ].x, Tank.missle[ i ].y, Tank.missle[ i ].rection, Tank.missle[ i ].pindex );
						Tank.missle[ i ].type = 0;
						break;
					case 27: // small blue zap go big
						tankaddMissle( 8, Tank.missle[ i ].x, Tank.missle[ i ].y, Tank.missle[ i ].rection, Tank.missle[ i ].pindex );
						Tank.missle[ i ].type = 0;
						break;
					default: // just disappear
						Tank.missle[ i ].type = 0;
						break;
				}
			}
			 
			if ( Tank.missle[ i ].rection == 7 || Tank.missle[ i ].rection == 9 || Tank.missle[ i ].rection == 8 )// up
				Tank.missle[ i ].y -= Tank.sprite[ Tank.missle[ i ].type ].speed;
			if ( Tank.missle[ i ].rection == 1 || Tank.missle[ i ].rection == 3 || Tank.missle[ i ].rection == 2 )// down
				Tank.missle[ i ].y += Tank.sprite[ Tank.missle[ i ].type ].speed;
			if ( Tank.missle[ i ].rection == 4 || Tank.missle[ i ].rection == 7 || Tank.missle[ i ].rection == 1 ) // 4 7 1 = LEFT
				Tank.missle[ i ].x -= Tank.sprite[ Tank.missle[ i ].type ].speed;
			if ( Tank.missle[ i ].rection == 6 || Tank.missle[ i ].rection == 9 || Tank.missle[ i ].rection == 3 ) // 3 6 9 = RIGHT
				Tank.missle[ i ].x += Tank.sprite[ Tank.missle[ i ].type ].speed;   


			if ( Tank.missle[ i ].x > Room.maxx - 1 ) // stay on the map
				Tank.missle[ i ].x = 0;
			if ( Tank.missle[ i ].x < 0 )
				Tank.missle[ i ].x = Room.maxx - 1;

			if ( Tank.sprite[ Tank.missle[ i ].type ].dist && Tank.missle[ i ].odom++ > Tank.sprite[ Tank.missle[ i ].type ].dist ) // animation adjustments
			{
				Tank.missle[ i ].curframe++;
				Tank.missle[ i ].odom = 0;
			}
			if ( Tank.missle[ i ].curframe >= Tank.sprite[ Tank.missle[ i ].type ].framenum )
			{
				Tank.missle[ i ].curframe = 0;
			}
		}
	}
	return;
}
//-----------------------------------------------------------------
// adjustPlayers
//-----------------------------------------------------------------
void tankadjustPlayers( void )
{
	int i, j, xx, yy;
	
	for ( i = 0; i < PLAYERMAX; i++ )
	{
		if ( Player[ i ].id < -1 )  // nobot
			continue;

		if ( Player[ i ].x > Room.maxx )
			xx = Player[ i ].x - Room.maxx;
		else
			xx = Player[ i ].x;
		yy = Player[ i ].y + tankhalfhigh( i );
	
		if ( Player[ i ].guy != JUMPTOAD && Player[ i ].guy != DUSTTOAD && Player[ i ].guy != FLIPTOAD ) // as I slip the surly bonds of earth...
		{
			for ( j = 0; j < HIGEAR; j++ ) // gravity check
			{
				if( ! roomsolidGround( xx, yy ))
				{
					yy++;
					Player[ i ].y++;
				}
				else
				{
					j = HIGEAR; // bail
					if ( Player[ i ].rection == 1 ) // stop falling now
						Player[ i ].rection = 4;
					else if ( Player[ i ].rection == 3 )
						Player[ i ].rection = 6;
				}				
			}
		}
											
		if ( Player[ i ].rection == 7 || Player[ i ].rection == 9 || Player[ i ].rection == 8 )// up
			Player[ i ].y -= Player[ i ].speed ? Player[ i ].speed : LOGEAR; // whatever default speed is when jumping

		if ( Player[ i ].rection == 4 || Player[ i ].rection == 7 || Player[ i ].rection == 1 ) // 4 7 1 = LEFT
			Player[ i ].x -= Player[ i ].speed;
		if ( Player[ i ].rection == 6 || Player[ i ].rection == 9 || Player[ i ].rection == 3 ) // 3 6 9 = RIGHT
			Player[ i ].x += Player[ i ].speed;   

		if (( Player[ i ].guy == DUSTTOAD )&&( Player[ i ].rection == 1 || Player[ i ].rection == 3 || Player[ i ].rection == 2 ))// down
			Player[ i ].y += Player[ i ].speed ? Player[ i ].speed : LOGEAR;

		if ( Player[ i ].x > Room.maxx - 1 ) // wrap around the map
			Player[ i ].x = 0;
		if ( Player[ i ].x < 0 )
			Player[ i ].x = Room.maxx - 1;
		
		if ( Player[ i ].y + tankhalfhigh( i ) > Room.maxy - FAKEFLOOR ) // from room.h
			Player[ i ].y = Room.maxy - tankhalfhigh( i ) - FAKEFLOOR; // artificial floor

		if ( Player[ i ].y < CEILING )
			Player[ i ].y += (Player[ i ].speed ? Player[ i ].speed : LOGEAR) * 2; // ceiling bounce

		if ( Player[ i ].dist && Player[ i ].odom++ > Player[ i ].dist ) // animation adjustments
		{
			// printf( "inc c:%d o:%d d:%d\n",Player[i].curframe,Player[i].odom,Player[i].dist);
			Player[ i ].curframe++;
			Player[ i ].odom = 0;
		}
		if ( Player[ i ].curframe >= Tank.sprite[ Player[ i ].guy ].framenum )
			Player[ i ].curframe = 0;

		if ( Player[ i ].power > POWERMIN && Player[ i ].speed == HIGEAR ) // power guage
			Player[ i ].power -= MOVELOSS;
			
		if ( Player[ i ].inertia ) // when to switch sprites
			Player[ i ].inertia--;
									// walktoad never ends
		if ( Player[ i ].inertia == 0 && Player[ i ].guy != DUSTTOAD )   // end of the line for current sprite
		{
			// check for burn out
			if ( Player[ i ].shield < 19 ) 
			{
				tanksetPlayerSprite( (short)i, DUSTTOAD );
				Player[ i ].speed = ALLSTOP;
				tankaddMissle( 13, Player[ i ].x, Player[ i ].y, 5, i ); // blam
				
				tankaddMissle( 14, Player[ i ].x, Player[ i ].y, 6, i ); // thigh
				tankaddMissle( 15, Player[ i ].x, Player[ i ].y, 4, i ); // leg
				tankaddMissle( 18, Player[ i ].x, Player[ i ].y, 7, i ); // nuggets
				tankaddMissle( 19, Player[ i ].x, Player[ i ].y, 3, i ); // sparkler
				
				tankaddMissle( 14, Player[ i ].x, Player[ i ].y, 9, i ); // thigh
				tankaddMissle( 15, Player[ i ].x, Player[ i ].y, 8, i ); // leg
				tankaddMissle( 18, Player[ i ].x, Player[ i ].y, 3, i ); // nuggets
				tankaddMissle( 19, Player[ i ].x, Player[ i ].y, 1, i ); // sparkler
 
				tankaddMissle( 19, Player[ i ].x, Player[ i ].y, 4, i ); // sparkler
				tankaddMissle( 19, Player[ i ].x, Player[ i ].y, 9, i ); // sparkler
				tankaddMissle( 19, Player[ i ].x, Player[ i ].y, 7, i ); // sparkler
			}
				
			switch( Player[ i ].guy )  // change from what
			{
				case TURNTOAD: // about face
					tanksetPlayerSprite( (short)i, STOPTOAD );
					break;
				
				case FLIPTOAD: 
				case JUMPTOAD: // start walkin'
					if ( Player[ i ].speed )
						tanksetPlayerSprite( (short)i, WALKTOAD );
					else
						tanksetPlayerSprite( (short)i, STOPTOAD );
					break;
							 
				case SQUATOAD: // stand up stupid
					if ( Player[ i ].speed )
						tanksetPlayerSprite( (short)i, WALKTOAD );
					else
						tanksetPlayerSprite( (short)i, STOPTOAD );
					break;
					
				case SPLATOAD: // been slimed - return to normal
					if ( Player[ i ].speed )
						tanksetPlayerSprite( (short)i, WALKTOAD );
					else
						tanksetPlayerSprite( (short)i, STOPTOAD );
					tankaddMissle( 10, Player[ i ].x, Player[ i ].y, 5, i ); // puddle 
					break;
					
				case REDTOAD1: 
					if ( Player[ i ].shield ) // shield guage says you live
							tanksetPlayerSprite( (short)i, STOPTOAD );
					else			// goodnight gracie
						tanksetPlayerSprite( (short)i, DUSTTOAD );
					break;
					
				case ZAPZTOAD: // blue shock over now
					if ( Player[ i ].speed )
						tanksetPlayerSprite( (short)i, WALKTOAD );
					else
						tanksetPlayerSprite( (short)i, STOPTOAD );
					break;
					
				default:
					break;
			}
			// now straighten him out
			if ( Player[ i ].rection == 3 || Player[ i ].rection == 9 ) // right
				Player[ i ].rection = 6;
			if ( Player[ i ].rection == 1 || Player[ i ].rection == 7 ) // left
				Player[ i ].rection = 4;
		}
	}				
}

//-----------------------------------------------------------------
// move - update positions
//-----------------------------------------------------------------
void tankmove( void )
{
	tankadjustPlayers();
	if ( Option.NoNet )
		tankadjustRobots();
	tankcollide();
	tankadjustMissles();
}
//-----------------------------------------------------------------
// changeTanks - takes tank name and adds prefixes for tank parts
// usage: changeTanks("toad");
//-----------------------------------------------------------------
void tankchangeTanks( char *newtilepath )
{
	sprintf( Tank.bmpname, PKGDATADIR "/bmp/%s.bmp", newtilepath );
	sprintf( Tank.spritename, PKGDATADIR "/bmp/%s.elf", newtilepath );
	Tank.change = 1;
}

//-----------------------------------------------------------------
// restoreSurfaces
//-----------------------------------------------------------------
void tankrestoreSurfaces( void )
{
	Tank.tanktile = SDL_LoadBMP( Tank.bmpname );
	SDL_SetSurfacePalette(Tank.tanktile, Screen->format->palette);
	SDL_SetColorKey(Tank.tanktile, SDL_TRUE, 185 );
	tankparseSpriteFile( Tank.spritename );
}

//-----------------------------------------------------------------
// int tanknextCommand( char *instr, char *outstr, char *place, int len )
//	  find and retrieve a command string from instr starting at place
//	  strip cr/lf/comments, add a terminating comma and a null
// input:
//	  char *instr - compound string full of comments
//	  char *outstr - null terminated command string a-z:#,#,#,\0
// returns:
//	  0 - success
//	  !0 - failure
//----------------------------------------------------------------
int tanknextCommand( char *place, char *outstr )
{
   char *x = place;
   char *y = outstr;
   int found = 0; // increment if command found
	
	while( *x++ && !found ) // find start of a command
	{
		if ( *x == ':' )
		{
			--x;
			if ( strchr( "eq#", (int)*x ) != NULL ) // got one
			{
				found++;
				while( *x && strchr( " \t\r\n\\", (int)*x ) == NULL )  // fill output
					*y++ = *x++;
				*y++ = ',';
				*y++ = '\0';
			}
			else
				while( *x != '\n' )  // skip it
					x++;
		 }
	}
			
	return found;
}
//eo nextCommand

//-----------------------------------------------------------------
// parseSpriteFile - opens, reads and builds a sprite list from a sprite description file
//input:
//	  sprite *Sprite- Sprite of sprite array
//	  char *fname - sprite config filename as defined in sprite.h
//returns:
//	  int 0 - success
//	  !0 - failure - bad value encountered etc.
//-----------------------------------------------------------------
int tankparseSpriteFile( char *elfname )
{
	int rfile;
	off_t sizeofile, actual;
	char comstr[256] = "";
	char *text = NULL;
	char *place;
	char *nextarg;
	int retval = 0, elfnum = 0, i, cmd, left, top, right, bottom;

	rfile = open( elfname,  O_RDONLY );
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

		if ( actual == sizeofile )
		{
			while( tanknextCommand( place, comstr ))
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
						Tank.spritecount = atoi( nextarg );
						if ( Tank.sprite != NULL )
							free( Tank.sprite );
						Tank.sprite = (SPRITE *)calloc( Tank.spritecount + 1, sizeof( SPRITE ));
						break;

					case 101: // e elf
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].x = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].y = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].z = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].opaque = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].type = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].framenum = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].rection = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].speed = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].dist = atoi( nextarg );
						Tank.sprite[ elfnum ].curframe = 0;						
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].ttl = atoi( nextarg );
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].noise = atoi( nextarg );
						// printf("Tank.sprite[ %d ].noise = %d\n", elfnum,Tank.sprite[ elfnum ].noise);
						nextarg = strtok( NULL, "," );
						Tank.sprite[ elfnum ].pal = atoi( nextarg );
						Tank.sprite[ elfnum ].frame = NULL;
						spritesizeFrames( &Tank.sprite[ elfnum ] );
						elfnum++;
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
						Tank.sprite[ elfnum - 1 ].wide = right - left;
						Tank.sprite[ elfnum - 1 ].high = bottom - top;
						spritesetFrame( &Tank.sprite[ elfnum - 1],  Tank.sprite[ elfnum-1 ].curframe++, left, top, right - left, bottom - top );
						break;
					default:
						break;
				}
			}
		}	 
		else
		{
			if ( ! retval )
				retval = 2;
		}
		if ( text != NULL ) 
			free( text );
		close( rfile );
		for ( i = 0; i < Tank.spritecount; i++ )
			Tank.sprite[ i ].curframe = 0;
			Tank.sprite[ i ].pindex = -1;
	}
	
	return retval;

}//parseSpriteFile
//-----------------------------------------------------------------
//  halfwide - return half of Me's spritewidth
//-----------------------------------------------------------------
int tankhalfwide( int pindex )
{
	return Tank.sprite[ Player[ pindex ].guy ].wide / 2 ;
}
//-----------------------------------------------------------------
//  howwide - return half of Me's spritewidth
//-----------------------------------------------------------------
int tankhowwide( int pindex )
{
	return Tank.sprite[ Player[ pindex ].guy ].wide;
}
//-----------------------------------------------------------------
// howhigh - return height of Me's current sprite
//-----------------------------------------------------------------
int tankhowhigh( int pindex )
{
	return Tank.sprite[ Player[ pindex ].guy ].high;
}
//-----------------------------------------------------------------
// halfhigh - return height of Me's current sprite
//-----------------------------------------------------------------
int tankhalfhigh( int pindex )
{
	return Tank.sprite[ Player[ pindex ].guy ].high / 2;
}
//-----------------------------------------------------------------
// change me sprite to sprite number X me bucko
//-----------------------------------------------------------------
void tanksetPlayerSprite( int who, int spritenum )
{
	Player[ who ].guy = spritenum;
	Player[ who ].odom = Tank.sprite[ spritenum ].odom;
	Player[ who ].dist = Tank.sprite[ spritenum ].dist;
	Player[ who ].curframe = 0;
	Player[ who ].inertia = Tank.sprite[ spritenum ].ttl; 
}
//-----------------------------------------------------------------
// collide - check and see if we've plowed into another player or
// a sprite, set guy to proper animation sequence and return zero
//-----------------------------------------------------------------
int tankcollide( void )
{
	RECT me, them;
	POINT hot;
	int i, j, m;
	
	for ( i = 0; i < PLAYERMAX; i++ )
	{
		if (( Player[ i ].id < -1 )||( Player[ i ].guy == DUSTTOAD ))
			continue;
		me.x = Player[ i ].x  - tankhalfwide( i );	   // kill zone
		me.y = Player[ i ].y  - tankhalfhigh( i );
		me.w = Player[ i ].x + tankhalfwide( i );
		me.h = Player[ i ].y + tankhalfhigh( i );
		
		for ( j = 0; j < PLAYERMAX; j++ ) // ramming toad
		{
			if (( Player[ j ].id < -1 )||( i == j )||( Player[ j ].guy == DUSTTOAD ))
				continue;
				
			them.x = Player[ j ].x; - tankhalfwide( j ); // kill zone adjustments here
			them.y = Player[ j ].y; - tankhalfhigh( j );
			them.w = Player[ j ].x + tankhalfwide( j );
			them.h = Player[ j ].y + tankhalfhigh( j );
 
			if ( intersectrect( &me, &them )) // when players collide
			{
				if ( Player[ j ].guy == WALKTOAD ||Player[ j ].guy == STOPTOAD )
				{
					if ( Player[ i ].rection == 6 || Player[ j ].rection == 5 )
						Player[ j ].rection = 7;
					if ( Player[ i ].rection == 4 )
						Player[ j ].rection = 9;
					Player[ j ].speed = LOGEAR;
					tanksetPlayerSprite( j, FLIPTOAD ); // switch to guy leaping 
				}
				if ( Player[ i ].guy == WALKTOAD ||Player[ i ].guy == STOPTOAD )
				{
					if ( Player[ j ].rection == 6 || Player[ i ].rection == 5 )
						Player[ i ].rection = 7;
					if ( Player[ j ].rection == 4 )
						Player[ i ].rection = 9;
					Player[ i ].speed = HIGEAR;
					tanksetPlayerSprite( i, FLIPTOAD ); // switch to guy leaping
				}
				if ( Player[ i ].rection == Player[ j ].rection )
				{
					if ( Player[ i ].rection == 9 )
						Player[ i ].rection = 7;
					else
						Player[ j ].rection = 9; 
				}
				tankaddMissle( 26, Player[ i ].x, Player[ i ].y, 5, i ); // bump marks
				racketplay( 16, 0 ); // hit
				// Player[ i ].shield -= SLIMEHIT;
			}
		}
				
		for ( j = 0; j < MISSLECOUNT; j++ ) // missle hit
		{
			if ( Tank.missle[ j ].type == 0 )// || Tank.missle[ j ].pindex == Me  )
				continue;
			hot.x = Tank.missle[ j ].x;	 
			hot.y = Tank.missle[ j ].y;

			for ( m = 0; m < MISSLECOUNT; m++ )
			{
				if ( m == j || Tank.missle[ m ].type != 23 ) 
					continue; 			// red type 23 cancels blue green

				them.x = Tank.missle[ m ].x  - ( Tank.sprite[ Tank.missle[ j ].type ].wide / 2 );
				them.y = Tank.missle[ m ].y - ( Tank.sprite[ Tank.missle[ j ].type ].high / 2 );
				them.w = them.x + ( Tank.sprite[ Tank.missle[ i ].type ].wide / 2 );
				them.h = them.y + ( Tank.sprite[ Tank.missle[ i ].type ].high / 2 );

				if ( pointinrect( &hot, &them ))
					Tank.missle[ j ].type = 0;   
			}

			if ( pointinrect( &hot, &me ))   
			{							   // guy changes 
				switch( Tank.missle[ j ].type ) // the effects of being hit
				{
					// case 3:					 // green slime 2 sizes
					case 4:
						Tank.missle[ j ].type = 0;
						Player[ Tank.missle[ j ].pindex ].bc += 1;
						tanksetPlayerSprite( (short)i, SPLATOAD ); // splat
						Player[ i ].inertia = Tank.sprite[ 9 ].ttl;
						Player[ i ].rection = Tank.missle[ j ].rection == 6 ? 6 : 4;
						Player[ i ].speed = LOGEAR;
						if ( Player[ i ].shield > 9 && Player[ i ].shield ) // shield guage
							Player[ i ].shield -= SLIMEHIT;
						break;

					// case 27:
					case 8:					 // blue meanie
						Tank.missle[ j ].type = 0;
						Player[ Tank.missle[ j ].pindex ].bc += 2;
						Player[ i ].speed = ALLSTOP; // stunned
						tanksetPlayerSprite( (short)i, ZAPZTOAD ); // zap - turn blue
						// your quickening
						tankaddMissle( 12, Player[ i ].x, Player[ i ].y, Player[ i ].rection, i ); 
						if ( Player[ i ].shield > 9 && Player[ i ].shield ) // shield guage
							Player[ i ].shield -= BLUEZHIT;
						break;
						
					case 23:			// red lava spit
						Tank.missle[ j ].type = 0;
						Player[ Tank.missle[ j ].pindex ].bc += 3;
						Player[ i ].speed = ALLSTOP; // toast
						tanksetPlayerSprite( (short)i, REDTOAD1 ); // red white boy
						if ( Player[ i ].shield > 9 && Player[ i ].shield ) // shield guage
							Player[ i ].shield -= REDLVHIT;
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
// adjustRobots - provide semi-intelligent moves for robot players
//-----------------------------------------------------------------
void tankadjustRobots( void )
{   
	short i, j;
	static int lastfired;
	static short boom = 0; // crude timer
	
	if ( ++boom > 18 ) // about a sec
		boom = 0;	

	for ( i = 1; i < PLAYERMAX; i++ )
	{
		if ( Player[ i ].guy == FLIPTOAD ) 
			continue;

		if ( Player[ i ].speed < HIGEAR ) //|| ( Player[ i ].x - Player[ 0 ].x > 600 )||( Player[ 0 ].x - Player[ i ].x > 600 ))
		{
			Player[ i ].power = 500; // otherwise they slow down and stop firing
			if ( Player[ i ].x < Player[ 0 ].x )
			{
				playerrightClickRection( i );
				continue;
			}
			else if ( Player[ i ].x > Player[ 0 ].x )
			{
				playerleftClickRection( i );
				continue;
			}
		}

		if ( Player[ i ].y < Player[ 0 ].y )
		{
			playerrightClickJump( i );
			continue;
		}
		if ( Player[ i ].y > Player[ 0 ].y )
		{
			playerleftClickJump( i );
			continue;
		}
		if ( Player[ i ].y == Player[ 0 ].y )
		{
			for ( j = 1; j < PLAYERMAX; j++ )
			{
				if ( Player[ j ].rection ==  RIGHT )
				{
					if ( j == i || ( Player[ j ].x > Player[ i ].x && Player[ j ].x < Player[ 0 ].x ))
						continue; // theres another robot in front of me
				}
				if ( Player[ j ].rection ==  LEFT )
				{
					if ( j == i || ( Player[ j ].x < Player[ i ].x && Player[ j ].x > Player[ 0 ].x ))
						continue; // theres another robot in front of me
				}
				if ( boom == 5 || boom == 11 || boom == 17 ) // 3 * a sec
				{
					lastfired = boom;
					playerleftClickFire( i, i % 3 );
				}
				else
					playerrightClickFire( i );
			}
		}
	} // eofor
} // eo adjust robots	

// eof
