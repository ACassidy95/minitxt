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
	free(r->rendernp);
	r->rendernp = malloc(r->len + 1);

	int j;
	for(j = 0; j < r->len; ++j) {
		r->rendernp[j + 1] = r->chars[j];
	}

	r->rendernp[j] = '\0';
	r->render = j;
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
