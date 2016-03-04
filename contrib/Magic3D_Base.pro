MEMORY_MANAGER = $$CONTRIB/memory_manager
BULLET         = $$CONTRIB/bullet           #2.82
VORBIS         = $$CONTRIB/libvorbis        #1.3.5
OGG            = $$CONTRIB/libogg           #1.3.2
THEORA         = $$CONTRIB/libtheora        #1.1.1
TINYXML        = $$CONTRIB/tinyxml2
ZLIB           = $$CONTRIB/zlib             #1.2.8
MINIZIP        = $$CONTRIB/minizip          #1.1
VECTORMATH     = $$CONTRIB/vectormath
PNG            = $$CONTRIB/lpng             #1.6.21
LUA            = $$CONTRIB/lua              #5.3.2
ENET           = $$CONTRIB/enet             #1.3.13
SDL            = $$CONTRIB/sdl              #2.0.4
IMGUI          = $$CONTRIB/imgui            #1.48

INCLUDEPATH += \
    $$MEMORY_MANAGER \
    $$BULLET \
    $$VORBIS \
    $$VORBIS/lib \
    $$OGG \
    $$THEORA \
    $$TINYXML \
    $$ZLIB \
    $$MINIZIP \
    $$VECTORMATH/cpp \
    $$LUA \
    $$ENET \
    $$IMGUI
win32|macx:INCLUDEPATH += $$PNG
win32:INCLUDEPATH += $$SDL/include
macx:INCLUDEPATH += /Library/Frameworks/SDL2.framework/Headers

MEMORY_MANAGER_SOURCE  = $$MEMORY_MANAGER
MEMORY_MANAGER_INCLUDE = $$MEMORY_MANAGER

BULLET_SOURCE  = $$BULLET
BULLET_INCLUDE = $$BULLET

VORBIS_SOURCE  = $$VORBIS/lib
VORBIS_INCLUDE = $$VORBIS/vorbis

OGG_SOURCE  = $$OGG/src
OGG_INCLUDE = $$OGG/ogg

THEORA_SOURCE  = $$THEORA/lib
THEORA_INCLUDE = $$THEORA/theora

TINYXML_SOURCE  = $$TINYXML
TINYXML_INCLUDE = $$TINYXML

ZLIB_SOURCE  = $$ZLIB
ZLIB_INCLUDE = $$ZLIB

MINIZIP_SOURCE  = $$MINIZIP
MINIZIP_INCLUDE = $$MINIZIP

VECTORMATH_INCLUDE = $$VECTORMATH/cpp

PNG_SOURCE  = $$PNG
PNG_INCLUDE = $$PNG

LUA_SOURCE  = $$LUA
LUA_INCLUDE = $$LUA

ENET_SOURCE  = $$ENET
ENET_INCLUDE = $$ENET/enet

SDL_SOURCE  = $$SDL/src
SDL_INCLUDE = $$SDL/include

IMGUI_SOURCE  = $$IMGUI
IMGUI_INCLUDE = $$IMGUI
