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
// pond - frog network community
//--------------------------------------------------------------------------------
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#endif
#define MAXMSGLENGTH 64 		  // load size to server
#define MAXLOAD 1480			  // max load size from server

// our message types, second byte in a packet is how to cast it
#define MSGNAME 'n'	 // hello my name is...		load = handle
#define MSGFIRE 'k'	 // requesting ass kick		load = tool number 0-2
#define MSGMOVE 'm'  // request a movement 		load = direction 1-9 on keypad
#define MSGHAIL 'h'	 // text message to all		load = message
#define MSGREST 'r'	 // reset the game please	load = n/a
// -- 
#define MSGC	'g'	 // inbound game data		load = missles, varies size
#define MSGD	'b'	 // in bound game data		load = players, size fixed
#define MSGR	't'	 // in bound game data		load = players, size fixed

typedef struct
{
	int sock;
	struct sockaddr_in addr;
} SOCK;

typedef struct 	// global command continuously filled by interface thread 
{				// net client thread on the clock sends contents in msgA/B to host	
	char id;	// thread then recieves a data toad for it's trouble
	char type;
	char bload;
	char aload[ MAXMSGLENGTH + 1 ];
} FROGCOMMAND;

// three types of msgs, one a string payload two a single character and three the world
// all msgs start with two bytes, id and type for msg cracking purposes
typedef struct 						// request to server
{
	char id;						// from
	char type;						// message type (MSG_MOVETOAD etc. from above)
	unsigned long seq;				// sequence number
	char load[ MAXMSGLENGTH ];		// payload 
} MSGATOAD;							// load is ascii string

typedef struct 						// request to server
{
	char id;						// from
	char type;						// message type from above table of msg types
	unsigned long seq;				// sequence number
	char load;						// payload data " " " " " " " "
} MSGBTOAD;							// load is digit

typedef struct						// broadcast of variable size missle data
{
	char id;	
	char type;						// message type (MSGC)
	unsigned long seq;				// sequence number
	SPRITEDATA girls[ MISSLECOUNT ]; // from tank.h
} MSGCTOAD;

typedef struct						// broadcast of game data
{
	char id;	
	char type;						// message type (MSGD)
	unsigned long seq;				// sequence number
	short yodaddy;					// current server index in player array
	PLAYER boys[ PLAYERMAX ];		// game data load from server
} MSGDTOAD;

typedef struct						// broadcast of game data
{
	char id;	
	char type;						// message type (MSGR)
	unsigned long seq;				// sequence number
	short yodaddy;					// current server index in player array
	ROSTER boys[ PLAYERMAX ];		// game data load from server
} MSGRTOAD;

// server
int pondsplash( void* ); // main thread
int pondserverinit( void );
int pondsplashupdate( void );
int pondserversignupplayer( char*, char* );
// client
int pondripple( void* ); // main thread
int pondclientinit( char* );
int pondclientsignup( void );
int pondrippleupdate( void );
int pondclientMsgA(short, char, char* );
int pondclientMsgB(short, char, char );
// both
short pondsignedup( char* );
void ponddestroy( void );
// eof
