#ifndef _MINITXT_H_
#define _MINITXT_H_

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Used for detecting ctrl+ key combinations
#define CTRL_K(k) ((k) & 0x1f)

void init();

/* Terminal mode functions */
// kill with error - takes in an error
// string and prints it to stdout and exits
void kwerror(const char* s);

// disable raw mode - return terminal to
// canonical mode on termination
void drawm();

// enable raw mode - change terminal to
// raw mode from canonical mode
void erawm();

/* Input functions */
// read key & process key - read a key 
// press and decide how to respond
char rkey();
void pkey();

/* Output functions */
// refreshes the screen
void rfscrn();

// draw a vim-like column of tildes on the screen
void drscrn();

// get window information
int gwsize(int* r, int* c);
int cursorpos(int* r, int* c);

#endif
