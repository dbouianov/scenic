include Makefile.inc

TARGET		=	game

SOCKET_DIR	=	$(SRC_DIR)/socket
SOCKET_SRC	=	$(wildcard $(SOCKET_DIR)/*.cpp)

MAIN_DIR	=	$(SRC_DIR)/game
MAIN_SRC	=	$(wildcard $(MAIN_DIR)/*.cpp)

SRC		+=	$(SOCKET_SRC) $(MAIN_SRC)

$(MAIN_DIR)/%.o: CXXFLAGS += -I$(SOCKET_DIR)

include Makefile.inc2

# Target dependencies must be specified after Makefile.inc2
$(TARGET): CXXFLAGS += -DNAMESPACE=scenic
$(TARGET): LDFLAGS  += -lglut
