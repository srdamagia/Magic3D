win32:RC_FILE = icons.rc

TARGET = magic3d
TEMPLATE = app
DESTDIR = ./game

OBJECTS_DIR = ./build
UI_DIR = ./build
MOC_DIR = ./build

CONFIG -= exceptions

win32:LIBS += -mwindows -lwsock32 -lwinmm -lws2_32 -lmingw32 -lopengl32 -lOpenAL32
unix:!macx:LIBS += -lopenal
macx:LIBS += -framework OpenAL

DEFINES += MAGIC3D_LOG MAGIC3D_MEMORY

MAGIC3D = ./

include(Magic3D_Base.pro)

SOURCES += \
    ./*.cpp

DISTFILES += \
    bin/config.xml \
    game/config.xml
