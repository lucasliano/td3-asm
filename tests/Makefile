#################################################################
#                                                               #
#                 Técnicas Digitales 3 - Makefile               #
#                                                               #
#@author        Lucas Liaño                                     #                                                                      #
#@brief         Makefile para debuggear en C-Linux.				#
#@date          06/08/2023                                      #
#################################################################

# Main folder name
ROOT_NAME = ${notdir ${shell pwd}}

# Configs
CHAIN=gcc
CFLAGS=-g -Wall -I${INC}

# Folders
# --- Added to .gitignore ---
OBJ=obj

# --- Must be pushed ----
INC=inc
SRC=src

# List source files
SRC_FILES_C = ${wildcard ${SRC}/*.c}
OBJ_FILES_C = ${subst ${SRC},${OBJ},${SRC_FILES_C:.c=_o}}

#################################################################
##                           Rules                             ##
#################################################################
default: clean build

build: ${OBJ_FILES_C}

debug:
	@echo "-----------------"
	@echo "Running binary:"
	./obj/test_o

clean:
	rm -rf $(OBJ)

help:
	@echo "make build: Generates .bin"
	@echo "make clean: Deletes folders: bin/ lst/ obj/"
	@echo "make debug: Runs Cortex-A8 emulator and gdb-multiarch"

#################################################################
##                         Auto.  Rules                        ##
#################################################################
# .o 	<- .c

# C sources compiling
$(OBJ)/%_o: $(SRC)/%.c
	@echo ""
	@mkdir -p obj
	@echo "Compiling .c sources..."
	$(CHAIN) ${CFLAGS} $< -o $@


# To take into account .h files
-include ${OBJ}/*.d