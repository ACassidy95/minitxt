#ifndef _FUNCS_H_
#define _FUNCS_H_

void init();

/* ============================================
	    Terminal mode functions 
   ===========================================*/

// kill with error - takes in an error
// string and prints it to stdout and exits
void kwerror(const char* s);

// disable raw mode - return terminal to
// canonical mode on termination
void drawm();

// enable raw mode - change terminal to
// raw mode from canonical mode
void erawm();

/* ============================================
	    	Input functions 
   ===========================================*/

// read key & process key - read a key 
// press and decide how to respond
int rkey();
void pkey();

/* ============================================
	    	Output functions 
   ===========================================*/

// refreshes the screen
void rfscrn();

// draw a vim-like column of tildes on the screen
void drscrn(buffer_t* buf);

// Allows the user to scroll through a file
void edscroll();

// Draws the status bar at the bottom of the screen
void drstat(buffer_t* buf);

// Draws the message bar at the bottom of the screen
void drmsg(buffer_t* buf);

// Used to print the status message on the status bar
void edstatmsg(const char* fmt, ...);

// get window information
int gwsize(int* r, int* c);
int cursorpos(int* r, int* c);
void mvcursor(int c);

/* ============================================
	    	File IO functions 
   ===========================================*/

// Opens a file for reading in the editor
void edopen(const char* fname); 

// Saves each edrow_t as a string to be written to disk
char* edrtos(int* len);

// Saves changes to file
void edsave();

/* ============================================
	    	Row functions 
   ===========================================*/

// Appends a row from file to print to the editor
void edappendr(char* s, size_t len);

// Updates a row with characters to be rendered to the screen
void edupdater(edrow_t* r);

// Used to insert text to a row and update the editor with
// the inseted text
void insertchar(edrow_t* r, int idx, int c);
void edinsertchar(int c);

// Convert actual index to render index
int indexrender(edrow_t* r, int cx);

/* ============================================
	    	Buffer functions 
   ===========================================*/

// adds string data of length l to an existing buffer
void appendbuf(buffer_t* buf, const char* s, int l);

// frees a buffer
void freebuf(buffer_t* buf); 

#endif
