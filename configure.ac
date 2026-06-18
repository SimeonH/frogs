AC_PREREQ([2.69])
AC_INIT([Frogs Of War II], [2.2], [], [frogs])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC

PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.5],
    [],
    [AC_MSG_ERROR([SDL2 >= 2.0.5 not found. Install libsdl2-dev or sdl2.])])

PKG_CHECK_MODULES([SDL2_MIXER], [SDL2_mixer >= 2.0.0],
    [],
    [AC_MSG_ERROR([SDL2_mixer not found. Install libsdl2-mixer-dev or sdl2_mixer.])])

AC_SEARCH_LIBS([pthread_create], [pthread], [],
    [AC_MSG_ERROR([pthreads not found])])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
