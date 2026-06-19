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
// a network pond for frog spawning
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define sock_close(s) closesocket(s)
#else
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <fcntl.h>
#  define sock_close(s) close(s)
#endif
#include <SDL.h>
#include "frogs.h"
#include "util.h"
#include "player.h"
#include "sprite.h"
#include "tank.h"
#include "pond.h"

#define SHUTUP 1

extern int Nrate;
extern TANKS Tank;
extern OPTIONS Option;
extern PLAYER Player[];
extern ROSTER Roster[];
extern FROGCOMMAND Cmd;
extern short Me;
extern short Winner;
extern short Yodaddy;
struct hostent *CurrentHost;
SOCK Server;
SOCK Client;

//---------------------------------------------------------------------
// int pondserverinit( void )
// 			initialize a server socket and start listening for clients
// 		returns:
// 			>0 == really bad news
// --------------------------------------------------------------------
int pondserverinit( void )
{
	int ret = -1;
	
	Server.sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( Server.sock == -1 )
	{
		perror( "pondinitsocket" );
	}
	else 
	{
		// fcntl( Server.sock, F_SETFL, O_NONBLOCK );	
		Server.addr.sin_family = AF_INET;
		Server.addr.sin_port = htons( Option.Port );
		Server.addr.sin_addr.s_addr = htonl( INADDR_ANY ); // inet_addr( host );
		memset( &Server.addr.sin_zero, 0, sizeof( Server.addr.sin_zero ));

		ret = bind( Server.sock, (struct sockaddr*)&(Server.addr), sizeof( struct sockaddr_in ));
		if ( ret == -1 )
			perror( "pondinitbind" );
	}
	return( ret );
}

// --------------------------------------------------------------------
// void pondsplashupdate( void )
// 		server check for incoming messages and broadcast current state
// --------------------------------------------------------------------
int pondsplashupdate( void )
{
	int ret = 0, alen, flags = 0, i;
	struct sockaddr_in a; 
	char buf[ MAXLOAD + 1 ];
	MSGDTOAD dmsg; // outbound
	MSGCTOAD cmsg; 
	MSGRTOAD rmsg;
	short gamecast = 0; // missles and players
	short playcast = 0; // player roster
	short gamefull = 0; // tough titty kitty

	static unsigned long inseq = 0; // inbound data packet sequence number discard if less than
	static unsigned long outseq = 1; // outbound data packet sequence number discard if less than
	unsigned long in = 0; // sequence just in

	alen = sizeof( a );
	memset( &a.sin_zero, 0, sizeof( a.sin_zero ));
	ret = recvfrom( Server.sock, &buf, MAXLOAD, flags, (struct sockaddr*)&a, &alen );
	if ( ret > 8 )
	{
		switch( buf[ 1 ] )
		{				// type A control msgs	
			case MSGNAME: 
				fprintf( stderr, "%s signing in from %s:%d ", 
								&buf[ 8 ], inet_ntoa( a.sin_addr ), ntohs( a.sin_port ));
				ret = pondserversignupplayer( &buf[ 8 ], inet_ntoa( a.sin_addr ));
				if ( ret == PLAYERMAX )
				{
					fprintf( stderr, "%d rejected - game full\n", ret );
					gamefull++;
				}
				else
				{
					fprintf( stderr, "at #%d\n", ret );
					playcast++;
				}
				break;

			case MSGHAIL: // text messaging
#ifndef SHUTUP
				fprintf( stderr, "msgHAIL IN %s:%d\tid:%d\ttype:%c\tload:%s\n", 
						inet_ntoa( a.sin_addr ), ntohs( a.sin_port ), buf[0], buf[1], &buf[8]);
#endif
				break;
						// type B request msgs
			case MSGFIRE: 
#ifndef SHUTUP
				fprintf( stderr, "msgFIRE IN %s:%d\tid:%d\ttype:%c\tload:%d\n", 
					inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1], buf[8]);
#endif
				gamecast++;
				playerleftClickFire( buf[0], buf[8] );
				break;

			case MSGMOVE:
#ifndef SHUTUP
				fprintf( stderr, "msgMOVE IN %s:%d\tid:%d\ttype:%c\tload:%d\n", 
					inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1], buf[8]);
#endif
				gamecast++;
				switch( buf[ 8 ] )
				{
					case 1:
					case 3:
					case 2:
						playerrightClickJump( buf[0] );
						break;

					case 4:
						playerleftClickRection( buf[0] );
						break;

					case 5:
						playerclickcenterstop( buf[0] );
						break;

					case 6:
						playerrightClickRection( buf[0] );
						break;

					case 7:
						playerclickupleft( buf[0] );
						playerleftClickJump( buf[0] );
						break;

					case 8:
						playerdoubleClickFlip( buf[0] );
						break;

					case 9:
						playerclickupright( buf[0] );
						playerleftClickJump( buf[0] );
						break;

					default:
						break;
				}
				break;

			case MSGREST: 
#ifndef SHUTUP
				fprintf( stderr, "msgREST IN %s:%d\tid:%d\ttype:%c\tload:%c\n", 
						inet_ntoa( a.sin_addr ), ntohs( a.sin_port ), buf[0], buf[1], buf[8]);
#endif
				playcast++;
				gamecast++;
				if ( (short)buf[0] == Winner )
				{
					for ( i = 0; i < PLAYERMAX; i++ )
					{
						playerrefresh( i );
					}
				}
				break;
						// rest are game data

			case MSGC:
#ifndef SHUTUP
				fprintf( stderr, "msgC IN %s:%d\tid:%d\ttype:%c\n", 
								inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1]);
#endif
				if ( ret == sizeof( MSGCTOAD ))
				{	
					memcpy( &in, &buf[2], sizeof(unsigned long));
					if ( in > inseq )
					{
						for ( i = 0; i < MISSLECOUNT; i++) 
						{ 
							memcpy(&Tank.missle[i], &((MSGCTOAD*)&buf)->girls[ i ], sizeof ( SPRITEDATA )); 
						} 
						inseq = in;
					}
				}
				break;

			case MSGD:
#ifndef SHUTUP
				fprintf( stderr, "msgD IN %s:%d\tid:%d\ttype:%c\n", 
								inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1]);
#endif
				if ( ret == sizeof( MSGDTOAD ))
				{	
					memcpy( &in, &buf[2], sizeof(unsigned long));
					if ( in > inseq )
					{
						memcpy( Player, ((MSGDTOAD*)&buf)->boys, sizeof( PLAYER )* PLAYERMAX );
						inseq = in;
					}
				}
				break;

			case MSGR:  // always accept new roster in sequence or not
#ifndef SHUTUP
				fprintf( stderr, "msgR IN %s:%d\tid:%d\ttype:%c\n", 
								inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1]);
#endif
				if ( ret == sizeof( MSGRTOAD ))
				{	
					memcpy( Roster, ((MSGRTOAD*)&buf)->boys, sizeof( ROSTER )* PLAYERMAX );
					Yodaddy = (int)((MSGRTOAD*)&buf)->yodaddy;
				}
				break;

			default:
#ifndef SHUTUP
				fprintf( stderr, "DK IN %s:%d\tid:%d\ttype:%c\n", 
								inet_ntoa(a.sin_addr), ntohs(a.sin_port), buf[0], buf[1]);
#endif
				gamecast++;
				break;
		}
		if ( gamecast )
		{
			dmsg.id = buf[ 0 ];
			dmsg.type = MSGD;
			dmsg.seq = outseq++;
			dmsg.yodaddy = Me;
			a.sin_port = htons( Option.Port );
			memcpy( dmsg.boys, Player, sizeof( PLAYER ) * PLAYERMAX );
			ret = sendto( Server.sock, &dmsg, sizeof(dmsg), 0, (struct sockaddr*)&a, sizeof( a ));
#ifndef SHUTUP
			fprintf( stderr, "msgD OUT %s on %d size:%d\n", inet_ntoa(a.sin_addr), ntohs(a.sin_port), ret );
#endif
			cmsg.id = buf[0];
			cmsg.type = MSGC;
			cmsg.seq = outseq++;
			a.sin_port = htons( Option.Port );
			for ( i = 0; i < MISSLECOUNT; i++)
			{
				memcpy( &cmsg.girls[ i ], &Tank.missle[ i ], sizeof ( SPRITEDATA ));
			}
			ret = sendto( Server.sock, &cmsg, sizeof(MSGCTOAD), 0, (struct sockaddr*)&a, sizeof(a));
#ifndef SHUTUP
			fprintf( stderr, "msgC OUT %s:%d size:%d\n", inet_ntoa(a.sin_addr), ntohs(a.sin_port), ret );
#endif
		}
		if ( playcast ) // roster has changed
		{
			for ( i = 0; i < PLAYERMAX; i++ )
			{
				if ( Roster[ i ].ip[ 0 ] != '\0' )
				{
					a.sin_family = AF_INET;
					a.sin_port = htons( Option.Port );
					a.sin_addr.s_addr = inet_addr( Roster[ i ].ip );
					rmsg.id = i;
					rmsg.type = MSGR;
					rmsg.seq = outseq++;
					rmsg.yodaddy = Me;
					a.sin_port = htons( Option.Port );
					memcpy( rmsg.boys, Roster, sizeof( ROSTER ) * PLAYERMAX );
					ret = sendto( Server.sock, &rmsg, sizeof(rmsg), 0, (struct sockaddr*)&a, sizeof( a ));
#ifndef SHUTUP
					fprintf( stderr, "msgR OUT %s on %d size:%d\n", inet_ntoa(a.sin_addr), ntohs(a.sin_port), ret );
#endif
				}
			}
		}

	}
	return( ret );
}

// --------------------------------------------------------------------
// int pondclientinit( char *hostname or address )
// --------------------------------------------------------------------
int pondclientinit( char *host )
{
	if (( CurrentHost = gethostbyname( host ))== NULL )
	{
		herror("gethostbyname");
		exit(1);
	}
	fprintf( stderr, "Server name: %s\n", CurrentHost->h_name );
	fprintf( stderr, "IP Address : %s\n",inet_ntoa( *((struct in_addr *)CurrentHost->h_addr )));
	
	Client.sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( Client.sock == -1 )
		perror( "pondclientinit" );
	// ret = connect( Client.sock, (struct sockaddr*)&Client.addr, sizeof(struct sockaddr));		
	return( Client.sock );
}

// --------------------------------------------------------------------
// int pondclientsignedup( char* )
// args:	
// 		a players name
// returns:	
// 		index in player roster or PLAYERMAX if not signed up 
// --------------------------------------------------------------------
short pondclientsignedup( char *name )
{
	int i, ret;

	for ( i = 0; i < PLAYERMAX; i++ )
	{
		ret = strcmp( Roster[ i ].name, name );
		if ( ret == 0 )
		{
			ret = i;
			i = PLAYERMAX;
		}
		else
			ret = PLAYERMAX;
	}
	return( ret );
}
// --------------------------------------------------------------------
// int pondclientsignup( void ) send hello to current server and join the game
// args:
// 		server name or address
// returns:
// 		number of bytes sent (return value of send)
// --------------------------------------------------------------------
int pondclientsignup( void ) 
{
	return( pondclientMsgA(Me, MSGNAME, Option.FrogName ));
}

// --------------------------------------------------------------------
// int pondserversignupplayer( char *name, char *ip )
// args:
// 		player name to add
// returns:
// 		index number of player or MAXPLAYER if game full
// --------------------------------------------------------------------
int pondserversignupplayer( char *name, char *ip )
{
	int ret, i;

	ret = pondclientsignedup( name );
	if ( ret == PLAYERMAX )
	{
		for ( i = 0; i < PLAYERMAX; i++ )
		{
			if ( strcmp( "", Roster[ i ].name )== 0 )
			{
				strncpy( Roster[ i ].name, name, 29 );
				strncpy( Roster[ i ].ip, ip, 29 );
				strcpy( Roster[ i ].rm, "mine" );
				Player[ i ].id = 0; // -1 robot -2 invisible
				Player[ i ].bc = 0;
				strcpy( Roster[ i ].rm, "mine" ); // default room
				playerrefresh( i ); // ahhh
				ret = i;
				i = PLAYERMAX;
			}
		}
	}
	return( ret );
}

// --------------------------------------------------------------------
// int pondclientMsgA(char id, char type, char *load )
// args:
// 		player index, type of msg from pond.h, ascii string to send
// returns:
// 		number of bytes sent
// --------------------------------------------------------------------
int pondclientMsgA(short id, char type, char *load )
{
	MSGATOAD msg;

	msg.id = (char)id;
	msg.type = type;
	strcpy( msg.load, load );

	Client.addr.sin_family = AF_INET;
	memcpy( &Client.addr.sin_addr.s_addr, CurrentHost->h_addr, CurrentHost->h_length);
	Client.addr.sin_port = htons( Option.Port );
	memset( &Client.addr.sin_zero, 0, sizeof(Client.addr.sin_zero ));

	// return ( send( Client.sock, &msg, sizeof(msg), 0 ));
	return ( sendto( Client.sock, &msg, sizeof(msg), 0, \
		(struct sockaddr *)&Client.addr, sizeof(struct sockaddr)));
}

// --------------------------------------------------------------------
// int pondclientMsgB(char id, char type, char load )
// args:
// 		player index, type of msg from pond.h, byte to send
// returns:
// 		number of bytes sent usually 3
// --------------------------------------------------------------------
int pondclientMsgB(short id, char type, char load )
{
	MSGBTOAD msg;

	msg.id = (char)id;
	msg.type = type;
	msg.load = load;

	Client.addr.sin_family = AF_INET;
	memcpy( &Client.addr.sin_addr.s_addr, CurrentHost->h_addr, CurrentHost->h_length);
	Client.addr.sin_port = htons( Option.Port );
	memset( &Client.addr.sin_zero, 0, sizeof( Client.addr.sin_zero ));

	//return ( send( Client.sock, &msg, sizeof(msg), 0 ));
	return ( sendto( Client.sock, &msg, sizeof(msg), 0, \
		(struct sockaddr *)&Client.addr, sizeof(struct sockaddr)));
}

// --------------------------------------------------------------------
// void pondrippleupdate( void )
// 		client send command in global FROGCOMMAND and recieve game data
// --------------------------------------------------------------------
int pondrippleupdate( void )
{
	int ret = -1;

	switch( Cmd.type )
	{
		// A
		case MSGNAME:
		case MSGHAIL:
		case MSGREST:
			ret = pondclientMsgA( Me, Cmd.type, (void*)Cmd.aload );
			memset( &Cmd, 0, sizeof(Cmd));
			break;
		// B
		case MSGFIRE:
		case MSGMOVE:
		default:			// send crap we don't care
			ret = pondclientMsgB( Me, Cmd.type, Cmd.bload );
			memset( &Cmd, 0, sizeof( Cmd ));
			break;
	}
	return( ret );
}
/*
	if ( ret < 0 )
	{
		switch( errno )
		{
			case EBADF:
				puts( "pondrippleupdate EBADF" );
				break;
			case ENOTSOCK:
				puts( "pondrippleupdate ENOTSOCK" );
				break;
			case EFAULT:
				puts( "pondrippleupdate EFAULT" );
				break;
			case EMSGSIZE:
				puts( "pondrippleupdate EMSGSIZE" );
				break;
			case EAGAIN:
				puts( "pondrippleupdate EAGAIN" );
				break;
			case ENOBUFS:
				puts( "pondrippleupdate ENOBUFS never happens in linux" );
				break;
			case EINTR:
				puts( "pondrippleupdate EINTR" );
				break;
			case ENOMEM:
				puts( "pondrippleupdate ENOMEM" );
				break;
			case EINVAL:
				puts( "pondrippleupdate EINVAL" );
				break;
			case EPIPE:
				puts( "pondrippleupdate EPIPE" );
				break;
			default:
				puts( "pondrippleupdate ERROR UNKNOWN" );
				break;
		}
		exit( -1 );
	}
*/

// --------------------------------------------------------------------
// ponddestroy - clean up at exit
// --------------------------------------------------------------------
void ponddestroy( void )
{
	if ( Server.sock > 0  )
	{
		fprintf( stderr, "pond splash destroyed\n" );
		sock_close( Server.sock );
		Server.sock = -1;
	}
	if ( Client.sock > 0 )
	{
		fprintf( stderr, "pond ripple destroyed\n" );
		sock_close( Client.sock );
		Client.sock = -1;
	}
	tankdestroy();
}

// --------------------------------------------------------------------
// pondsplash main server thread 
// --------------------------------------------------------------------
int pondsplash( void *ptarg )
{
	(void)ptarg;
	atexit( ponddestroy );

	if( ! pondserverinit())
	{
		while( pondsplashupdate() >= 0 )
			;
	}
	return( 0 );
}

// --------------------------------------------------------------------
// pondripple main client thread 
// --------------------------------------------------------------------
int pondripple( void *ptarg )
{
	int i, dude, retries = 3;
	char *host = (char*)ptarg;


	atexit( ponddestroy );

	if ( pondclientinit( host ) > 0 )
	{
		fprintf( stderr, "ripple to %s ok\n", host );
		for( i = 0; i <= retries; i++ ) // re tries you're out
		{
			fprintf( stderr, "checking for %s\n", Option.FrogName );
			if (( dude = pondclientsignedup( Option.FrogName )) < PLAYERMAX )
			{
				fprintf( stderr, "connected to %s number %d\n", host, dude );
				Me = dude;
				i = 10;
			}
			else
			{
				fprintf( stderr, "connect to %s number %d dude: %d\n", host, i, dude );
				pondclientsignup();
				SDL_Delay( Nrate * 20 );
				if ( i == retries )
				{
					fprintf( stderr, "connection to %s failed\n", host );
					exit( 0 );
				}
			}
		}
		while( pondrippleupdate() >= 0 )
		{
			SDL_Delay(timeleft(Nrate));
		}
	}
	else
	{
		fprintf( stderr, "ripple to %s failed\n", host );
		exit( -1 );
	}
	return( 0 );
}
// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
// eof
