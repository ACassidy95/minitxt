#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Remembers the state of the terminal
// before this program executes so it can
// be set back to canonical mode
struct termios orig_termios;

// Error handling mode
void die(char* s)
{
	perror(s);
	exit(1);
}

// This fucntion will be called on exit to 
// return the terminal to its original state
void disable_raw_mode() 
{
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enable_raw_mode() 
{
	// Set orig_termios to current settings
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
		die("tcgetattr");
	atexit(disable_raw_mode);
 
	struct termios raw = orig_termios;

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
	// and save this as the terminal's 
	// current state
	raw.c_iflag 		&= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag		&= ~(OPOST);
	raw.c_cflag 		|= (CS8); // set char size to 8 if not already
	raw.c_lflag 		&= ~(ECHO | ICANON | ISIG | IEXTEN);

	// Set read timeout
	raw.c_cc[VMIN] 		= 0;
	raw.c_cc[VTIME] 	= 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

int main(int argc, char const **argv)
{
	enable_raw_mode();

	while(1) {
		char c = '\0';
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("read");
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d('%c')\r\n", c, c);
		}

		if(c == 'q')	break;
	}

	return 0;
}