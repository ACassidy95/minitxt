minitxt: ./src/minitxt.c ./src/mode.c ./src/io.c
	$(CC) ./src/minitxt.c ./src/mode.c ./src/io.c  -o minitxt -Wall -Wextra -pedantic -std=c99