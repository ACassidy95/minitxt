#include "../include/minitxt.h"
#include "../include/state.h"

// Waits for a single keypress and returns it
char rkey()
{
	int r;
	char c; 

	while((r = read(STDIN_FILENO, &c, 1)) != 1) {
		if(r == -1 && errno != EAGAIN) {
			kwerror("read");
		}
	}

	return c;
}

// Decides how to process based on the value
// recieved from rkey
void pkey()
{
	char c = rkey();

	switch(c) {
		case CTRL_K('q'):
			rfscrn();
			exit(0);
			break;
	}
}

// Refresh screen with escape sequence '\x1b[2J'
// and position the cursor at top left with 
// sequence '\x1b[H'
void rfscrn()
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}

// Print a column of tildes and reposition cursor
// back at the top of the screen
void drscrn()
{
	for(int i = 0; i < TERMINAL.scrrows; ++i) {
		write(STDOUT_FILENO, "~", 1);

		if(i < TERMINAL.scrrows - 1) {
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}

	write(STDOUT_FILENO, "\x1b[H", 3);
}

// This function gets the position of the cursor 
// using escape sequence '\x1b[6n in the form of
// an escape sequence, e.g. '\x1b[24;80R' which
// is parsed using sscanf
int cursorpos(int* r, int* c)
{
	char* buf = malloc(32 * sizeof(char));
  	unsigned int i = 0;

  	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
		return -1;
  	}

  	while (i < sizeof(buf)) {
    		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    		if (buf[i] == 'R') break;
    		i++;
  	}

  	buf[i] = '\0';
  	
  	if(buf[0] != '\x1b' || buf[1] != '[') {
  		return -1;
  	}

  	if(sscanf(&buf[2], "%d;%d", r, c) != 2) {
  		return -1;
  	}

  	return 0;
}

// Attempts to get window size using definitions in
// ioctl.h. Failing this, the size of the window is got
// by forcing the cursor to the bottom right corner of the 
// screen and reading its position.
int gwsize(int* r, int* c) 
{
	struct winsize w; 

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
		if(write(STDOUT_FILENO, "\x1b[999B\x1b[999C", 12) != 12) {
			return -1;
		}
		return cursorpos(&r, &c);
	} else {
		*r = w.ws_row;
		*c = w.ws_col;
		return 0;
	}
}
