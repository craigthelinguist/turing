all: turing.c
	gcc -o tm turing.c map.c list.c -l ncurses
