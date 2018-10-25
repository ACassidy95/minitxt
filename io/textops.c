#include "../include/define.h"
#include "../include/funcs.h"

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

	TMNL.row[at].render = 0;
	TMNL.row[at].rendernp = NULL;

	edupdater(&TMNL.row[at]);

	TMNL.ctrows++;
}

void edupdater(edrow_t* r)
{
	int tabs = 0; 
	int j; 

	for(j = 0; j < r->len; ++j) {
		if(r->chars[j] == '\t') {
			++tabs; 
		}
	}

	free(r->rendernp);
	r->rendernp = malloc(r->len + tabs * (TAB_STOP - 1) + 1);

	int idx = 0; 

	for(j = 0; j < r->len; ++j) {
		if(r->chars[j] == '\t') {
			r->rendernp[idx++] = ' ';
			while(idx % TAB_STOP != 0) {
				r->rendernp[idx++] = ' ';
			}
		} else {
			r->rendernp[idx++] = r->chars[j];
		}
	}

	r->rendernp[idx] = '\0';
	r->render = idx;
}

void insertchar(edrow_t* r, int idx, int c)
{
	if(idx < 0 || idx > r->len) {
		idx = r->len;
	}
	r->chars = realloc(r->chars, r->len + 2); // add 2 to make room for '\0'
	memmove(&r->chars[idx + 1], &r->chars[idx], r->len - idx + 1);
	++r->len; 
	r->chars[idx] = c; 
	edupdater(r);
}

// Appends a new row to the file if it's at the last one
// then inserts the characters and bumps the cursor along one space
void edinsertchar(int c)
{
	if(TMNL.y_pos == TMNL.ctrows) {
		edappendr("", 0); 

	}
	insertchar(&TMNL.row[TMNL.y_pos], TMNL.x_pos, c);
	++TMNL.x_pos;
}

int indexrender(edrow_t* r, int cx)
{
	int rx = 0; 
	
	for(int j = 0; j < cx; j++) {
		if(r->chars[j] == '\t') {
			rx += (TAB_STOP - 1) - (rx % TAB_STOP);
		}
		++rx;
	}

	return rx;
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
