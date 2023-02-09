all: first
first: first.c
	gcc -g -Wall -Werror -fsanitize=address -std=c11 -lm first.c -o first
clean:
	rm -rf first