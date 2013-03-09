include Makefile.inc

TARGET		=	render

MAIN_DIR	=	$(SRC_DIR)/render
MAIN_SRC	= 	$(wildcard $(MAIN_DIR)/*.cpp)

SRC		+=	$(MAIN_SRC)

include Makefile.inc2

# Target dependencies must be specified after Makefile.inc2
$(TARGET): CXXFLAGS += -DNAMESPACE=scenic
$(TARGET): LDFLAGS  += -lpng12 -ljpeg
