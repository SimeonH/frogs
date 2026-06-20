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
// your basic frog pilot
//--------------------------------------------------------------------------------
#include "compat.h"
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
/*
#include "helm.h"
*/

extern PLAYER Player[];
extern ROSTER Roster[];
extern ROOM Room;
extern TANKS Tank;
extern OPTIONS Option;

/*
unsigned short far Draghand[ 32 ] = // mouse cursor hot spot 0 0
{
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xf000, 0x2e00, 0x1e80, 0x0de0, 0x03d0, 0x01f8, 0x187c, 0x7376,
		0x779f, 0x075f, 0xf6df, 0xf69e, 0xd87d, 0x7ffa, 0x3bb5, 0x1bea
};
*/

//-----------------------------------------------------------------
// void playerreset( int numb )
// 		reset a players variables to just joined state
//-----------------------------------------------------------------
void playerreset( int numb )
{
	Player[ numb ].id = -2; // -1 robot -2 invisible else index in player array
	Player[ numb ].bc = 0;
	playerrefresh( numb );
}

//-----------------------------------------------------------------
// void playerreset( int numb )
// 		reset a players variables to just joined state
//-----------------------------------------------------------------
void playerresetAll( void )
{
	int i;

	for ( i = 0; i < PLAYERMAX; i++ )
	{
		playerreset( i );
	}
	for ( i = 0; i < PLAYERMAX; i++ )
	{
		strcpy( Roster[i].name, "" );
		strcpy( Roster[i].ip, "" );
		strcpy( Roster[ i ].rm, "mine" ); // default room
	}
	if ( Option.NoNet )
	{
		Player[ 0 ].id = 1;
		strcpy( Roster[ 0 ].name, Option.FrogName );
	}
}
//-----------------------------------------------------------------
// refresh - reset values for recycled players
//-----------------------------------------------------------------
void playerrefresh( int numb )
{
	Player[ numb ].x = 0;
	Player[ numb ].y = 0;
	Player[ numb ].rection = RIGHT;
	Player[ numb ].inertia = 0;
	Player[ numb ].power = 5999;
	Player[ numb ].shield = 5999;
	Player[ numb ].speed = 0;
	Player[ numb ].guy = BACKTOAD;
	Player[ numb ].curframe = 0;
	Player[ numb ].dist = 1;
	Player[ numb ].odom = 1;
}

//-----------------------------------------------------------------
// clickcenterstop - mouse signature move pass pindex into player array
//-----------------------------------------------------------------
void playerclickcenterstop( int pindex )
{
	if ( Player[ pindex ].guy < SPLATOAD || 
			Player[ pindex ].guy == FLIPTOAD || Player[ pindex ].guy != DUSTTOAD )
		tanksetPlayerSprite( pindex, 5 );
	if ( Player[ pindex ].guy < SPLATOAD || Player[ pindex ].guy == FLIPTOAD )
	{
		Player[ pindex ].speed = ALLSTOP;
		Player[ pindex ].dist = 0;
	}
}

//-----------------------------------------------------------------
// clickupleft - mouse signature move pass pindex into player array
//-----------------------------------------------------------------
void playerclickupleft( int pindex )
{
	if ( Player[ pindex ].rection == 9 )
		Player[ pindex ].rection = 7;
}

//-----------------------------------------------------------------
// clickupright - mouse signature move pass pindex into player array
//-----------------------------------------------------------------
void playerclickupright( int pindex )
{
	if ( Player[ pindex ].rection == 7 )
		Player[ pindex ].rection = 9;
}

//-----------------------------------------------------------------
// leftclickrection - pindex index into player array
//-----------------------------------------------------------------
void playerleftClickRection( int pindex )
{
	if ( Player[ pindex ].guy == DUSTTOAD )
	{
		if ( Player[ pindex ].rection == 6 )
			Player[ pindex ].speed = ALLSTOP;
		if ( Player[ pindex ].rection  == 4 )
			Player[ pindex ].speed = HIGEAR;
		Player[ pindex ].rection = 4;
	}
	else if ( Player[ pindex ].guy < SPLATOAD || Player[ pindex ].guy == FLIPTOAD )
	{
		if ( Player[ pindex ].guy == JUMPTOAD ) // stop climbing the walls
			Player[ pindex ].guy = WALKTOAD;
		if ( Player[ pindex ].rection != 4 && Player[ pindex ].rection != 6 )
			Player[ pindex ].rection = 4;

		if ( Player[ pindex ].rection == 4 && Player[ pindex ].speed == LOGEAR && Player[ pindex ].power > POWERMIN ) // haul ass
		{
			tanksetPlayerSprite( pindex, 0 ); //truckin set guy to 0
			Player[ pindex ].speed = HIGEAR;
			Player[ pindex ].dist = 1;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke
		}
		if ( Player[ pindex ].rection == 4 && Player[ pindex ].speed == ALLSTOP ) // put it in 1st  
		{
			tanksetPlayerSprite( pindex, 0 ); //truckin set guy to 0
			Player[ pindex ].speed = LOGEAR;
			Player[ pindex ].dist = 2;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke

		}
		if ( Player[ pindex ].rection == 6 && Player[ pindex ].speed == ALLSTOP ) // twirl
		{
			tanksetPlayerSprite( pindex, 1 );
			Player[ pindex ].guy = TURNTOAD;
			Player[ pindex ].rection = 4;
			Player[ pindex ].inertia = 3;
			Player[ pindex ].dist = 0;
		}
		if ( Player[ pindex ].rection == 6 && Player[ pindex ].speed == LOGEAR ) // brakes
		{
			if ( Player[ pindex ].guy != DUSTTOAD )
				tanksetPlayerSprite( pindex, 5 );
			Player[ pindex ].speed = ALLSTOP;
			Player[ pindex ].dist = 0;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
		}
		if ( Player[ pindex ].rection == 6 && Player[ pindex ].speed == HIGEAR ) // downshift
		{
			Player[ pindex ].speed = LOGEAR;
			Player[ pindex ].dist = 2;
			Player[ pindex ].curframe = 0;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
		}
	}
}
//-----------------------------------------------------------------
// rightclickrection - pindex index into player array
//-----------------------------------------------------------------
void playerrightClickRection( int pindex )
{
	if ( Player[ pindex ].guy == DUSTTOAD )
	{
		if ( Player[ pindex ].rection == 4 )
			Player[ pindex ].speed = ALLSTOP;
		if ( Player[ pindex ].rection  == 6 )
			Player[ pindex ].speed = HIGEAR;
		Player[ pindex ].rection = 6;
	}
	else if ( Player[ pindex ].guy < SPLATOAD )
	{
		if ( Player[ pindex ].guy == JUMPTOAD ) // stop climbing the walls
			Player[ pindex ].guy = WALKTOAD;
		if ( Player[ pindex ].rection != 6 && Player[ pindex ].rection != 4 )
			Player[ pindex ].rection = 6;
		if ( Player[ pindex ].rection == 6 && Player[ pindex ].speed == LOGEAR && Player[ pindex ].power > POWERMIN ) // haul ass
		{
			tanksetPlayerSprite( pindex, WALKTOAD ); // truckin sprite
			Player[ pindex ].speed = HIGEAR;
			Player[ pindex ].dist = 1;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
		}   
		if ( Player[ pindex ].rection == 6 && Player[ pindex ].speed == ALLSTOP ) // downshift
		{
			tanksetPlayerSprite( pindex, WALKTOAD );
			Player[ pindex ].speed = LOGEAR;
			Player[ pindex ].dist = 2;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
		
		}
		if ( Player[ pindex ].rection == 4  && Player[ pindex ].speed == ALLSTOP ) // twirl
		{
			Player[ pindex ].rection = 6;
			tanksetPlayerSprite( pindex, TURNTOAD );
			Player[ pindex ].dist = 0;
			Player[ pindex ].inertia = 3;
		}
		if ( Player[ pindex ].rection == 4  && Player[ pindex ].speed == LOGEAR ) // standing still
		{
			tanksetPlayerSprite( pindex, STOPTOAD ); 
			Player[ pindex ].speed = ALLSTOP;
			Player[ pindex ].dist = 0;	  
		}
		if ( Player[ pindex ].rection == 4  && Player[ pindex ].speed == HIGEAR ) // downshift
		{
			tanksetPlayerSprite( pindex, WALKTOAD );
			Player[ pindex ].speed = LOGEAR;
			Player[ pindex ].dist = 2;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke
		}
	}
}
	
//-----------------------------------------------------------------
// rightclickjump - pindex index into player array - going down please
//-----------------------------------------------------------------
void playerrightClickJump( int pindex )
{
	int xx, yy;
	
	if ( Player[ pindex ].x > Room.maxx )
		xx = Player[ pindex ].x - Room.maxx;
	else
		xx = Player[ pindex ].x;
	yy = Player[ pindex ].y + tankhalfhigh( pindex );
	// printf("g:%d x:%d y:%d s:%d\n", Player[ pindex ].guy, xx, yy, roomsolidGround( xx, yy ));
	if ( Player[ pindex ].guy == DUSTTOAD )
	{
		if ( Player[ pindex ].rection == 4 || Player[ pindex ].rection == 7 )
			Player[ pindex ].rection = 1;
		else if ( Player[ pindex ].rection == 6 || Player[ pindex ].rection == 9 )
			Player[ pindex ].rection = 3;
		else if ( Player[ pindex ].rection == 8 )
			Player[ pindex ].rection = 2;
	}
	else if ( Player[ pindex ].guy < SPLATOAD && Player[ pindex ].guy != JUMPTOAD && roomsolidGround( xx, yy ))
	{
		if ( Player[ pindex ].rection == 6 )
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
		else
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke
		Player[ pindex ].y++;
		tanksetPlayerSprite( pindex, SQUATOAD );
	}
}

//-----------------------------------------------------------------
// leftclickjump - pindex index into player array
//-----------------------------------------------------------------
void playerleftClickJump( int pindex )
{
	int xx, yy;

	if ( Player[ pindex ].x > Room.maxx )
		xx = Player[ pindex ].x - Room.maxx;
	else
		xx = Player[ pindex ].x;
	yy = Player[ pindex ].y + tankhalfhigh( pindex );
	// printf("g:%d x:%d y:%d s:%d\n", Player[ pindex ].guy, xx, yy, roomsolidGround( xx, yy ));
	if ( Player[ pindex ].guy == DUSTTOAD )
	{
		if ( Player[ pindex ].rection == 4 || Player[ pindex ].rection == 1 )
			Player[ pindex ].rection = 7;
		else if ( Player[ pindex ].rection == 6 || Player[ pindex ].rection == 3 )
			Player[ pindex ].rection = 9;
		else if ( Player[ pindex ].rection == 2 )
			Player[ pindex ].rection = 8;
	}
	else if ( Player[ pindex ].guy < SPLATOAD && Player[ pindex ].guy != JUMPTOAD && roomsolidGround( xx, yy ))
	{
		if ( Player[ pindex ].rection == 6 )
		{
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
			Player[ pindex ].rection = 9;
		}
		else if ( Player[ pindex ].rection == 4 )
		{
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke
			Player[ pindex ].rection = 7;
		}
		tanksetPlayerSprite( pindex, JUMPTOAD ); // switch to guy leaping
//		inertia = 24; // how high set by jump sprite ttl in tank.txt
	}
}
//-----------------------------------------------------------------
// leftClickFire - number of weapon fired
//-----------------------------------------------------------------
void playerleftClickFire( int pindex, int projectile )
{
	int drection = 0, xx; // HEADSTART defined in player.h keeps player from pissing on his shoes
	
	if ( Player[ pindex ].guy != DUSTTOAD && Player[ pindex ].guy < SPLATOAD )
	{
		if ( Player[ pindex ].rection == 4 || Player[ pindex ].rection == 7 || Player[ pindex ].rection == 1 ) // 4 7 1 = LEFT
		{
			drection = 4;
			switch( projectile )
			{
				case 0: // blue
					xx = Player[ pindex ].x - tankhalfwide( pindex ) - HEADSTART;
					tankaddMissle( 27, xx, Player[ pindex ].y, drection, pindex );
					break;
				case 1: //green
					xx = Player[ pindex ].x - tankhalfwide( pindex ) - HEADSTART;
					tankaddMissle( 3, xx, Player[ pindex ].y, drection, pindex );
					break;
				case 2: // red 
					xx = Player[ pindex ].x - tankhowwide( pindex ) - HEADSTART;
					tankaddMissle( 23, xx, Player[ pindex ].y, drection, pindex );
					Player[ pindex ].power -= POWRLOSS;
					break;
				default: // dildo
					break;
			}	   
		}
		else if ( Player[ pindex ].rection == 6 || Player[ pindex ].rection == 9 || Player[ pindex ].rection == 3 ) // 3 6 9 = RIGHT
		{
			drection = 6;
			switch( projectile )
			{
				case 0: // blue
					xx = HEADSTART + Player[ pindex ].x + tankhalfwide( pindex );
					tankaddMissle( 27, xx, Player[ pindex ].y, drection, pindex );
					break;
				case 1: //green
					xx = HEADSTART + Player[ pindex ].x + tankhalfwide( pindex );
					tankaddMissle( 3, xx, Player[ pindex ].y, drection, pindex );
					break;
				case 2: // red 
					xx = HEADSTART + Player[ pindex ].x + tankhowwide( pindex );
					tankaddMissle( 23, xx, Player[ pindex ].y, drection, pindex );
					Player[ pindex ].power -= POWRLOSS;
					break;
				default: // dildo
					break;
			}	   
		}

	}
}
//-----------------------------------------------------------------
// rightClickFire - pindex index into player array
//-----------------------------------------------------------------
void playerrightClickFire( int pindex )
{
	pindex = pindex; // shaddup
}
//-----------------------------------------------------------------
// leftclickjump - pindex index into player array
//-----------------------------------------------------------------
void playerdoubleClickFlip( int pindex )
{
	if ( Player[ pindex ].guy != DUSTTOAD && Player[ pindex ].guy < SPLATOAD )
	{
		if ( Player[ pindex ].rection == 6 )
		{
			Player[ pindex ].rection = 9;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 4, pindex ); // smoke
			tanksetPlayerSprite( pindex, FLIPTOAD ); // switch to guy leaping
		} 
		else if ( Player[ pindex ].rection == 4 )
		{
			Player[ pindex ].rection = 7;
			tankaddMissle( 7, Player[ pindex ].x, Player[ pindex ].y, 6, pindex ); // smoke
			tanksetPlayerSprite( pindex, FLIPTOAD ); 
		}
	}
}
//-----------------------------------------------------------------

