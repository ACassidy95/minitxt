#include "minitxt.h"

// Waits for a single keypress and returns it
int rkey()
{
	int r;
	char c; 

	while((r = read(STDIN_FILENO, &c, 1)) != 1) {
		if(r == -1 && errno != EAGAIN) {
			kwerror("read");
		}
	}

	if(c == '\x1b') {
		char seq[3];

		// Must be sequential - read one char after another
		if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
		if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

		if(seq[0] == '[') {
			switch(seq[1]) {
				case 'A' : return UARROW;
				case 'B' : return DARROW;
				case 'C' : return RARROW; 
				case 'D' : return LARROW;
			}
		}

		return '\x1b';
	} else {
		return c;
	}
}

// Decides how to process based on the value
// recieved from rkey
void pkey()
{
	int c = rkey();

	switch(c) {
		case CTRL_K('q'):
			write(STDOUT_FILENO, CLEAR_SCREEN, 4);
			write(STDOUT_FILENO, MV_CURSOR_00, 3);
			exit(0);
			break;
		case UARROW:
		case DARROW:
		case LARROW:
		case RARROW:
			mvcursor(c);
			break;
	}
}

// Refresh screen with escape sequence '\x1b[2J'
// and position the cursor at top left with 
// sequence '\x1b[H'
//
// \x1b[?25l and \x1b[?25h are used to briefly
// hide the cursor and reshow it. Used to prevent
// flicker effect in screen repainting
void rfscrn()
{
	buffer_t buf = BUFFER_INIT; 

	appendbuf(&buf, CURSOR_OFF, 6);
	appendbuf(&buf, MV_CURSOR_00, 3);

	drscrn(&buf);

	char c[32];
	snprintf(c, sizeof(c), MV_CURSOR_XY, TERMINAL.y_pos + 1, TERMINAL.x_pos + 1);
	appendbuf(&buf, c, strlen(c));

	appendbuf(&buf, CURSOR_ON, 6);

	write(STDOUT_FILENO, buf.str, buf.len);
	freebuf(&buf);
}

// Print a column of tildes and welcome messgae
// reposition cursor back at the top of the screen
void drscrn(buffer_t* buf)
{
  	for (int i = 0; i < TERMINAL.scrrows; i++) {
    		if(i == TERMINAL.scrrows / 10) {
    			char* msg = calloc(64, sizeof(char));
    			int len = snprintf(msg, 64 * sizeof(char), "Welcome to %s", VER);

    			if(len > TERMINAL.scrcols) {
    				len = TERMINAL.scrcols;
    			}

    			int padding = (TERMINAL.scrcols - len) >> 1;

    			if(padding) {
    				appendbuf(buf, "~", 1); 
    				--padding;
    			}

    			while(--padding) {
    				appendbuf(buf, " ", 1);
    			}

    			appendbuf(buf, msg, len);
    		} else {
    			appendbuf(buf, "~", 1);
    		}

    		appendbuf(buf, CLEAR_LINE, 3);

    		if(i < TERMINAL.scrrows - 1) {
    			appendbuf(buf, "\r\n", 2);
    		}
  	}
}

void mvcursor(int c)
{
	switch(c) {
		case UARROW:
			--TERMINAL.y_pos;
			break;
		case DARROW:
			++TERMINAL.y_pos;
			break;
		case LARROW:
			--TERMINAL.x_pos;
			break;
		case RARROW:
			++TERMINAL.x_pos;
	}
}

// This function gets the position of the cursor 
// using escape sequence '\x1b[6n in the form of
// an escape sequence, e.g. '\x1b[24;80R' which
// is parsed using sscanf
int cursorpos(int* r, int* c)
{
	char* buf = malloc(32 * sizeof(char));
  	unsigned int i = 0;

  	if (write(STDOUT_FILENO, CURSOR_POS, 4) != 4) {
		return -1;
  	}

  	while (i < sizeof(buf)) {
    		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    		if (buf[i] == 'R') break;
    		i++;
  	}

  	buf[i] = '\0';
  	
  	if(buf[0] != '\x1b' || buf[1] != '[') {
  		return -1;
  	}

  	if(sscanf(&buf[2], "%d;%d", r, c) != 2) {
  		return -1;
  	}

  	return 0;
}

// Attempts to get window size using definitions in
// ioctl.h. Failing this, the size of the window is got
// by forcing the cursor to the bottom right corner of the 
// screen and reading its position.
int gwsize(int* r, int* c) 
{
	struct winsize w; 

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
		if(write(STDOUT_FILENO, MV_CURSOR_MAX, 12) != 12) {
			return -1;
		}
		return cursorpos(&r, &c);
	} else {
		*r = w.ws_row;
		*c = w.ws_col;
		return 0;
	}
}

// Appends data to buffer and updates the buffer length
void appendbuf(buffer_t* buf, const char* s, int l)
{
	char* c = realloc(buf->str, buf->len + l);

	// exit if allocation fails
	if(c == NULL) {
		return; 
	}

	memcpy(&c[buf->len], s, l);
	buf->str = c; 
	buf->len += l;
}

void freebuf(buffer_t* buf)
{
	free(buf->str);
}
