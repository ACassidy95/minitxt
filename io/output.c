#include "../include/define.h"
#include "../include/funcs.h"

/* ============================================
	    	Output functions 
   ===========================================*/

// Refresh screen
void rfscrn()
{
	edscroll();

	buffer_t buf = BUFFER_INIT; 

	appendbuf(&buf, CURSOR_OFF, 6);
	appendbuf(&buf, MV_CURSOR_00, 3);

	drscrn(&buf);

	char c[32];
	snprintf(c, sizeof(c), MV_CURSOR_XY, 
		(TMNL.y_pos - TMNL.rwoffset) + 1, 
		(TMNL.x_pos - TMNL.coloffset) + 1);

	appendbuf(&buf, c, strlen(c));

	appendbuf(&buf, CURSOR_ON, 6);

	write(STDOUT_FILENO, buf.str, buf.len);
	freebuf(&buf);
}

// Print a column of tildes and welcome messgae
// reposition cursor back at the top of the screen
void drscrn(buffer_t* buf)
{
  	for (int i = 0; i < TMNL.scrrows; i++) {
  		int frow = TMNL.rwoffset + i;
    		if(frow >= TMNL.ctrows) {
    			if(TMNL.ctrows == 0 && i == TMNL.scrrows / 2) {
    				char* msg = calloc(32, sizeof(char));
    				int wlen = snprintf(msg, 32, WELCOME);

    				if(wlen > TMNL.scrcols) {
    					wlen = TMNL.scrcols;
    				}

    				int padding = (TMNL.scrcols - wlen) >> 1;

    				if(padding) {
    					appendbuf(buf, "~", 1); 
    					--padding;
    				}

    				while(--padding) {
    					appendbuf(buf, " ", 1);
    				}

    				appendbuf(buf, msg, wlen);
    			} else {
    				appendbuf(buf, "~", 1);
    			}
    		} else {
    			int len = TMNL.row[frow].render - TMNL.coloffset;

    			// Since the chars of each row are indexed relative to the
    			// column offset, this ensures we never go into negative
    			// cursor positions
    			if(len < 0) {
    				len = 0;
    			}

    			if(len > TMNL.scrcols) {
    				len = TMNL.scrcols;
    			}
    			appendbuf(buf, &TMNL.row[frow].rendernp[TMNL.coloffset], len);
    		}

    		appendbuf(buf, CLEAR_LINE, 3);

    		if(i < TMNL.scrrows - 1) {
    			appendbuf(buf, "\r\n", 2);
    		}
  	}
}

// Allows the user to scroll through a file by moving the
// cursor around the screen horizontally and vertically
void edscroll() 
{
	if(TMNL.y_pos < TMNL.rwoffset) {
		TMNL.rwoffset = TMNL.y_pos;
	}

	if(TMNL.y_pos >= TMNL.rwoffset + TMNL.scrrows) {
		TMNL.rwoffset = TMNL.y_pos - TMNL.scrrows + 1;
	}

	if(TMNL.x_pos < TMNL.coloffset) {
		TMNL.coloffset = TMNL.x_pos;
	}

	if(TMNL.x_pos >= TMNL.coloffset + TMNL.scrcols) {
		TMNL.coloffset = TMNL.x_pos - TMNL.scrcols + 1;
	}
}

// This function gets the position of the cursor 
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

// Moves cursor around the screen while ensuring its 
// current coordinates don't move beyond the screen bounds
void mvcursor(int c)
{
	edrow_t* r = (TMNL.y_pos >= TMNL.ctrows) ? NULL : &TMNL.row[TMNL.y_pos];

	switch(c) {
		case UARROW:
			if(TMNL.y_pos != 0) {
				--TMNL.y_pos;
			}
			break;
		case DARROW:
			if(TMNL.y_pos < TMNL.ctrows) {
				++TMNL.y_pos;
			}
			break;
		case LARROW:
			if(TMNL.x_pos != 0) {
				--TMNL.x_pos;
			} else if(TMNL.y_pos > 0) {
				// Snap cursor to end of prev line if user hits left at line start
				TMNL.y_pos--;
				TMNL.x_pos = TMNL.row[TMNL.y_pos].len;
			}
			break;
		case RARROW:
			if(r && TMNL.x_pos < r->len) {
				++TMNL.x_pos;
			} else if(r && TMNL.x_pos == r->len) {
				// Snap cursor to start of next line if user hits right at line end
				++TMNL.y_pos;
				TMNL.x_pos = 0;
			}
			break;
	}

	// Snap the cursor back to the end of the current row
	// if scrolling from a longer one
	r = (TMNL.y_pos >= TMNL.ctrows) ? NULL : &TMNL.row[TMNL.y_pos];
	int rlen = r ? r->len : 0;
	if(TMNL.x_pos > rlen) {
		TMNL.x_pos = rlen;
	}
}
