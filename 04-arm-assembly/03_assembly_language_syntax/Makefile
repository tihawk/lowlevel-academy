SRC = first.s
BIN = first.bin
GCC = arm-linux-gnueabihf-gcc
GCC_FLAGS = -nostdlib -static
QEMU = qemu-arm -g 4242

run: clean default
	$(QEMU) ./$(BIN)

default: $(BIN)

clean:
	rm -f $(BIN)

$(BIN): $(SRC)
	$(GCC) -o $@ $? $(GCC_FLAGS)
