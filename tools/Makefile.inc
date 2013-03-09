LUA_INC_DIR	=	/usr/include/lua50

UTIL_DIR	=	$(SRC_DIR)/util
UTIL_SRC	=	$(wildcard $(UTIL_DIR)/*.cpp)

MATH_DIR	=	$(SRC_DIR)/math
MATH_SRC	=	$(wildcard $(MATH_DIR)/*.cpp)

SCENE_DIR	=	$(SRC_DIR)/scene
SCENE_SRC	=	$(wildcard $(SCENE_DIR)/*.cpp)

DEPS		=	$(wildcard $(UTIL_DIR)/*.h)
SRC		=	$(UTIL_SRC) $(MATH_SRC) $(SCENE_SRC)

$(SCENE_DIR)/loader.o: CXXFLAGS += -I$(LUA_INC_DIR)

CXX		=	g++
CXXFLAGS	=	-W -Wall -I$(UTIL_DIR) -I$(MATH_DIR) -I$(SCENE_DIR) #-std=c++0x
LDFLAGS 	=	-lGL -lGLU -lGLEW -lrt -lpng12 -llua50 -llualib50

.PHONY: rel
rel: CXXFLAGS += -O3 -DNDEBUG
rel: all

.PHONY: dbg
dbg: CXXFLAGS += -g -D_DEBUG
dbg: all
