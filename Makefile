io: ./io/output.c ./io/input.c ./io/textops.c ./io/file.c
	$(CC) -c ./io/output.c ./io/input.c ./io/textops.c ./io/file.c

mode: ./mode/minitxt.c ./mode/mode.c
	$(CC) -c ./mode/minitxt.c ./mode/mode.c

minitxt: 
	$(CC) mode io  -o minitxt -Wall -Wextra -pedantic -std=c99