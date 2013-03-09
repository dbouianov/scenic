THRIFT_DIR	=	/usr/share/php/Thrift/packages
SCHEMA_DIR	=	$(SRC_DIR)/thrift

MAIN_DIR	=	$(SRC_DIR)/client
MAIN_SRC	=	$(MAIN_SRC)/client.php

GEN_DIR		=	$(MAIN_DIR)/gen-php

.PHONY: client install clean clean_gen

client: $(GEN_DIR)
$(GEN_DIR):
	thrift --gen php -o $(MAIN_DIR) $(SCHEMA_DIR)/Scenic.thrift

install:
	sudo cp -R $(GEN_DIR)/Scenic $(THRIFT_DIR)

clean: clean_gen
clean_gen:
	rm -rf $(GEN_DIR)
