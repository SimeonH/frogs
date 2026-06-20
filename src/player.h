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
//-------------- actions as in what kind of guy am i ...correspond to tank.elf sprite number
#define WALKTOAD  0	 // normal walking
#define TURNTOAD  1	 // face front used in changing directions
#define JUMPTOAD  2	 // leaping
#define STOPTOAD  5	 // standing still 
#define BACKTOAD  6	 // buffalo stance 
#define SPLATOAD  9	// been slimed
#define ZAPZTOAD  11   // feelin blue
#define REDTOAD1  20   // ashes to ashes
#define REDTOAD2  21   // a to a phase two
#define DUSTTOAD  22   // a to dust phase three
#define SQUATOAD  24   // gotta squat
#define FLIPTOAD  25   // acrobatics
//--------------- penalties/bonuses
#define MOVELOSS  4	// power decremented by moving
#define SLIMEHIT  200	// shield power decremented by hit of slime
#define BLUEZHIT  350	 // shield power decremented by hit of lava
#define REDLVHIT  400	// shield power decremented by hit of lava
#define POWRGAIN  2	 // amount recharged by resting
#define POWRLOSS  16	 // amount lost by hocking lava loogie
#define SHIELDMAX 5998
#define POWERMAX  5998
#define POWERMIN  9
//--------------- how fast was i goin' officer
#define ALLSTOP   0
#define LOGEAR	9
#define HIGEAR	17
#define LEFT	  4
#define RIGHT	 6
//--------------- buffer space to avoid premature collisions with yourself
#define HEADSTART 8 // how many pixels to add between you and your missle fired

// matching an array of these make up the active game data to be distributed
typedef struct
{
	char name[ 30 ];
	char ip[ 20 ];
	char rm[ 20 ];
} ROSTER;

typedef struct 
{
	char id;		// -1 = robot
	short bc;		 // body count
	short x;		  // location in room
	short y;
	short power;	 // power available
	short shield;	 // craft integrity
	short inertia;	// how long
	short rection;  // value according to keypad compass 4 left 6 right 5 buffalo stance
	short guy;	  // index into sprite array to use - whatami doing now
	// --------------- guy's current sprite specific vars so we don't dance like the rockettes
	char speed;	  // number of pixels to jump
	char odom;	   // no. of times to show before incrementing curframe
	char dist;	   // counts til = odom and then curframe++
	char curframe;   // current frame -what is this sprite doing now
} PLAYER;

// prototypes
void playeradjust( int ); // move etc.
void playerleftClickFire( int, int );
void playerrightClickFire( int );
void playerleftClickRection( int );
void playerrightClickRection( int );
void playerrightClickJump( int );
void playerleftClickJump( int );
void playerdoubleClickFlip( int );
void playerrefresh( int );
void playerresetAll( void );
void playerclickcenterstop( int );
void playerclickupleft( int );
void playerclickupright( int );
