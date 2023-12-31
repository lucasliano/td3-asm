#################################################################
#                                                               #
#                 Técnicas Digitales 3 - Makefile               #
#                                                               #
#@author        Lucas Liaño                                     #                                                                      #
#@brief         Makefile para debuggear en C-Linux.				#
#@date          04/09/2023                                      #
#################################################################

# Main folder name
ROOT_NAME = ${notdir ${shell pwd}}
SRV_EXE = lucas_server

# Configs
CHAIN=gcc
# CFLAGS=-c -g -Wall
CFLAGS= -c -Wall -g -march=armv7-a -marm -mfpu=neon-vfpv4
LDFLAGS= -Wall -g -pthread -I${INC}


# Folders
# --- Added to .gitignore ---
OBJ=obj
BIN=bin
LST=lst

# --- Must be pushed ----
INC=inc
SRC=src

# List source files
SRC_FILES_C = ${wildcard ${SRC}/*.c}
OBJ_FILES_C = ${subst ${SRC}.c,${OBJ}.o,${SRC_FILES_C}}


#################################################################
##                           Rules                             ##
#################################################################
default: build

build: $(SRV_EXE)

run:
	@echo "Running binary:"
	@echo "-----------------"
	sudo ./$(BIN)/$(SRV_EXE)

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(LST)

help:
	@echo "make build: Generates .bin"
	@echo "make clean: Deletes folders: bin/ lst/ obj/"

#################################################################
##                         Auto.  Rules                        ##
#################################################################
# .o 	<- .c

# C sources compiling
$(SRV_EXE): $(OBJ_FILES_C)
	@echo ""
	@mkdir -p bin
	@echo "Linking..."
	$(CHAIN) -o $(BIN)/$@ $^ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c
	@echo ""
	@mkdir -p obj
	@echo "Compiling $<..."
	$(C) $(CFLAGS) $< -o $@