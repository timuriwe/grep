FLAGS = -std=c11 -Wall -Wextra -Werror -D_GNU_SOURCE

all: my_grep

clean: 
	rm -f my_grep my_grep.o

my_grep: my_grep.o
	gcc $(FLAGS) my_grep.o -o my_grep

my_grep.o:
	gcc $(FLAGS) -c my_grep.c -o my_grep.o
