all: turing.c
	gcc -o tm turing.c list.c -l ncurses
