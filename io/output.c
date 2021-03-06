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

	// Screen drawing functionality
	// Prints the screen format, status bar
	// and message bar in the terminal window
	drscrn(&buf);
	drstat(&buf);
	drmsg(&buf);

	char c[32];
	snprintf(c, sizeof(c), MV_CURSOR_XY, 
		(TMNL.y_pos - TMNL.rwoffset) + 1, 
		(TMNL.render_x - TMNL.coloffset) + 1);

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
    			appendbuf(buf, "~", 1);
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
		appendbuf(buf, "\r\n", 2);
  	}
}

// Allows the user to scroll through a file by moving the
// cursor around the screen horizontally and vertically
void edscroll() 
{
	TMNL.render_x = 0;

	if(TMNL.y_pos < TMNL.ctrows) {
		TMNL.render_x = indexrender(&TMNL.row[TMNL.y_pos], TMNL.x_pos);
	}

	if(TMNL.y_pos < TMNL.rwoffset) {
		TMNL.rwoffset = TMNL.y_pos;
	}

	if(TMNL.y_pos >= TMNL.rwoffset + TMNL.scrrows) {
		TMNL.rwoffset = TMNL.y_pos - TMNL.scrrows + 1;
	}

	if(TMNL.render_x < TMNL.coloffset) {
		TMNL.coloffset = TMNL.render_x;
	}

	if(TMNL.render_x >= TMNL.coloffset + TMNL.scrcols) {
		TMNL.coloffset = TMNL.render_x - TMNL.scrcols + 1;
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

void drstat(buffer_t* buf)
{
	appendbuf(buf, STYLE_STATUS, 6);
	char status[80], place[80];

	// Displays filename and line count or No File depending on whether or not somehting is open
	int len = snprintf(status, sizeof(status), "%.20s - %d lines", TMNL.filen ? TMNL.filen : "No File", TMNL.ctrows);

	// Displays current line and column
	int plen = snprintf(place, sizeof(place), "Line : %d | Column : %d", TMNL.y_pos + 1, TMNL.x_pos);

	if(len > TMNL.scrcols) {
		len = TMNL.scrcols;
	}

	appendbuf(buf, status, len);

	while(len < TMNL.scrcols) {
		if(TMNL.scrcols - len == plen){
			appendbuf(buf, place, plen);
			break;
		} else {
			appendbuf(buf, " ", 1);
			++len;
		}
	}

	appendbuf(buf, CLEAR_STYLE, 3);
	appendbuf(buf, "\r\n", 2); // Makes an extra line for the status message
}

void drmsg(buffer_t* buf)
{
	appendbuf(buf, CLEAR_LINE, 3);
	appendbuf(buf, STYLE_STATUS, 6);
	int len = strlen(TMNL.stmsg);

	if(len > TMNL.scrcols) {
		len = TMNL.scrcols;
	}

	if(len && time(NULL) - TMNL.sttime < 5) {
		appendbuf(buf, TMNL.stmsg, len);
	}

	appendbuf(buf, CLEAR_STYLE, 3);
}

void edstatmsg(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(TMNL.stmsg, sizeof(TMNL.stmsg), fmt, ap);
	va_end(ap);
	TMNL.sttime = time(NULL);
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
		return cursorpos(r, c);
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
