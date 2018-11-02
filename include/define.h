#ifndef _DEFINE_H_
#define _DEFINE_H_

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

// Used for detecting ctrl+ key combinations
#define CTRL_K(k) 	((k) & 0x1f)

#define WELCOME 	"Welcome to MiniTxt - Press Ctrl-H for help"

#define TAB_STOP	8

#define CURSOR_OFF 	"\x1b[?25l"
#define CURSOR_ON 	"\x1b[?25h"
#define CURSOR_POS	"\x1b[6n"
#define MV_CURSOR_00	"\x1b[H"
#define MV_CURSOR_XY	"\x1b[%d;%dH"
#define MV_CURSOR_MAX	"\x1b[999C\x1b[999B"
#define CLEAR_SCREEN	"\x1b[2J"
#define CLEAR_LINE	"\x1b[K"
#define STYLE_STATUS	"\x1b[1;7m"
#define CLEAR_STYLE	"\x1b[m"


// Defines a row in the editor
typedef struct edrow_t {
	int len;
	int render;
	char* rendernp; // Used for printing wide invisible characters like tab, or Ctrl+_
	char* chars;
} edrow_t;

// Defines the current state of the terminal
// and stores the original state to revert to
// when execution finishes
typedef struct config_t {
	int x_pos;
	int y_pos;
	int render_x;
	int rwoffset;
	int coloffset; 
	int scrrows;
	int scrcols;
	int ctrows;
	edrow_t* row;
	char* filen; 
	char stmsg[80]; // Stats message - fixed size
	time_t sttime; 
	struct termios orig_termios;
} config_t;

typedef struct buffer_t {
	char* str;
	int len; 
} buffer_t;

struct config_t TMNL;

// Base state for a buffer is an empty string
// with 0 length
#define BUFFER_INIT 	{NULL, 0}

// Key mappings
enum keys {
	BSPACE = 127,
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

#endif
