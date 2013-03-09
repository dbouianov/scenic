LUA_INC_DIR	=	/usr/include/lua50

UTIL_DIR	=	$(SRC_DIR)/util
MATH_DIR	=	$(SRC_DIR)/math
SCENE_DIR	=	$(SRC_DIR)/scene

CXX		=	g++
CXXFLAGS	=	-std=c++0x -W -Wall -I$(UTIL_DIR) -I$(MATH_DIR) -I$(SCENE_DIR)
LDFLAGS 	=	-lGL -lGLU -lGLEW -lrt -lpng12 -llua50 -llualib50

UTIL_SRC	=	$(UTIL_DIR)/logger.cpp		\
			$(UTIL_DIR)/config.cpp		\
			$(UTIL_DIR)/image.cpp		\
			$(UTIL_DIR)/timer.cpp

MATH_SRC	=	$(MATH_DIR)/floatmath.cpp

SCENE_SRC	=	$(SCENE_DIR)/scenenode.cpp	\
			$(SCENE_DIR)/groupnode.cpp	\
			$(SCENE_DIR)/transform.cpp	\
			$(SCENE_DIR)/primitive.cpp	\
			$(SCENE_DIR)/psystem.cpp	\
			$(SCENE_DIR)/material.cpp	\
			$(SCENE_DIR)/texture.cpp	\
			$(SCENE_DIR)/displist.cpp	\
			$(SCENE_DIR)/skybox.cpp		\
			$(SCENE_DIR)/light.cpp		\
			$(SCENE_DIR)/font.cpp		\
			$(SCENE_DIR)/camera.cpp		\
			$(SCENE_DIR)/factory.cpp	\
			$(SCENE_DIR)/object.cpp		\
			$(SCENE_DIR)/loader.cpp		\
			$(SCENE_DIR)/context.cpp

DEPS		=	$(UTIL_DIR)/common.h		\
			$(UTIL_DIR)/custom.h		\
			$(UTIL_DIR)/default.h		\
			$(UTIL_DIR)/logger.h

SRC		=	$(UTIL_SRC) $(MATH_SRC) $(SCENE_SRC)

$(SCENE_DIR)/loader.o: CXXFLAGS += -I$(LUA_INC_DIR)

.PHONY: rel dbg all test clean
rel: CXXFLAGS += -O3 -DNDEBUG
rel: all
dbg: CXXFLAGS += -g -D_DEBUG
dbg: all
