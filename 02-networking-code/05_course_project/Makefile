TARGET_SERVER = bin/dbserver
TARGET_CLIENT = bin/dbclient

SRC_SERVER = $(wildcard src/server/*.c)
OBJ_SERVER = $(patsubst src/server/%.c, obj/server/%.o, $(SRC_SERVER))

SRC_CLIENT = $(wildcard src/client/*.c)
OBJ_CLIENT = $(patsubst src/client/%.c, obj/client/%.o, $(SRC_CLIENT))

run: clean default
	./$(TARGET_SERVER) -n
	./$(TARGET_SERVER) -n -p 8080
	./$(TARGET_SERVER) -n -p
	./$(TARGET_SERVER) -n -f ./noport.db
	./$(TARGET_SERVER) -n -f ./existent.db -p 5555
#	./$(TARGET) -n
#	./$(TARGET) -f ./nonexistent.db
#	./$(TARGET) -nf ./existent.db
#	./$(TARGET) -f ./existent.db -a "K. Blagoev,Mariannenstr. 105,10"
#	./$(TARGET) -f ./existent.db -a "A. Randomm,Not a real st. 12,2400"
#	./$(TARGET) -f ./existent.db -l
	rm -f existent.db

default: $(TARGET_SERVER) $(TARGET_CLIENT)

clean:
	rm -f obj/server/*.o
	rm -f obj/client/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SERVER): $(OBJ_SERVER)
	gcc -o $@ $?

$(OBJ_SERVER): obj/server/%.o: src/server/%.c
	gcc -c $< -o $@ -Iinclude

$(TARGET_CLIENT): $(OBJ_CLIENT)
	gcc -o $@ $?

$(OBJ_CLIENT): obj/client/%.o: src/client/%.c
	gcc -c $< -o $@ -Iinclude
