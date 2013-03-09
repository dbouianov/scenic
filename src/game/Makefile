include Makefile.inc

TARGET		=	game

SOCKET_DIR	=	$(SRC_DIR)/socket
SOCKET_SRC	=	$(SOCKET_DIR)/socket.cpp	\
			$(SOCKET_DIR)/sockettcp.cpp	\
			$(SOCKET_DIR)/socketudp.cpp

MAIN_DIR	=	$(SRC_DIR)/game
MAIN_SRC	=	$(MAIN_DIR)/game.cpp		\
			$(MAIN_DIR)/gameserver.cpp	\
			$(MAIN_DIR)/gameclient.cpp	\
			$(MAIN_DIR)/mymenu.cpp		\
			$(MAIN_DIR)/mymenuitem.cpp

SRC		+=	$(SOCKET_SRC) $(MAIN_SRC)

$(MAIN_DIR)/%.o: CXXFLAGS += -I$(SOCKET_DIR)

include Makefile.inc2

# Target dependencies must be specified after Makefile.inc2
$(TARGET): CXXFLAGS += -DNAMESPACE=scenic
$(TARGET): LDFLAGS  += -lglut
