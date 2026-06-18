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
// noise array maintainer and sound mixer interface
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

extern OPTIONS Option;
extern Mix_Chunk **Noise; // global array of sounds
int Racketnum; // size of Noise array

//------------------------------------
// void racketplay( int song_number, int count )
// 		play Noise[ song_number ] count number of times or -1 forever in mixer
//------------------------------------
void racketplay( short n, short duration ) 
{
	// printf("racket: %d\n", n );
	if ( Option.Sound < 1 )
		return;	
	if ( Mix_Playing( n ))
		Mix_HaltChannel(n);
	if ( Mix_PlayChannel(n, Noise[ n ], duration ) == -1 )
		fprintf( stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
}	
//-----------------------------------
// void racketvolume( int increment )
// 		positive or negative to be added to volume on all chunks but 0 music
//-----------------------------------
void racketvolume( int increment )
{
	int i;
	static int vol = 0;

	vol += increment;
	if ( vol > MIX_MAX_VOLUME )
		vol = MIX_MAX_VOLUME;
	if ( vol  < 0 )
		vol = 0;
	for ( i=0; i < Racketnum; i++ )
	{
		Mix_VolumeChunk( Noise[ i ], vol );
	}
}

//------------------------------------
// void racketon( void )
// 		speak boy
//------------------------------------
void racketoff()
{
	int i;

	if ( Option.Sound == 1 )
		Option.Sound = 0;

	for ( i=0; i < Racketnum; i++ )
	{
		Mix_HaltChannel(i);
	}
}		

//------------------------------------
// void racketoff( void )
// 		shut the fuck up
//------------------------------------
void racketon()
{
	if ( Option.Sound == 0 )
		Option.Sound = 1;
}

//----------------------------------------------------------------
// int racketnextCommand( char *instr, char *outstr, char *place )
//      find and retrieve a command string from instr starting at place
//      strip cr/lf/comments, add a terminating comma and a null
// input:
//      char *instr - compound string full of comments
//      char *outstr - null terminated command string a-z:#,#,#,\0
// returns:
//      0 - success
//      !0 - failure
//----------------------------------------------------------------
int racketnextCommand( char *place, char *outstr )
{
   char *x = place;
   char *y = outstr;
   int found = 0; // increment if command found
        
    while( *x++ && !found ) // find start of a command
    {
        if ( *x == ':' )
        {
            --x;
            if ( strchr( "#w", (int)*x ) != NULL ) // got one
            {
                found++;
                while( *x && strchr( " \t\r\n\\", (int)*x ) == NULL )  // fill output
                    *y++ = *x++;
                *y++ = '\0';
            }
            else
                while( *x != '\n' )  // skip it
                    x++;
        }
    }
            
    return found;
}

//------------------------------------
// int racketstart( void )
//	  1. open and read wav/racket.elf
//	  2. size noise array to fit
//	  3. fill noise[]'s with sound
//------------------------------------

void racketstart()
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = Option.Soundbufsz;
	char path[256] = "wav/racket.elf";
	char comstr[256] = "";
	char *text = NULL;
	char *place;
	char *nextarg;
	int rfile;
	off_t sizeofile, actual;
	int i = 0, cmd; // i = index in sound array Noise[]
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
	{
		fprintf( stderr, "error initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

	rfile = open( path,  O_RDONLY );
	if ( rfile > 1 )
	{
		sizeofile = lseek( rfile, 0L, SEEK_END );
		lseek( rfile, 0L, SEEK_SET );
		text = (char *)calloc( sizeofile + 1, sizeof(char));
		if ( text != NULL )
		{
			text[ sizeofile ] = '\0';
			place = text;
			actual = read( rfile, text, sizeofile );
			if ( actual == sizeofile )
			{
				while( racketnextCommand( place, comstr )) // first how many sounds
				{
					place += strlen( comstr );
					while ( *place && *place != '\n' ) // strip empty lines
						place++;
					nextarg = (char *)strtok( comstr, ":" );
					cmd = (int)comstr[0];
					switch( cmd )
					{
						case 35: // # total sound count
							nextarg = (char *)strtok( NULL, "\n" );
							Racketnum = atoi( nextarg ); // now we can initialize noise array
							Noise = (Mix_Chunk**)calloc( Racketnum + 1, sizeof(Mix_Chunk));
							break;
	
						case 119: // w wave
							nextarg = (char *)strtok( NULL, "\n" );
							sprintf( path, "wav/%s.wav", nextarg );
							Noise[ i ] = Mix_LoadWAV( path );
							if(Noise[ i ] == NULL) 
							fprintf( stderr, "error load %s: %s\n", path, Mix_GetError());
							else
								i++;
						default:
							break;
					}
				}
			}
			free( text );
		}
		else
			Option.Sound = -1;
		close( rfile );	
	}
	if ( Option.Sound >= 0 )
	{
		fprintf( stderr, "Allocated %d mixer channels\n", Mix_AllocateChannels(Racketnum + 1 ));
		Mix_ReserveChannels( Racketnum );
		racketvolume( MIX_MAX_VOLUME / 8 );
	}
}

//------------------------------------
// void racketstop( void )
// 		clean up
//------------------------------------
void racketstop()
{
	if ( Option.Sound < 0 ) // never initialized
		return;	
	if ( Noise != NULL )
		free( Noise );
	Mix_CloseAudio();
}
//eof
/*
int main( void )
{
	int i;

	racketstart();
	for ( i = 0; i < Racketnum; i++ )
		racketplay( i, 0 );
	for ( i = 0; i < Racketnum; i++ )
		racketplay( i, 0 );
	while(Mix_Playing(0) != 0);
	racketstop();
	exit( 0 );
}
*/
