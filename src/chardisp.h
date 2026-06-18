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
// On Screen Character Display object 
//--------------------------------------------------------------------------------

#define ALPHANUM	37  // number of letters and numbers in our alphabet
#define ALPHAWIDE   480 // size of our bitmap
#define ALPHAHIGH   18 
#define CHARHIGH	17  // number of pixels wide each character
#define CHARWIDE	10  // number of pixels tall each character
#define MAXLINES	20  // number of lines in display
#define MAXLENGTH   60  // length of line in display
#define MARGIN	  10  // left margin
#define LEADING	 2  // space between lines
#define LINELIFE	240  // lifespan of a message - scroll speed in 1/18 seconds
#define MAXPATH		255

typedef struct
{
	char str[ MAXLENGTH + 1 ];
	short life;
} DISPLINE;

typedef struct 
{
	short status;	   // if !0 fatal error occurred in constructor
	short top;					// newest line 
	short show;		  // should I paint or not
	RECT Alphabet[ ALPHANUM ];
	SDL_Surface *lpAlphabet;   
	DISPLINE display[ MAXLINES ]; // the display
} ONSCREENDISPLAY;

void chardispaddaline( char* );	 // insert new line before top and inc top
void chardispaddaword( char * );	// cat a string onto last added line
void chardispclear( void );		 // empty display and set top to zero
void chardispbull( char * );		// clear screen open and display a file of bull
void chardispinit( void );
void chardispdestroy( void );
short chardisphas( char * );		 // see if it's already in display
void chardispdraw( SDL_Surface * ); // paint
void chardisprestoreSurfaces( void );
