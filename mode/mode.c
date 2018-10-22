#include "../include/define.h"
#include "../include/funcs.h"

void init()
{
	TMNL.x_pos = 0;
	TMNL.y_pos = 0;
	TMNL.render_x = 0;
	TMNL.rwoffset = 0;
	TMNL.coloffset = 0; 
	TMNL.ctrows = 0; 
	TMNL.row = NULL;
	if(gwsize(&TMNL.scrrows, &TMNL.scrcols) == -1) {
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
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &TMNL.orig_termios) == -1) {
		kwerror("tcsetattr");
	}
}

void erawm() 
{
	// Set orig_termios to current settings
	if(tcgetattr(STDIN_FILENO, &TMNL.orig_termios) == -1) {
		kwerror("tcgetattr");
	}
	atexit(drawm);
 
	struct termios raw = TMNL.orig_termios;

	// Disable the following input flags
	//	ICRNL	- disable auto conversion of \n to \r\n in input
	//	IXON	- disable software control flow
	//	BRKINT	- disable SIGINTS on break conditions 
	//	INPCK	- disable parity checking
	//	
	// Disable the following output flags
	//	OPOST	- disable auto conversion of \n to \r\n in output
	//
	// Disable the following local flags
	//	ECHO 	- stop printing input
	//	ICANON 	- enter raw mode
	//	ISIG	- stop sending signals, e.g. SIGINT, SIGSTP
	//	IEXTEN	- diable ctrl-v
	//
	// and save this as the terminal's current state
	// When all these changes are made, the terminal will
	// be in raw mode
	raw.c_iflag 		&= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag		&= ~(OPOST);
	raw.c_cflag 		|= (CS8); // set char size to 8 if not already
	raw.c_lflag 		&= ~(ECHO | ICANON | ISIG | IEXTEN);

	// Set read timeout
	raw.c_cc[VMIN] 		= 0;
	raw.c_cc[VTIME] 	= 100;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		kwerror("tcsetattr");
	}
}

