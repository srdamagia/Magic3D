win32:RC_FILE = icons.rc
macx:ICON = icons/icons.icns
macx:QMAKE_INFO_PLIST = Info.plist

TARGET = game
TEMPLATE = app
DESTDIR = ./game

OBJECTS_DIR = ./build
UI_DIR = ./build
MOC_DIR = ./build

CONFIG -= exceptions
CONFIG += c++11

QMAKE_LFLAGS += -F/Library/Frameworks

LIBS += -L./game -lmagic3d_contrib
win32:LIBS += -mwindows -lwsock32 -lwinmm -lws2_32 -limm32 -lmingw32 -lopengl32 -lOpenAL32 -lSDL2
unix:!macx:LIBS += -lopenal -lpng -lSDL2
macx:LIBS += -framework OpenAL -framework SDL2

DEFINES += MAGIC3D_LOG MAGIC3D_MEMORY

MAGIC3D = ./
CONTRIB = ./contrib

include($$CONTRIB/Magic3D_Base.pro)
include($$CONTRIB/Magic3D_Base_Include.pro)
include(Magic3D_Engine.pro)

SOURCES += \
    ./main.cpp

DISTFILES += \    
    game/config.xml
