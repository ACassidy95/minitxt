#ifndef _MINITXT_H_
#define _MINITXT_H_

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Used for detecting ctrl+ key combinations
#define CTRL_K(k) ((k) & 0x1f)

typedef struct config_t {
	int scrrows;
	int scrcols; 
	struct termios orig_termios;
} config_t;


// Define a type to store dynamic strings which
// allows the terminal window to be updated at once
typedef struct buffer_t {
	char* str;
	int len; 
} buffer_t;

// Global variable to store terminal state
struct config_t TERMINAL;

// Base state for a buffer is an empty string
// with 0 length
#define BUFFER_INIT {NULL, 0}

void init();

/* ============================================
	    Terminal mode functions 
   ===========================================*/

// kill with error - takes in an error
// string and prints it to stdout and exits
void kwerror(const char* s);

// disable raw mode - return terminal to
// canonical mode on termination
void drawm();

// enable raw mode - change terminal to
// raw mode from canonical mode
void erawm();

/* ============================================
	    	Input functions 
   ===========================================*/

// read key & process key - read a key 
// press and decide how to respond
char rkey();
void pkey();

/* ============================================
	    	Output functions 
   ===========================================*/

// refreshes the screen
void rfscrn();

// draw a vim-like column of tildes on the screen
void drscrn(buffer_t* buf);

// get window information
int gwsize(int* r, int* c);
int cursorpos(int* r, int* c);

/* ============================================
	    	Buffer functions 
   ===========================================*/

// adds string data of length l to an existing buffer
void appendbuf(buffer_t* buf, const char* s, int l);

// frees a buffer
void freebuf(buffer_t* buf); 

#endif
