.PHONY: valgrind

valgrind: pokedex
	valgrind  -s --leak-check=full ./pokedex
	
.PHONY: clean

clean:
	rm -f pokedex
	cp docs/*.txt .

#compile
pokedex: pokedex.c main.c
	gcc *.c -Wall -Werror -Wconversion -std=c99 -g -o pokedex

gdb: pokedex
	gdb ./pokedex

