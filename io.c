#include "minitxt.h"

/* ============================================
	    	Input functions 
   ===========================================*/

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
			if(seq[1] >= '0' && seq[1] <= '9'){
				if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
				if(seq[2] == '~') {
					switch(seq[2]) {
						// Home and end have two ways of being sent each.
						// Correspondance here is awkward but that's just
						// how it is
						case '1' : return HOME;
						case '3' : return DEL;
						case '4' : return END;
						case '5' : return PGUP;
						case '6' : return PGDN;
						case '7' : return HOME;
						case '8' : return END;
					}
				}
			} else {
				switch(seq[1]) {
					case 'A' : return UARROW;
					case 'B' : return DARROW;
					case 'C' : return RARROW; 
					case 'D' : return LARROW;
					case 'H' : return HOME;
					case 'F' : return END;
				}
			}
		} else if (seq[0] == 'O') {
			// Home and end esc sequence must also be dealt
			// with in this format <esc>H/F or <esc>OH/OF
			switch(seq[0]) {
				case 'H' : return HOME;
				case 'F' : return END;
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

		case HOME:
			TMNL.x_pos = 0;
			break;
		case END:
			TMNL.x_pos = TMNL.scrcols - 1; 

		case PGUP:
		case PGDN:
			{
				int times = TMNL.scrrows - 1;
				while(--times) {
					mvcursor(c == PGUP ? UARROW : DARROW);
				}
			} 
			break; 

		case UARROW:
		case DARROW:
		case LARROW:
		case RARROW:
			mvcursor(c);
			break;
	}
}

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
    			int len = TMNL.row[frow].len - TMNL.coloffset;

    			// Since the chars of each row are indexed relative to the
    			// column offset, this ensures we never go into negative
    			// cursor positions
    			if(len < 0) {
    				len = 0;
    			}

    			if(len > TMNL.scrcols) {
    				len = TMNL.scrcols;
    			}
    			appendbuf(buf, &TMNL.row[frow].chars[TMNL.coloffset], len);
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

/* ============================================
	    	File IO functions 
   ===========================================*/

void edopen(const char* fname)
{
	FILE *fp = fopen(fname, "r");
	if(!fp) {
		kwerror("edopen");
	}

	char* line = NULL;
	size_t lncap = 0; 
	ssize_t lnlen;

	while ((lnlen = getline(&line, &lncap, fp)) != -1) {
		while(lnlen > 0 && (line[lnlen - 1] == '\n' || line[lnlen - 1] == '\r')) {
			lnlen--; 
		}
		edappendr(line, lnlen);
	}

	free(line);
	fclose(fp);
}

/* ============================================
	    	Row functions 
   ===========================================*/

void edappendr(char* s, size_t len)
{
	TMNL.row = realloc(TMNL.row, sizeof(edrow_t) * (TMNL.ctrows + 1));

	int at = TMNL.ctrows;
	TMNL.row[at].len = len; 
	TMNL.row[at].chars = malloc(len + 1);
	memcpy(TMNL.row[at].chars, s, len);
	TMNL.row[at].chars[len] = '\0';
	TMNL.ctrows++;
}

/* ============================================
	    	Buffer functions 
   ===========================================*/

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
