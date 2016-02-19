INCLUDEPATH += $$MAGIC3D/magic3d

#Magic3D ************************************************************************
MAGIC3D_SOURCE  = $$MAGIC3D/magic3d/src
MAGIC3D_INCLUDE = $$MAGIC3D/magic3d/magic3d

SOURCES += \
    $$MAGIC3D_SOURCE/*.cpp \
    $$MAGIC3D_SOURCE/gui/*.cpp \
    $$MAGIC3D_SOURCE/gui/imgui_window/*.cpp \
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
    $$MAGIC3D_INCLUDE/gui/imgui_window/*.h \
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
