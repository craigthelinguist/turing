COMPILER=gcc
FLAGS=-Wall -Wundef -Wcast-align -Wpointer-arith -Wstrict-overflow=5 -Winit-self -Waggregate-return -o

tm: turing.c list.c map.c
	$(COMPILER) $(FLAGS) -o tm turing.c map.c list.c -l ncurses

tests: tests_list tests_map

tests_list: tests_list.c list.c
	$(COMPILER) $(FLAGS) tests_list tests_list.c list.c

tests_map: tests_map.c map.c list.c
	$(COMPILER) $(FLAGS) tests_map tests_map.c map.c list.c
