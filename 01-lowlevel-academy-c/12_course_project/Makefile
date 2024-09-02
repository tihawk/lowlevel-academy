TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET)
	./$(TARGET) -n
#	./$(TARGET) -f ./nonexistent.db
	./$(TARGET) -nf ./existent.db
	./$(TARGET) -f ./existent.db -a "K. Blagoev,Mariannenstr. 105,10"
	./$(TARGET) -f ./existent.db -a "A. Randomm,Not a real st. 12,2400"
	./$(TARGET) -f ./existent.db -l
	rm -f existent.db

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
