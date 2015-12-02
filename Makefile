COMPILER=gcc
FLAGS=-Wall -Wundef -Wcast-align -Wpointer-arith -Wstrict-overflow=5 -Winit-self -Waggregate-return

tm: turing.c map.c list.c str.c
	$(COMPILER) $(FLAGS) turing.c map.c list.c str.c -o tm

parser: parser.c map.c list.c str.c
	$(COMPILER) $(FLAGS) parser.c program.c map.c list.c str.c -o parser

interpreter: interpreter.c machine.c map.c list.c str.c
	$(COMPILER) $(FLAGS) interpreter.c machine.c map.c list.c str.c -o interpreter

tests: tests_list tests_map

tests_list: tests_list.c list.c
	$(COMPILER) $(FLAGS) tests_list tests_list.c list.c

tests_map: tests_map.c map.c list.c
	$(COMPILER) $(FLAGS) tests_map tests_map.c map.c list.c
