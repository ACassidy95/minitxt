#include "../include/minitxt.h"
#include "../include/state.h"

void init()
{
	if(gwsize(&TERMINAL.scrrows, &TERMINAL.scrcols) == -1) {
		kwerror("Init");
	}
}

// Error handling mode
void kwerror(const char* s)
{
	rfscrn();
	perror(s);
	exit(1);
}

// This fucntion will be called on exit to 
// return the terminal to its original state
void drawm() 
{
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &TERMINAL.orig_termios) == -1) {
		kwerror("tcsetattr");
	}
}

void erawm() 
{
	// Set orig_termios to current settings
	if(tcgetattr(STDIN_FILENO, &TERMINAL.orig_termios) == -1) {
		kwerror("tcgetattr");
	}
	atexit(drawm);
 
	struct termios raw = TERMINAL.orig_termios;

	// Disable the following input flags
	//	ICRNL	- disable auto conversion of \n to \r\n in input
	//	IXON	- disable software control flow
	//	BRKINT	- disable SIGINTS on break conditions 
	//	INPCK	- disable parity checking
	//	
	// Disable the following output flags
	//	OPOST	- disable auto conversion of \n to \r\n in output
	// Disable the following local flags
	//	ECHO 	- stop printing input
	//	ICANON 	- enter raw mode
	//	ISIG	- stop sending signals, e.g. SIGINT, SIGSTP
	//	IEXTEN	- diable ctrl-v
	// and save this as the terminal's current state
	// When all these changes are made, the terminal will
	// be in raw mode
	raw.c_iflag 		&= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag		&= ~(OPOST);
	raw.c_cflag 		|= (CS8); // set char size to 8 if not already
	raw.c_lflag 		&= ~(ECHO | ICANON | ISIG | IEXTEN);

	// Set read timeout
	raw.c_cc[VMIN] 		= 0;
	raw.c_cc[VTIME] 	= 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		kwerror("tcsetattr");
	}
}

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

// Refresh screen and move cursor to top left
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
		write(STDOUT_FILENO, "~\r\n", 3);
	}

	write(STDOUT_FILENO, "\x1b[H", 3);
}

int gwsize(int* r, int* c) 
{
	struct winsize w; 

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
		return -1;
	} else {
		*r = w.ws_row;
		*c = w.ws_col;
		return 0;
	}
}
