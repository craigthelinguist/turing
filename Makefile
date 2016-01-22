\CC=gcc
DIRS=-Idatastructs -Icore -Itests -Iview
FLAGS=-Wall -Wundef -Wcast-align -Wpointer-arith -Wstrict-overflow=5 -Winit-self $(DIRS)
VPATH=datastructs:core:view:tests

sim: sim.c parser.c interpreter.c program.c machine.c parser.c map.c list.c str.c
	$(CC) $(FLAGS) $^ -o $@ -l ncurses

parser: parser.c program.c map.c list.c str.c
	$(CC) $(FLAGS) $^ -o $@

interpreter: interpreter.c program.c machine.c map.c list.c str.c
	$(CC) $(FLAGS) $^ -o $@

tests: tests_list tests_map

tests_list: tests_list.c list.c
	$(CC) $(FLAGS) $^ -o $@

tests_map: tests_map.c map.c list.c
	$(CC) $(FLAGS) $^ -o $@
