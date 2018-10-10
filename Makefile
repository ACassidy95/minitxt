minitxt: minitxt.c mode.c io.c
	$(CC) minitxt.c mode.c io.c  -o minitxt -Wall -Wextra -pedantic -std=c99