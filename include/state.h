#ifndef _STATE_H_
#define _STATE_H_

// Define a type to store the global state
// of the terminal during exection

typedef struct config_t {
	int scrrows;
	int scrcols; 
	struct termios orig_termios;
} config_t;

#endif
