#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "frogs.h"
#include "util.h"
#include "player.h"
#include "sprite.h"
#include "tank.h"
#include "pond.h"

int main( void )
{
	printf("size of short is %d bytes\n", sizeof( short ));
	printf("size of char is %d bytes\n", sizeof( char ));
	printf("size of int is %d bytes\n", sizeof( int ));
	printf("size of long is %d bytes\n", sizeof( long ));
	printf("size of MSGATOAD is %d bytes\n", sizeof( MSGATOAD ));
	printf("size of MSGBTOAD is %d bytes\n", sizeof( MSGBTOAD ));
	printf("size of MSGCTOAD is %d bytes\n", sizeof( MSGCTOAD ));
	printf("size of MSGDTOAD is %d bytes\n", sizeof( MSGDTOAD ));
	return 1;
}
