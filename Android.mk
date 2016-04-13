LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI ?= $(APP_ABI)

###########################
#
# Magic3D shared library
#
###########################
include $(CLEAR_VARS)
LOCAL_MODULE            := SDL2
LOCAL_SRC_FILES         := $(LOCAL_PATH)/android/$(TARGET_ARCH_ABI)/libSDL2.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := openal
LOCAL_SRC_FILES         := $(LOCAL_PATH)/android/$(TARGET_ARCH_ABI)/libopenal.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := magic3d

CONTRIB := $(LOCAL_PATH)/contrib
MAGIC3D := $(LOCAL_PATH)/magic3d

BULLET         := $(CONTRIB)/bullet
VORBIS         := $(CONTRIB)/libvorbis
OGG            := $(CONTRIB)/libogg
THEORA         := $(CONTRIB)/libtheora
TINYXML        := $(CONTRIB)/tinyxml2
ZLIB           := $(CONTRIB)/zlib
MINIZIP        := $(CONTRIB)/minizip
VECTORMATH     := $(CONTRIB)/vectormath
PNG            := $(CONTRIB)/lpng
LUA            := $(CONTRIB)/lua
ENET           := $(CONTRIB)/enet
SDL            := $(CONTRIB)/sdl
IMGUI          := $(CONTRIB)/imgui
OPENAL         := $(CONTRIB)/openal

MAGIC3D_SOURCE     := $(MAGIC3D)/src
BULLET_SOURCE      := $(BULLET)
VORBIS_SOURCE      := $(VORBIS)/lib
OGG_SOURCE         := $(OGG)/src
THEORA_SOURCE      := $(THEORA)/lib
TINYXML_SOURCE     := $(TINYXML)
ZLIB_SOURCE        := $(ZLIB)
MINIZIP_SOURCE     := $(MINIZIP)
VECTORMATH_INCLUDE := $(VECTORMATH)/cpp
PNG_SOURCE         := $(PNG)
LUA_SOURCE         := $(LUA)
ENET_SOURCE        := $(ENET)
SDL_SOURCE         := $(SDL)/src
IMGUI_SOURCE       := $(IMGUI)

LOCAL_C_INCLUDES := \
    $(BULLET) \
    $(VORBIS) \
    $(VORBIS)/lib \
    $(OGG) \
    $(THEORA) \
    $(TINYXML) \
    $(ZLIB) \
    $(MINIZIP) \
    $(VECTORMATH)/cpp \
    $(LUA) \
    $(ENET) \
    $(IMGUI) \
    $(PNG) \
    $(SDL)/include \
	$(OPENAL)/include \
	$(MAGIC3D)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(BULLET_SOURCE)/BulletCollision/BroadphaseCollision/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletCollision/CollisionDispatch/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletCollision/CollisionShapes/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletCollision/Gimpact/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletCollision/NarrowPhaseCollision/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletDynamics/Character/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletDynamics/ConstraintSolver/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletDynamics/Dynamics/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletDynamics/Vehicle/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/BulletSoftBody/*.cpp) \
    $(wildcard $(BULLET_SOURCE)/LinearMath/*.cpp) \
    $(wildcard $(VORBIS_SOURCE)/*.c) \
    $(wildcard $(OGG_SOURCE)/*.c) \
    $(wildcard $(THEORA_SOURCE)/*.c) \
    $(wildcard $(TINYXML_SOURCE)/*.cpp) \
    $(wildcard $(ZLIB_SOURCE)/*.c) \
    $(MINIZIP_SOURCE)/ioapi.c \
    $(MINIZIP_SOURCE)/ioapi_buf.c \
    $(MINIZIP_SOURCE)/ioapi_mem.c \
    $(MINIZIP_SOURCE)/unzip.c \
    $(MINIZIP_SOURCE)/zip.c \
    $(wildcard $(PNG_SOURCE)/*.c) \
    $(wildcard $(LUA_SOURCE)/*.c) \
    $(wildcard $(ENET_SOURCE)/*.c) \
    $(wildcard $(IMGUI_SOURCE)/*.cpp) \
	$(wildcard $(MAGIC3D_SOURCE)/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/gui/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/gui/imgui_window/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/image/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/model/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/renderer/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/renderer/opengl/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/resource/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/sound/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/script/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/tween/*.cpp) \
    $(wildcard $(MAGIC3D_SOURCE)/physics/*.cpp)) \
	$(LOCAL_PATH)/SDL_android_main.c \
	$(LOCAL_PATH)/android.cpp
	

LOCAL_CFLAGS += -DMAGIC3D_OES2 -DMAGIC3D_LOG -DGL_GLEXT_PROTOTYPES -DANDROID_NDK -DHAS_SOCKLEN_T -DMAGIC3D_ANDROID
ifeq ($(TARGET_ARCH_ABI), x86)
	LOCAL_CFLAGS += -DMAGIC3D_ABI_x86
endif
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := openal

include $(BUILD_SHARED_LIBRARY)

###########################
#
# Magic3D static library
#
###########################

LOCAL_MODULE := magic3d_static

LOCAL_MODULE_FILENAME := libmagic3d

#LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,,$(LOCAL_PATH)/src/main/android/SDL_android_main.c)

LOCAL_LDLIBS := 
LOCAL_EXPORT_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := openal

include $(BUILD_STATIC_LIBRARY)
