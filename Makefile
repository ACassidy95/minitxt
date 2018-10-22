minitxt: io mode
	$(CC) *.o -o minitxt -Wall -Wextra -pedantic -std=c99

io: ./io/output.c ./io/input.c ./io/textops.c ./io/file.c
	$(CC) -c ./io/output.c ./io/input.c ./io/textops.c ./io/file.c

mode: ./mode/minitxt.c ./mode/mode.c
	$(CC) -c ./mode/minitxt.c ./mode/mode.c

clean:
	rm *.o minitxt