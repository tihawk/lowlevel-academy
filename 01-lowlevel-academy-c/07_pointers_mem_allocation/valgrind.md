# Valgrind

Used for automatic checking for memory leaks.

It wraps around the heap, and keeps track of allocation and freeing of memory,
and can hence provide debug information upon losing track of a given chunk
of memory in-code.

## Usage

1. Compile with `-g` tag:

```bash
gcc -o <file-output> <file-input.c> -g
```

This provides debug symbols in the code.

2. Install Valgrind:

```bash
sudo apt install valgrind
```

3. Run Valgrind:

```bash
valgrind --leak-check=full <file-executable> -s
```

## Notes

For branching code, we need to somehow provide full logic paths for Valgrind,
so it can check for leaks in code which isn't run always or by default.
