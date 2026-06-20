#ifndef FROGS_COMPAT_H
#define FROGS_COMPAT_H

/* On Windows, winsock2.h must come before any other header (including SDL).
   It pulls in windows.h/windef.h which define RECT and POINT as Windows types.
   frogs.h later does #define RECT SDL_Rect which shadows the Windows typedef,
   so game code using RECT gets SDL_Rect fields (x,y,w,h) as expected.
   util.h skips its own POINT typedef on Windows; windef.h's POINT (LONG x,y)
   is ABI-compatible with our int x,y on all Windows targets. */
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#endif

/* The game's data files (*.elf, robots*.txt) use CRLF line endings. On Windows
   open()/read() default to text mode and translate CRLF->LF, so read() returns
   fewer bytes than the file size and parsers that check actual==sizeofile bail
   out, leaving arrays unallocated and causing NULL writes. Open such files with
   O_BINARY. fcntl.h is included here first so that on Windows its real O_BINARY
   is picked up; on POSIX (where no translation happens and O_BINARY is
   undefined) it maps to 0. */
#include <fcntl.h>
#ifndef O_BINARY
#  define O_BINARY 0
#endif

#endif /* FROGS_COMPAT_H */
