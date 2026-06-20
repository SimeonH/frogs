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

#endif /* FROGS_COMPAT_H */
