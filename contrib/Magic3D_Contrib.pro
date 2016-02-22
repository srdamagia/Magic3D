TARGET = magic3d_contrib
TEMPLATE = lib
DESTDIR = ../game

OBJECTS_DIR = ./build
UI_DIR = ./build
MOC_DIR = ./build

CONFIG -= exceptions
CONFIG += staticlib warn_off c++11

CONTRIB = ./

include(Magic3D_Base.pro)
include(Magic3D_Base_Source.pro)
include(Magic3D_Base_Include.pro)

SOURCES += \
    ./magic3d_contrib.cpp

HEADERS += \
    ./magic3d_contrib.h
