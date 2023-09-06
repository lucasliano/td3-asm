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

# Configs
CHAIN=gcc
CFLAGS=-g -Wall -I${INC}

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
OBJ_FILES_C = ${subst ${SRC},${OBJ},${SRC_FILES_C:.c=.o}}

#################################################################
##                           Rules                             ##
#################################################################
default: build

build: ${OBJ_FILES_C}

debug:
	gdb-multiarch -iex "add-auto-load-safe-path /" ${OBJ}/${ROOT_NAME}.o

run:
	@echo "-----------------"
	@echo "Running binary:"
	./${OBJ}/${ROOT_NAME}.o

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(LST)
	rm -f .gdb_history

help:
	@echo "make build: Generates .bin"
	@echo "make clean: Deletes folders: bin/ lst/ obj/"
	@echo "make debug: Runs Cortex-A8 emulator and gdb-multiarch"

#################################################################
##                         Auto.  Rules                        ##
#################################################################
# .o 	<- .c

# C sources compiling
$(OBJ)/%.o: $(SRC)/%.c
	@echo ""
	@mkdir -p obj
	@echo "Compiling .c sources..."
	$(CHAIN) ${CFLAGS} $< -o $@ 


# To take into account .h files
-include ${OBJ}/*.d