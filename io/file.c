#include "../include/define.h"
#include "../include/funcs.h"

/* ============================================
	    	File IO functions 
   ===========================================*/

void edopen(const char* fname)
{
	free(TMNL.filen);
	TMNL.filen = strdup(fname);

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
