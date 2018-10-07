#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Remembers the state of the terminal
// before this program executes so it can
// be set back to canonical mode
struct termios orig_termios;

// This fucntion will be called on exit to 
// return the terminal to its original state
void disable_raw_mode() 
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() 
{
	// Set orig_termios to current settings
	tcgetattr(STDIN_FILENO, &orig_termios);
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

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(int argc, char const **argv)
{
	enable_raw_mode();

	while(1) {
		char c = '\0';
		read(STDIN_FILENO, &c, 1);
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d('%c')\r\n", c, c);
		}

		if(c == 'q')	break;
	}

	return 0;
}