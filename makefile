run: assembler.o macro.o first_pass.o func_first_pass.o second_pass.o help_function.o
	gcc -ansi -pedantic -Wall assembler.o macro.o first_pass.o func_first_pass.o second_pass.o help_function.o -o run

assembler.o: assembler.c header_macro.h header_first_pass.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

macro.o: macro.c header_macro.h
	gcc -c -ansi -pedantic -Wall macro.c -o macro.o

first_pass.o: first_pass.c header_first_pass.h
	gcc -c -ansi -pedantic -Wall first_pass.c -o first_pass.o

func_first_pass.o: func_first_pass.c header_func_first_pass.h
	gcc -c -ansi -pedantic -Wall func_first_pass.c -o func_first_pass.o

second_pass.o: second_pass.c header_second_pass.h
	gcc -c -ansi -pedantic -Wall second_pass.c -o second_pass.o

help_function.o: help_function.c header_help_func.h
	gcc -c -ansi -pedantic -Wall help_function.c -o help_function.o



