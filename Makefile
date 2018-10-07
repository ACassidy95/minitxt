minitxt: ./src/minitxt.c ./src/mode.c
	$(CC) ./src/minitxt.c ./src/mode.c -o minitxt -Wall -Wextra -pedantic -std=c99