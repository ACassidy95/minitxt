#ifndef _STATE_H_
#define _STATE_H_

// Define a type to store the global state
// of the terminal during exection

typedef struct config_t {
	int scrrows;
	int scrcols; 
	struct termios orig_termios;
} config_t;


// Define a type to store dynamic strings which
// allows the terminal window to be updated at once
typedef struct buf {
	char* str;
	int len; 
} buffer_t;

// Global variable to store terminal state
struct config_t TERMINAL;

// Base state for a buffer is an empty string
// with 0 length
#define BUFFER_INIT {NULL, 0}

#endif
