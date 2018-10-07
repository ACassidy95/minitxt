#ifndef _MINITXT_H_
#define _MINITXT_H_

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Used for detecting ctrl+ key combinations
#define CTRL_K(k) ((k) & 0x1f);

// Remembers the state of the terminal
// before this program executes so it can
// be set back to canonical mode
struct termios orig_termios;

// kill with error - takes in an error
// string and prints it to stdout and exits
void kwerror(char* s);

// disable raw mode - return terminal to
// canonical mode on termination
void drawm();

// enable raw mode - change terminal to
// raw mode from canonical mode
void erawm();

#endif
