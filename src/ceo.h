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
#define CEOBMPWIDTH 640
#define CEOBMPHEIGHT 479
#define CEODBMPWIDTH 320
#define CEODBMPHEIGHT 240

typedef struct
{
    SDL_Surface *lpCeo;    // our guy
    SDL_Surface *lpCeod;   // decorations
    short show;            // turn on off drawing
    short status;          // if !0 fatal error occurred in constructor
    short timer;           // counter counting how many times draw called
    short delay;           // time to show ceo and play sound
    char name[ 32 ];       // basename of the bitmap or resource
    RECT dest;            // destination of bitmap
    RECT sport;            // target  rectangle of bitmap
} CEO;

void ceoinit( void );
void ceodestroy( void );
void ceorestoreSurfaces( void );
void ceochangeCeo( void );
void ceodraw( SDL_Surface * );
