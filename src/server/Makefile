include Makefile.inc

TARGET		=	server

THRIFT_INC_DIR	=	/usr/local/include/thrift
SCHEMA_DIR	=	$(SRC_DIR)/thrift

MAIN_DIR	=	$(SRC_DIR)/server
MAIN_SRC	=	$(MAIN_DIR)/server.cpp

GEN_DIR		=	$(MAIN_DIR)/gen-cpp
GEN_SRC		=	$(wildcard $(GEN_DIR)/*.cpp)

SRC		+=	$(GEN_SRC) $(MAIN_SRC)

$(MAIN_DIR)/%.o: CXXFLAGS += -I$(THRIFT_INC_DIR) -I$(GEN_DIR) \
	-Wno-strict-aliasing -Wno-unused-parameter -DHAVE_NETINET_IN_H

$(GEN_SRC): $(GEN_DIR)
$(GEN_DIR):
	thrift --gen cpp -o $(MAIN_DIR) $(SCHEMA_DIR)/Scenic.thrift

.PHONY: clean_gen
clean: clean_gen
clean_gen:
	rm -rf $(GEN_DIR)

include Makefile.inc2

# Target dependencies must be specified after Makefile.inc2
$(TARGET): CXXFLAGS += -DNAMESPACE=scenic -DMAJOR_VERSION=0 -DMINOR_VERSION=1
$(TARGET): LDFLAGS  += -lthrift -lthriftnb -levent -lpng12 -ljpeg -ltiffxx -lboost_program_options
