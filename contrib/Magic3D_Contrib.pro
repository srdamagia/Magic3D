win32:QMAKE_CXXFLAGS += -std=c++11

TARGET = magic3d_contrib
TEMPLATE = lib
DESTDIR = ../game

OBJECTS_DIR = ./build
UI_DIR = ./build
MOC_DIR = ./build

CONFIG -= exceptions
CONFIG += staticlib warn_off

DEFINES += MAGIC3D_MEMORY

CONTRIB = ./

include(Magic3D_Base.pro)
include(Magic3D_Base_Source.pro)
include(Magic3D_Base_Include.pro)

SOURCES += \
    ./magic3d_contrib.cpp

HEADERS += \
    ./magic3d_contrib.h
