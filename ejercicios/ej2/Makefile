#################################################################
#                                                               #
#                 Técnicas Digitales 3 - Makefile               #
#                                                               #
#@author        Lucas Liaño                                     #                                                                      #
#@brief         Makefile para TD3.                              #
#@date          23/04/2023                                      #
#@credits       Schiavoni Leandro                               #
#################################################################

# Main folder name
ROOT_NAME = ${notdir ${shell pwd}}

# Configs
CHAIN=arm-none-eabi
CFLAGS=-std=gnu99 -Wall -mcpu=cortex-a8 -MD -I${INC}
LDEXTRAS= -lc -L /usr/lib/arm-none-eabi/newlib/

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
OBJ_FILES_C = ${subst ${SRC},${OBJ},${SRC_FILES_C:.c=_c.o}}

SRC_FILES_S = ${wildcard ${SRC}/*.s}
OBJ_FILES_S = ${subst ${SRC},${OBJ},${SRC_FILES_S:.s=_s.o}}

#################################################################
##                           Rules                             ##
#################################################################
default: build

build: $(BIN)/${ROOT_NAME}.bin $(OBJ)/${ROOT_NAME}.elf

qemu: $(BINS) $(ELFS)
	qemu-system-arm -M realview-pb-a8 -m 32M -no-reboot -nographic -monitor telnet:127.0.0.1:1234,server,nowait -kernel ${BIN}/${ROOT_NAME}.bin -S -gdb tcp::2159

ddd:
	cd .. && ddd --debugger gdb-multiarch ${ROOT_NAME}/${OBJ}/${ROOT_NAME}.elf

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(LST)

help:
	@echo "make build: Generates .bin"
	@echo "make clean: Deletes folders: bin/ lst/ obj/"
	@echo "make qemu:  Runs Cortex-A8 emulator"
	@echo "make ddd:   Runs the debugger"

#################################################################
##                         Auto.  Rules                        ##
#################################################################
# .bin 	<- .elf
# .elf 	<- .o
# .o 	<- .c .s

# Generating .bin file from .elf
$(BIN)/${ROOT_NAME}.bin: $(OBJ)/${ROOT_NAME}.elf
	$(CHAIN)-objcopy -O binary $< $@

# Generating .elf file from *.o
$(OBJ)/${ROOT_NAME}.elf: ${OBJ_FILES_S} ${OBJ_FILES_C}
	@echo "Linkeando..."
	@mkdir -p obj
	@mkdir -p lst
	$(CHAIN)-ld -T memmap.ld ${OBJ_FILES_S} ${OBJ_FILES_C} -o $@ -Map $(LST)/$(ROOT_NAME).map
	@echo "Linkeo finalizado"
	@echo ""
	@echo "Generating memory map and symbol information files.."
	readelf -a $@ > $(basename $@).txt
	$(CHAIN)-objdump -D $@ > $(basename $@).lst

# ASM sources assembly
$(OBJ)/%_s.o: $(SRC)/%.s
	@echo ""
	@mkdir -p bin
	@mkdir -p obj
	@mkdir -p lst
	@echo "Assembling $<"
	@echo "Assembling $@"
	$(CHAIN)-as $< -o $@ -a > $(basename $@).lst

# C sources compiling
$(OBJ)/%_c.o: $(SRC)/%.c
	@echo ""
	@mkdir -p bin
	@mkdir -p obj
	@mkdir -p lst
	@echo "Compiling .c sources..."
	$(CHAIN)-gcc ${CFLAGS} -c $< -o $@


# To take into account .h files
-include ${OBJ}/*.d