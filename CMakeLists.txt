cmake_minimum_required(VERSION 3.16)
project(FrogsOfWarII VERSION 2.2 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(GNUInstallDirs)

# SDL2 — modern SDL2 provides SDL2::SDL2 CMake targets
find_package(SDL2 REQUIRED)

# SDL2_mixer — try CMake config first, fall back to pkg-config
find_package(SDL2_mixer QUIET)
if(NOT SDL2_mixer_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)
endif()

set(SOURCES
    src/chardisp.c
    src/ceo.c
    src/frogs.c
    src/helm.c
    src/magi.c
    src/player.c
    src/pond.c
    src/racket.c
    src/room.c
    src/sprite.c
    src/tank.c
    src/util.c
)

add_executable(frogs ${SOURCES})

if(WIN32)
    configure_file(frogs.rc.in ${CMAKE_CURRENT_BINARY_DIR}/frogs.rc @ONLY)
    target_sources(frogs PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/frogs.rc)
endif()

if(WIN32)
    set(_pkgdatadir_default ".")
else()
    set(_pkgdatadir_default "${CMAKE_INSTALL_FULL_DATADIR}/frogs")
endif()
set(PKGDATADIR "${_pkgdatadir_default}" CACHE STRING
    "Runtime path to game data directory (use . for portable/bundled builds)")

target_compile_definitions(frogs PRIVATE "PKGDATADIR=\"${PKGDATADIR}\"")

# SDL2 link
if(TARGET SDL2::SDL2)
    if(WIN32 AND TARGET SDL2::SDL2main)
        # SDL2main must precede SDL2: it depends on SDL2 symbols (SDL_strlen etc.)
        # and GNU ld resolves archives in a single left-to-right pass.
        target_link_libraries(frogs PRIVATE SDL2::SDL2main SDL2::SDL2)
    else()
        target_link_libraries(frogs PRIVATE SDL2::SDL2)
    endif()
else()
    target_include_directories(frogs PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(frogs PRIVATE ${SDL2_LIBRARIES})
endif()

# SDL2_mixer link
if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(frogs PRIVATE SDL2_mixer::SDL2_mixer)
else()
    target_include_directories(frogs PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(frogs PRIVATE ${SDL2_MIXER_LIBRARIES})
    if(SDL2_MIXER_LIBRARY_DIRS)
        target_link_directories(frogs PRIVATE ${SDL2_MIXER_LIBRARY_DIRS})
    endif()
endif()

# Windows: Winsock2 for networking in pond.c
if(WIN32)
    target_link_libraries(frogs PRIVATE ws2_32)
endif()

# Install
install(TARGETS frogs RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY bmp wav doc
    DESTINATION ${CMAKE_INSTALL_DATADIR}/frogs
)

if(UNIX AND NOT APPLE)
    # frogs.desktop.in uses @pkgdatadir@ (lowercase, shared with the autotools
    # build); configure_file(@ONLY) matches the variable name case-sensitively.
    set(pkgdatadir "${CMAKE_INSTALL_FULL_DATADIR}/frogs")
    configure_file(frogs.desktop.in frogs.desktop @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/frogs.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
endif()
