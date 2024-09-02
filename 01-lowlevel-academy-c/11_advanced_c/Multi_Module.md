## Manually compiling and linking

```bash
gcc -o file.o -I$(pwd)/include src/file.c -c
gcc -o parse.o -I$(pwd)/include src/parse.c -c
gcc -o main.o -I$(pwd)/include src/main.c -c
```

```bash
gcc *.o -o bin/newout
```
## Makefile

In our last lesson I showed you how to manually compile multi module code by 
hand with GCC. This is painful and unnecesary. In this lesson we'll use a tool
called make to write a Makefile and automate the process for us.

### Make
make is a tool that, when ran, finds a local Makefile and executes the 
instructions to build a particular target.

### Syntax
Makefiles are organized around targets, rules, and instructions

```Makefile
TARGET = bin/final
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)
```
Here, default is a target, that says to be complete, first the `$(TARGET)`
binary must be complete. Once that is complete, the instructions within 
default will be ran.

Make will take this logic forward and recursively run the functionality in 
the Makefile until the target is met.

### Example

```MakeFile
TARGET = bin/final
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
```

### Wildcard

Wildcard will capture all of our source files as a list in `src/`

### Patsubst

Patsubst will execute a pattern substitution and convert all `src/\*.c` to `obj/\*.o`

### Symbols

$? represents a list of all input files given as a target

$< represents a single file given as a target

$@ represents the target of the rule

### Running

```bash
make clean
```

```bash
make [default]
```
