BULLET         = $$MAGIC3D/bullet           #2.82
VORBIS         = $$MAGIC3D/libvorbis        #1.3.5
OGG            = $$MAGIC3D/libogg           #1.3.2
THEORA         = $$MAGIC3D/libtheora        #1.1.1
MEMORY_MANAGER = $$MAGIC3D/memory_manager
TINYXML        = $$MAGIC3D/tinyxml2
ZLIB           = $$MAGIC3D/zlib             #1.2.8
MINIZIP        = $$MAGIC3D/minizip          #1.1
VECTORMATH     = $$MAGIC3D/vectormath
PNG            = $$MAGIC3D/lpng             #1.6.16
LUA            = $$MAGIC3D/lua              #5.3.0
ENET           = $$MAGIC3D/enet             #1.3.13

INCLUDEPATH += \
    $$MAGIC3D/magic3d \
    $$MEMORY_MANAGER \
    $$BULLET \
    $$VORBIS \
    $$VORBIS/lib \
    $$OGG \
    $$THEORA \
    $$ZLIB \
    $$MINIZIP \
    $$TINYXML \
    $$VECTORMATH/cpp \
    $$LUA \
    $$ENET
win32|macx:INCLUDEPATH += $$PNG \

#MEMORY_MANAGER *****************************************************************
MEMORY_MANAGER_SOURCE  = $$MEMORY_MANAGER
MEMORY_MANAGER_INCLUDE = $$MEMORY_MANAGER

SOURCES += \
    $$MEMORY_MANAGER_SOURCE/*.cpp \

HEADERS += \
    $$MEMORY_MANAGER_INCLUDE/*.h \
#********************************************************************************

#BULLET ************************************************************************
BULLET_SOURCE  = $$BULLET
BULLET_INCLUDE = $$BULLET

SOURCES += \
    $$BULLET_SOURCE/BulletCollision/BroadphaseCollision/*.cpp \
    $$BULLET_SOURCE/BulletCollision/CollisionDispatch/*.cpp \
    $$BULLET_SOURCE/BulletCollision/CollisionShapes/*.cpp \
    $$BULLET_SOURCE/BulletCollision/Gimpact/*.cpp \
    $$BULLET_SOURCE/BulletCollision/NarrowPhaseCollision/*.cpp \
#
    $$BULLET_SOURCE/BulletDynamics/Character/*.cpp \
    $$BULLET_SOURCE/BulletDynamics/ConstraintSolver/*.cpp \
    $$BULLET_SOURCE/BulletDynamics/Dynamics/*.cpp \
    $$BULLET_SOURCE/BulletDynamics/Vehicle/*.cpp \
#
    $$BULLET_SOURCE/BulletSoftBody/*.cpp \
#
    $$BULLET_SOURCE/LinearMath/*.cpp \

HEADERS += \
    $$BULLET_INCLUDE/*.h \
#
    $$BULLET_INCLUDE/BulletCollision/BroadphaseCollision/*.h \
    $$BULLET_INCLUDE/BulletCollision/CollisionDispatch/*.h \
    $$BULLET_INCLUDE/BulletCollision/CollisionShapes/*.h \
    $$BULLET_INCLUDE/BulletCollision/Gimpact/*.h \
    $$BULLET_INCLUDE/BulletCollision/NarrowPhaseCollision/*.h \
#
    $$BULLET_INCLUDE/BulletDynamics/Character/*.h \
    $$BULLET_INCLUDE/BulletDynamics/ConstraintSolver/*.h \
    $$BULLET_INCLUDE/BulletDynamics/Dynamics/*.h \
    $$BULLET_INCLUDE/BulletDynamics/Vehicle/*.h \
#
    $$BULLET_INCLUDE/BulletSoftBody/*.h \
#
    $$BULLET_INCLUDE/LinearMath/*.h \
    ../magic3d/magic3d/magic3d/resource/resources.h
#********************************************************************************

#VORBIS *************************************************************************
VORBIS_SOURCE  = $$VORBIS/lib
VORBIS_INCLUDE = $$VORBIS/vorbis

SOURCES += \
    $$VORBIS_SOURCE/*.c \

HEADERS += \
    $$VORBIS_SOURCE/*.h \
#
    $$VORBIS_SOURCE/books/coupled/*.h \
    $$VORBIS_SOURCE/books/floor/*.h \
    $$VORBIS_SOURCE/books/uncoupled/*.h \
#
    $$VORBIS_SOURCE/modes/*.h \
#
    $$VORBIS_INCLUDE/*.h \
#********************************************************************************

#OGG ****************************************************************************
OGG_SOURCE  = $$OGG/src
OGG_INCLUDE = $$OGG/ogg

SOURCES += \
    $$OGG_SOURCE/*.c \

HEADERS += \
    $$OGG_INCLUDE/*.h \
#********************************************************************************

#THEORA *************************************************************************
THEORA_SOURCE  = $$THEORA/lib
THEORA_INCLUDE = $$THEORA/theora

SOURCES += \
    $$THEORA_SOURCE/*.c \

HEADERS += \
    $$THEORA_SOURCE/*.h \
    $$THEORA_INCLUDE/*.h \
#********************************************************************************

#TINYXML ************************************************************************
TINYXML_SOURCE  = $$TINYXML
TINYXML_INCLUDE = $$TINYXML

SOURCES += \
    $$TINYXML_SOURCE/*.cpp \

HEADERS += \
    $$TINYXML_INCLUDE/*.h \
#********************************************************************************

#ZLIB ***************************************************************************
ZLIB_SOURCE  = $$ZLIB
ZLIB_INCLUDE = $$ZLIB

SOURCES += \
    $$ZLIB_SOURCE/*.c \

HEADERS += \
    $$ZLIB_INCLUDE/*.h \
#********************************************************************************

#MINIZIP ***************************************************************************
MINIZIP_SOURCE  = $$MINIZIP
MINIZIP_INCLUDE = $$MINIZIP

SOURCES += \
    $$MINIZIP_SOURCE/*.c \

HEADERS += \
    $$MINIZIP_INCLUDE/*.h \
#********************************************************************************

#VECTORMATH *********************************************************************
VECTORMATH_SOURCE  = $$VECTORMATH/scalar/cpp
VECTORMATH_INCLUDE = $$VECTORMATH/cpp

HEADERS += \
    $$VECTORMATH_SOURCE/*.h \
    $$VECTORMATH_INCLUDE/*.h \
#********************************************************************************

#PNG ****************************************************************************
win32|macx: {
PNG_SOURCE  = $$PNG
PNG_INCLUDE = $$PNG

SOURCES += \
    $$PNG_SOURCE/*.c \

HEADERS += \
    $$PNG_INCLUDE/*.h \
}
#********************************************************************************

#LUA ****************************************************************************
LUA_SOURCE  = $$LUA
LUA_INCLUDE = $$LUA

SOURCES += \
    $$LUA_SOURCE/*.c \

HEADERS += \
    $$LUA_INCLUDE/*.h \
#********************************************************************************

#ENET ***************************************************************************
ENET_SOURCE  = $$ENET
ENET_INCLUDE = $$ENET/enet

SOURCES += \
    $$ENET_SOURCE/*.c \

HEADERS += \
    $$ENET_INCLUDE/*.h \
#********************************************************************************

#Magic3D ************************************************************************
MAGIC3D_SOURCE  = $$MAGIC3D/magic3d/src
MAGIC3D_INCLUDE = $$MAGIC3D/magic3d/magic3d

SOURCES += \
    $$MAGIC3D_SOURCE/*.cpp \
    $$MAGIC3D_SOURCE/gui/*.cpp \
    $$MAGIC3D_SOURCE/image/*.cpp \
    $$MAGIC3D_SOURCE/model/*.cpp \
    $$MAGIC3D_SOURCE/renderer/*.cpp \
    $$MAGIC3D_SOURCE/renderer/opengl/*.cpp \
    $$MAGIC3D_SOURCE/resource/*.cpp \
    $$MAGIC3D_SOURCE/sound/*.cpp \
    $$MAGIC3D_SOURCE/script/*.cpp \
    $$MAGIC3D_SOURCE/tween/*.cpp \
    $$MAGIC3D_SOURCE/physics/*.cpp \

win32:SOURCES += \
    $$MAGIC3D_SOURCE/image/*.mm \

HEADERS += \
    $$MAGIC3D_INCLUDE/*.h \
    $$MAGIC3D_INCLUDE/gui/*.h \
    $$MAGIC3D_INCLUDE/image/*.h \
    $$MAGIC3D_INCLUDE/model/*.h \
    $$MAGIC3D_INCLUDE/renderer/*.h \
    $$MAGIC3D_INCLUDE/renderer/opengl/*.h \
    $$MAGIC3D_INCLUDE/resource/*.h \
    $$MAGIC3D_INCLUDE/sound/*.h \
    $$MAGIC3D_INCLUDE/script/*.h \
    $$MAGIC3D_INCLUDE/tween/*.h \
    $$MAGIC3D_INCLUDE/physics/*.h \

macx:OBJECTIVE_SOURCES += \
    $$MAGIC3D_SOURCE/image/*.mm \
#********************************************************************************
