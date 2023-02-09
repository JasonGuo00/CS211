all: tenth
tenth: tenth.c
	gcc -g -Wall -Werror -fsanitize=address -std=c11 tenth.c -o tenth -lm
clean:
	rm -rf tenth