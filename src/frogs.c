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
#include "compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "frogs.h"
#include "racket.h"
#include "util.h"
#include "sprite.h"
#include "player.h"
#include "tank.h"
#include "room.h"
#include "helm.h"
#include "chardisp.h"
#include "magi.h"
#include "ceo.h"
#include "pond.h"

SDL_Surface *Screen;
SDL_Window *Window;
SDL_Renderer *Renderer;
SDL_Texture *ScreenTex;
SDL_Surface *Screen32;
RECT rect = {0,0,SCREENWIDTH,SCREENHEIGHT};
PLAYER Player[ PLAYERMAX ];
ROSTER Roster[ PLAYERMAX ];
TANKS Tank;
ROOM Room;
ROOM Room;
ONSCREENDISPLAY Chardisp;
HELM Helm;
OPTIONS Option;
MAGI Magi;
CEO Ceo;
Mix_Chunk **Noise; // global array of sounds
short CurrentPalette = 0; // palette effect
short Me = 0;
short Yodaddy; // index of current server in player array
short Winner = -1;
short Helmon = 1;
char Bots[80] = PKGDATADIR "/doc/robots1.txt";
// -- pond 
FROGCOMMAND Cmd; 
int Nrate = 125; // default network time cycle 
SDL_Thread *pondd;
SDL_Thread *pondc;

/* unsigned short far Draghand[ 32 ] = // mouse cursor hot spot 0 0
{
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xf000, 0x2e00, 0x1e80, 0x0de0, 0x03d0, 0x01f8, 0x187c, 0x7376,
	0x779f, 0x075f, 0xf6df, 0xf69e, 0xd87d, 0x7ffa, 0x3bb5, 0x1bea
}; */

//-----------------------------------------------
// void flipscreen(void) - present Screen to the window
// ----------------------------------------------
void flipscreen()
{
	SDL_BlitSurface(Screen, NULL, Screen32, NULL);
	SDL_UpdateTexture(ScreenTex, NULL, Screen32->pixels, Screen32->pitch);
	SDL_RenderClear(Renderer);
	SDL_RenderCopy(Renderer, ScreenTex, NULL, NULL);
	SDL_RenderPresent(Renderer);
}

//-----------------------------------------------
// void hitanykey(void) - wait for a key press
// ----------------------------------------------
void hitanykey()
{
	int anykey = 0;
	SDL_Event event;

	while(!anykey)
	{
		while( SDL_PollEvent( &event ))
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
				case SDL_MOUSEBUTTONDOWN: // click
					switch( event.key.keysym.sym )
					{
						case SDLK_q:
						case SDLK_ESCAPE:
							exit( 0 );
						case SDLK_f:
							SDL_SetWindowFullscreen(Window,
								(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
								? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
							continue;
						default :
							anykey++;
							break;
					}
					break;
				case SDL_QUIT: /* SDL_QUIT event (window close) */
					exit( 0 );
				default :
					break;
			}
		}
		flipscreen();
		SDL_Delay(16);
	}
}

//------------------------------------------------------------------
// short doevents(void) - process events and return 1 on quit keys 
//------------------------------------------------------------------
short doevents()
{
	static short oneshot=0;
	int mx, my, i; // mouse xy
	SDL_Event event;
	char outstr[256] = "";

	while( SDL_PollEvent( &event ))
	{
		switch( event.type )
		{
			case SDL_QUIT: /* SDL_QUIT event (window close) */
				exit( 0 );
			case SDL_MOUSEBUTTONDOWN: // click
				switch( event.button.button )
				{
					case 1:  // LEFTBUTTONCLICK
						Helm.curlever = 2;
						if( ! oneshot )
						{
							oneshot++;
							if ( Option.NoNet )
								playerleftClickFire( Me, Helm.curtool );
							else
							{
								Cmd.id = Me;
								Cmd.type =  MSGFIRE;
								Cmd.bload = Helm.curtool;
							}
						}
						break;
					case 3:  // RIGHTBUTTONCLICK
						Helm.curlever = 3;
						if ( ! oneshot )
						{
							oneshot++;
							Helm.curtool--;
						}
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch( event.button.button ) // released
				{
					case 1 :  // LEFTBUTTON
					case 3 :  // RIGHTBUTTON
						//printf("buttonup %d\n", event.button.button);
						oneshot = 0;
					default:
						break;
				}
				break;
			case SDL_MOUSEMOTION: // the mouse, she has moved
				/* printf("Mouse moved by %d,%d to (%d,%d)\n", 
					event.motion.xrel, event.motion.yrel,
					event.motion.x, event.motion.y); */
				my = event.motion.y;
				mx = event.motion.x;
			 	if ( my < TOPTHIRD ) // jump
				{
					Helm.curlever = 1;
					if ( mx < LEFTTHIRD ) // top left
					{
						if ( Option.NoNet )
						{
							playerclickupleft( Me );
							playerleftClickJump( Me );
						}
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 7;
						}
					}
					else if( mx < RIGHTTHIRD ) // top middle
					{
						if ( Option.NoNet )
							playerdoubleClickFlip( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 8;
						}
					}
					else if ( mx > RIGHTTHIRD ) // top right
					{
						if ( Option.NoNet )
						{
							playerclickupright( Me );
							playerleftClickJump( Me );
						}
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 9;
						}
					}
				}
				else if ( my < BOTTOMTHIRD ) // center row
				{
					Helm.curlever = 1;
					if ( mx < LEFTTHIRD ) // left
					{
						if ( Option.NoNet )
							playerleftClickRection( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 4;
						}
					}
					else if( mx < RIGHTTHIRD ) // middle
					{ 
						if ( Option.NoNet )
							playerclickcenterstop( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 5;
						}
					}
					else if ( mx > RIGHTTHIRD ) // right
					{
						if ( Option.NoNet )
							playerrightClickRection( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 6;
						}
					}
				}
				else if ( my > BOTTOMTHIRD ) // bottom row
				{
					Helm.curlever = 0;
					if ( mx < LEFTTHIRD ) //  left
					{
						if ( Option.NoNet )
							playerrightClickJump( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 1;
						}
					}
					else if( mx < RIGHTTHIRD ) // middle
					{
						if ( Option.NoNet )
							playerrightClickJump( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 2;
						}
					}
					else if ( mx > RIGHTTHIRD ) // right
					{
						if ( Option.NoNet )
							playerrightClickJump( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 3;
						}
					}
				}
				break;
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
					case SDLK_q: 
                        return( 1 ); // bail out of main loop
					case SDLK_f:
						SDL_SetWindowFullscreen(Window,
							(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
							? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
						continue;
					case SDLK_h:
						Helmon = !Helmon;
						continue;
					case SDLK_LSHIFT:
						Helm.curlever = 1; // switch hands
						Helm.curtool--; // change tool
						break;
					case SDLK_LCTRL:  
						Helm.curlever = 1; // fuego
						if ( Option.NoNet )
							playerleftClickFire( Me, Helm.curtool );
						else
						{
							Cmd.id = Me;
							Cmd.type =  MSGFIRE;
							Cmd.bload = Helm.curtool;
						}
						break;
					case SDLK_UP:
						Helm.curlever = 2;
						if ( Option.NoNet )
							playerleftClickJump( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							if ( Player[Me].rection == 4 || Player[Me].rection == 1 )
								Cmd.bload = 7;
							else if ( Player[Me].rection == 6 || Player[Me].rection == 3 )
								Cmd.bload = 9;
							else if ( Player[Me].rection == 5 || Player[Me].rection == 2 )
								Cmd.bload = 8;
							else
								Cmd.bload = Player[Me].rection; // keep going
						}
						break;
					case SDLK_DOWN:
						Helm.curlever = 2;
						if ( Option.NoNet )
							playerrightClickJump( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							if ( Player[Me].rection == 4 || Player[Me].rection == 7 )
								Cmd.bload = 1;
							else if ( Player[Me].rection == 6 || Player[Me].rection == 9 )
								Cmd.bload = 3;
							else if ( Player[Me].rection == 5 || Player[Me].rection == 8 )
								Cmd.bload = 2;
							else
								Cmd.bload = Player[Me].rection; // keep going
						}
						break;
					case SDLK_LEFT:
						Helm.curlever = 3;
						if ( Option.NoNet )
							playerleftClickRection( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 4;
						}
						break;
					case SDLK_RIGHT:
						Helm.curlever = 3;
						if ( Option.NoNet )
							playerrightClickRection( Me );
						else
						{
							Cmd.id = Me;
							Cmd.type = MSGMOVE;
							Cmd.bload = 6;
						}
						break;
						
					case SDLK_F1:
						chardispclear();
						chardispaddaline( "left shift to change current tool" );
						chardispaddaline( "left control to use the current tool" );
						chardispaddaline( "up to jump" );
						chardispaddaline( "down to drop down" );
						chardispaddaline( "left turns left speeds up or slows down" );
						chardispaddaline( "right turns right speeds up or slows down" );
						chardispaddaline( "f9 for frog roster/score" );
						if ( Winner || Option.NoNet )
						{
							chardispaddaline( "f8 reset game" );
						}
						chardispaddaline( "f7 volume plus" );
						chardispaddaline( "f6 volume minus" );
						chardispaddaline( "f5 background music" );
						chardispaddaline( "f4 sound on off" );
						chardispaddaline( "f3 a frog story" );
						chardispaddaline( "f2 more hints" );
						chardispaddaline( "center mouse to stop  left right walk  up down jump" );
						chardispaddaline( "mouse clicking controls your tool" );
					  	chardispaddaline( "esc or q to quit" );
						break;
					case SDLK_F2:
						chardispbull( PKGDATADIR "/doc/bull2.txt" );
						break;
					case SDLK_F3:
						chardispbull( PKGDATADIR "/doc/bull1.txt" );
						break;
					case SDLK_F4:
						chardispclear();
						if ( Option.Sound >= 0 ) // -1 no sound
						{
							if ( Option.Sound == 0 ) // 0 sound and sound off
								racketon();
							else
								racketoff();
							chardispaddaline( Option.Sound?"sound on":"sound off" );
						}
						else
							chardispaddaline( "aural interface offline" );
						break;
					case SDLK_F5:
						if ( Option.Sound == -1 ) // -1 no sound
							break;
						if ( Mix_Playing( 0 ))
							Mix_HaltChannel( 0 );
						else
							racketplay( 0, -1 );
						break;
					case SDLK_F6:
						racketvolume( -8 );
						break;
					case SDLK_F7:
						racketvolume( 8 );
						break;
					case SDLK_F8:
						if ( Option.NoNet ) // unleash the robeasts
						{
							if ( loadRobotsFile( Bots ))
								setRobotsXy();
						}
						else if ( Winner > -1 )
						{
							if ( Option.NoNet )
							{	
								for ( i = 0; i < PLAYERMAX; i++ )
								{
									playerrefresh( i );
								}
							}
							else
							{
								Cmd.id = Me;
								Cmd.type = MSGREST;
								Cmd.bload = 'r';
							}
						}
						break;

					case SDLK_F9:
						chardispclear();
						for ( i = PLAYERMAX - 1; i >= 0; --i ) 
						{
							if ( Player[ i ].id > -2 )
							{
								sprintf( outstr, "%d: %s score %d", i, Roster[ i ].name, Player[ i ].bc );
								chardispaddaline( outstr );
							}
						}
						if ( Option.NoNet )
						{
							sprintf( outstr, "missles %d x:%d y:%d", Tank.misslecount, 
											Player[Me].x, Player[Me].y );
							chardispaddaline( outstr );
						}
						break;
					default :
						break;
			}
		}

	}
    return( 0 );
}

//----------------------------------------
// void draw(void) - paint a screen load
//----------------------------------------
void draw()
{
	rect.x = 0;
	rect.w = SCREENWIDTH;
	rect.y = 0;
	if ( Me == Winner )
	{
		rect.h = SCREENHEIGHT;
		SDL_SetClipRect( Screen, &rect );
		ceodraw( Screen );
		chardispdraw( Screen );
	}
	else
	{
		rect.h = SCREENHEIGHT - CONTROLSHEIGHT + OVERLAP;
		SDL_SetClipRect( Screen, &rect );
		roomdraw( Screen );
		tankdraw( Screen );
		roomdrawtop( Screen );
		magidraw( Screen );
		chardispdraw( Screen );
		// clip for helm
		rect.x = 0;
		rect.w = SCREENWIDTH;
		rect.y = SCREENHEIGHT - CONTROLSHEIGHT;
		rect.h = CONTROLSHEIGHT;
		SDL_SetClipRect( Screen, &rect );
		if ( Helmon )
		{
			helmdraw( Screen );
		}
	}
}

//-----------------------------------------------------------
// void copyleft(void) - logo
//-----------------------------------------------------------
void copyleft(void)
{
	fputs("\nFrogs Of War II The BIG One\n", stderr );
	fputs("(c)Copyright Linus Sphinx 2005 All Rights Reserved\n", stderr );
	fputs("http://fullsack.com\n\n", stderr );
	fputs("Ported to SDL2 by Simeon Higgs (simeoncode@protonmail.com)\n\n", stderr);
}

//-----------------------------------------------------------
// void help(void) - command line help
//-----------------------------------------------------------
void help( void )
{
	fputs("args:\n", stderr );
	fputs("\t-f for fullscreen mode f toggles during game\n", stderr );
	fputs("\t-w for window mode f toggles during game\n", stderr );
	fputs("\t-q for quiet - no sound  default is loud as hell\n", stderr );
	fputs("\t-b:<path/to/bots/file>  default is doc/robots1.txt\n", stderr );
	fputs("\t\tno bots file scatters them evenly\n", stderr );

	fputs("\n\t\t\tFor UDP Network Game\n", stderr );

	fputs("\t-n:joe-jack UNIQUE handle to use as one word with hyphens for spaces\n", stderr );
	fputs("\t-a:<address> play with server at address, use 0.0.0.0 to be the server\n", stderr );
	fputs("\t-p:<port> port to use, all players must match  default is 26000\n", stderr );

	fputs( "\n\t\t\tTunage\n", stderr );
	fputs("\t-r:<4096> sound buffer size in bytes- default is 32768\n", stderr );
	fputs("\t-d turn off double buffering  default is on\n", stderr );
	fputs("\t-t:<X> where X is rate of draw cycle in milliseconds. - default 83\n", stderr );
	fputs("\t-x:<X> where X is rate of server polling in milliseconds. - default 125\n\n", stderr );
	fputs("examples:\n\n", stderr );
	fputs("\t$ ./frogs -w -q\n", stderr );
	fputs("\t$ ./frogs -f -n:hooty-mcboob -a:192.168.1.2 -p:8008\n", stderr );
	fputs("\t$ ./frogs -f -n:big-richard -a:0.0.0.0\n", stderr );
	fputs("\t$ ./frogs -f -r:8192\n\n", stderr );
}

//-----------------------------------------------------------
// void froghandler(int sig) - handle those annoying signals
//-----------------------------------------------------------
void froghandler(int sig)
{
	fprintf( stderr, "caught signal %d\n", sig );
	fputs("frog halt\n", stderr );
	exit( 0 );
}

//---------------
// main program
//---------------
int main( int argc, char **argv )
{
 	int i, winflag = SDL_WINDOW_FULLSCREEN_DESKTOP, rate = 83;
	int opts = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

	copyleft();

	Option.Sound = 1;
	Option.NoNet = 1;
	Option.Soundbufsz = 4096;
	Option.Host = 0;
	Option.Port = 26000;
	memset( Option.FrogName, 0, sizeof( Option.FrogName ));

	for ( i=1; i < argc; i++ )
	{
		switch( argv[ i ][ 1 ] )
		{
			case 'h':
			case '?':
				help();
				exit(0);
			case 'w':
				break;
			case 'f':
				winflag = SDL_WINDOW_FULLSCREEN_DESKTOP;
				break;
			case 'd':
				break;
			case 'q':
				opts ^= SDL_INIT_AUDIO;
				Option.Sound = -1;
				break;
			case 'r':
				Option.Soundbufsz = atoi( &argv[ i ][ 3 ] );
				break;
			case 't':
				rate = atoi( &argv[ i ][ 3 ] );
				break;
			case 'x':
				Nrate = atoi( &argv[ i ][ 3 ] );
				break;
			case 'n':
				strncpy( Option.FrogName, &argv[ i ][ 3 ], 15 );
				strlwr( Option.FrogName );
				break;
			case 'b':
				strncpy( Bots, &argv[ i ][ 3 ], 15 );
				break;
			case 'p':
				Option.Port = atoi( &argv[ i ][ 3 ] );
				// strcpy( Option.Server, "0.0.0.0" );
				break;
			case 'a':
				Option.NoNet = 0; // it's on now
				strncpy( Option.Server, &argv[ i ][ 3 ], 32 );
				break;
			default:
				fprintf( stderr, "\nUNKNOWN ARG: -%s\n\n", &argv[ i ][ 1 ] );
				help();
				exit(0);
				break;
		}
	}


	fprintf( stderr, "initializing ");
	if ( ! Option.FrogName[ 0 ] )
		strcpy( Option.FrogName, "turd-ferguson" );

	fprintf( stderr, "tea interval: %d msec.\n", rate);
	if ( SDL_Init( opts ) != 0 )
	{
		fprintf( stderr, "Initialization error: %s.\n", SDL_GetError());
		exit(1);
	}
	if ( opts & SDL_INIT_AUDIO )
	{
		fputs("forte\n", stderr);
		racketstart();
		atexit(racketstop);
	}
	else
		fputs("acappella\n", stderr);
	fprintf( stderr, "id set to %s\n", Option.FrogName );
	atexit(SDL_Quit);
	signal( SIGTERM, froghandler );
	signal( SIGINT, froghandler );
	Window = SDL_CreateWindow("Frogs Of War II The BIG One by Linus Sphinx",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREENWIDTH, SCREENHEIGHT, winflag);
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(Renderer, SCREENWIDTH, SCREENHEIGHT);
	Screen = SDL_CreateRGBSurface(0, SCREENWIDTH, SCREENHEIGHT, 8, 0,0,0,0);
	Screen32 = SDL_CreateRGBSurfaceWithFormat(0, SCREENWIDTH, SCREENHEIGHT, 32,
		SDL_PIXELFORMAT_ARGB8888);
	ScreenTex = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREENWIDTH, SCREENHEIGHT);
	if ( Screen != NULL && Window != NULL )
	{
		SDL_SetClipRect(Screen, &rect);
		SDL_ShowCursor( 1 );
		loadbmp( Screen, PKGDATADIR "/bmp/liz.bmp", 1 );
		flipscreen();
		racketplay( 15, 0 ); // opening blast always first song in racket.elf
		playerresetAll();
		chardispinit();
		tankinit();
		roominit();
		helminit();
		magiinit();
		ceoinit();
		if ( Option.NoNet && loadRobotsFile( Bots ))
			setRobotsXy();
		if ( ! Option.NoNet )
		{
			fprintf( stderr, "using port: %d\n", Option.Port );
			fprintf( stderr, "pond splash\n");
			pondd = SDL_CreateThread( pondsplash, "pondsplash", NULL );
			fputs("pond ripple\n", stderr );
			pondc = SDL_CreateThread( pondripple, "pondripple", Option.Server );
		}
		hitanykey();
		racketplay( 0, -1 ); // background music
		paintscreen( Screen, 0 );
		chardispaddaline("f1 help f4 sound f6 volume up f7 volume down f8 reset");
		while( 1 )
		{
			if ( doevents())
				break;
			whowon();
			draw();
			tankmove();
			roomcollide();
			SDL_Delay(timeleft(rate));
			flipscreen();
		}
		rect.x = 0;
		rect.y = 0;
		rect.w = SCREENWIDTH;
		rect.h = SCREENHEIGHT;
		SDL_SetClipRect(Screen, &rect);
		loadbmp( Screen, PKGDATADIR "/bmp/grimfrog.bmp", 0 );
		chardispdestroy();
		magidestroy();
		ceodestroy();
		if ( Option.NoNet ) // else it's done in ponddestroy
			tankdestroy();
		roomdestroy();
		hitanykey();
		SDL_DestroyTexture(ScreenTex);
		SDL_FreeSurface(Screen32);
		SDL_FreeSurface(Screen);
		SDL_DestroyRenderer(Renderer);
		SDL_DestroyWindow(Window);
	}
	fputs( "frog all stop\n", stderr );
	exit(0);
}

// eof
