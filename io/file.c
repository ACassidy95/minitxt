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

char* edrtos(int* len)
{
	int total_len = 0;

	for(int i = 0; i < TMNL.ctrows; ++i) {
		total_len += TMNL.row[i].len + 1;
	}

	*len = total_len;

	char* buffer = malloc(total_len);
	char* j = buffer;

	for(int i = 0; i < TMNL.ctrows; ++i) {
		memcpy(j, TMNL.row[i].chars, TMNL.row[i].len);
		j += TMNL.row[i].len; 
		*j = '\n';
		++j;
	}

	return buffer;
}

void edsave()
{
	if(TMNL.filen == NULL) {
		return;
	}

	int len;
	char* buffer = edrtos(&len);

	// Create a new file if necessary
	int f = open(TMNL.filen, O_RDWR | O_CREAT, 0644);
	ftruncate(f, len);
	write(f, buffer, len);
	close(f);
	free(buffer);
}
