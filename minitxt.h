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
#define CTRL_K(k) 	((k) & 0x1f)

#define VER 		"MiniTxt Version 0.0.1"

#define CURSOR_OFF 	"\x1b[?25l"
#define CURSOR_ON 	"\x1b[?25h"
#define CURSOR_POS	"\x1b[6n"
#define MV_CURSOR_00	"\x1b[H"
#define MV_CURSOR_XY	"\x1b[%d;%dH"
#define MV_CURSOR_MAX	"\x1b[999C\x1b[999B"
#define CLEAR_SCREEN	"\x1b[2J"
#define CLEAR_LINE	"\x1b[K"


// Defines the current state of the terminal
// and stores the original state to revert to
// when execution finishes
typedef struct config_t {
	int x_pos;
	int y_pos;
	int scrrows;
	int scrcols; 
	struct termios orig_termios;
} config_t;

typedef struct buffer_t {
	char* str;
	int len; 
} buffer_t;

struct config_t TERMINAL;

// Base state for a buffer is an empty string
// with 0 length
#define BUFFER_INIT 	{NULL, 0}

// Key mappings
enum keys {
	UARROW = 1000,
	DARROW,
	LARROW,
	RARROW,
	PGUP,
	PGDN,
	HOME,
	END,
	DEL
};

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
int rkey();
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
void mvcursor(int c);

/* ============================================
	    	Buffer functions 
   ===========================================*/

// adds string data of length l to an existing buffer
void appendbuf(buffer_t* buf, const char* s, int l);

// frees a buffer
void freebuf(buffer_t* buf); 

#endif
