#include "../include/define.h"
#include "../include/funcs.h"

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
		case '\r':
			//TODO
			break;

		case CTRL_K('q'):
			write(STDOUT_FILENO, CLEAR_SCREEN, 4);
			write(STDOUT_FILENO, MV_CURSOR_00, 3);
			exit(0);
			break;

		case CTRL_K('h'):
			edstatmsg("Ctrl-Q - quit | CTRL-S - save changes");
			break;

		case CTRL_K('s'):
			edsave();
			break;

		case HOME:
			TMNL.x_pos = 0;
			break;
		case END:
			if(TMNL.y_pos < TMNL.ctrows) {
				TMNL.x_pos = TMNL.row[TMNL.y_pos].len;
			}
			break;

		case BSPACE:
		case CTRL_K('r'):
		case DEL:
			//TODO
			break;

		case PGUP:
		case PGDN:
			{
				if(c == PGUP) {
					TMNL.y_pos = TMNL.rwoffset;
				} else if(c == PGDN) {
					TMNL.y_pos = TMNL.rwoffset + TMNL.ctrows - 1;
					if(TMNL.y_pos > TMNL.ctrows) {
						TMNL.y_pos = TMNL.ctrows;
					}
				}

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

		// Traditionally, CTRL-L refreshes the screen in
		// terminal programs but this happens with each 
		// keypress in minitxt to this combo is ignored
		/*case CTRL_K('l'):
		case '\x1b';
			//TODO
			break;*/

		default:
			edinsertchar(c);
			break;
	}
}












