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
		(TMNL.render_x - TMNL.coloffset) + 1);

	appendbuf(&buf, c, strlen(c));

	appendbuf(&buf, CURSOR_ON, 6);

	write(STDOUT_FILENO, buf.str, buf.len);
	freebuf(&buf);
}

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
