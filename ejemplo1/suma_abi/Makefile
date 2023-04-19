CHAIN=arm-none-eabi
CFLAGS=-std=gnu99 -Wall -mcpu=cortex-a8
LDEXTRAS= -lc -L /usr/lib/arm-none-eabi/newlib/

OBJ=obj/
BIN=bin/
INC=inc/
SRC=src/
LST=lst/

ddd_ej_ABI:
	ddd --debugger gdb-multiarch $(OBJ)mi_suma.elf

run_ddd:
	qemu-system-arm -M realview-pb-a8 -m 32M -no-reboot -nographic -monitor telnet:127.0.0.1:1234,server,nowait -kernel $(BIN)mi_suma.bin -S -gdb tcp::2159

run:
	qemu-system-arm -M realview-pb-a8 -m 32M -no-reboot -nographic -monitor telnet:127.0.0.1:1234,server,nowait -kernel $(BIN)mi_suma.bin

symb:
	$(CHAIN)-objcopy --only-keep-debug $(OBJ)mi_suma.elf $(BIN)mi_suma.sym

all: $(BIN)mi_suma.bin $(OBJ)mi_suma.elf

$(BIN)mi_suma.bin: $(OBJ)mi_suma.elf
	@echo ""
	@echo "Generando binarios"
	$(CHAIN)-objcopy -O binary $< $(BIN)mi_suma.bin

$(OBJ)mi_suma.elf: $(OBJ)suma_abi.o $(OBJ)suma_c.o
	@echo ""
	@echo "Linkeando..."
	mkdir -p obj
	mkdir -p lst
	#$(CHAIN)-ld $(OBJ)*.o -o $(OBJ)mi_suma.elf -Map $(LST)mi_suma.map
	$(CHAIN)-ld -T td3_memmap.ld $(LDEXTRAS) $(OBJ)*.o -o $(OBJ)mi_suma.elf -Map $(LST)mi_suma.map
	@echo "Linkeo finalizado!!"
	@echo ""
	@echo "Generando archivos de información: mapa de memoria y símbolos"
	readelf -a $(OBJ)mi_suma.elf > $(LST)mi_suma_elf.txt
	$(CHAIN)-objdump -D $(OBJ)mi_suma.elf > $(LST)mi_suma.lst

$(OBJ)suma_abi.o: $(SRC)suma_abi.s
	@echo ""
	mkdir -p bin
	mkdir -p obj
	mkdir -p lst
	@echo "Ensamblando suma_abi.s..."
	$(CHAIN)-as $(SRC)suma_abi.s -o $(OBJ)suma_abi.o -a > $(LST)suma_abi.lst
	
$(OBJ)suma_c.o: $(SRC)suma_c.c
	@echo ""
	mkdir -p bin
	mkdir -p obj
	mkdir -p lst
	@echo "Compliando suma_c.c"
	$(CHAIN)-gcc -O3 $(CFLAGS) -c $(SRC)suma_c.c -o $(OBJ)suma_c.o

clean:
	rm -rf $(OBJ)*.o
	rm -rf $(OBJ)*.elf
	rm -rf $(BIN)*.bin
	rm -rf $(LST)*.lst
	rm -rf $(LST)*.txt
	rm -rf $(LST)*.map
