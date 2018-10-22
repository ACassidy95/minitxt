#include "../include/define.h"
#include "../include/funcs.h"

int main(int argc, char const **argv)
{
	erawm();
	init();

	if(argc >= 2) {
		edopen(argv[1]);
	}

	while(1) {
		rfscrn();
		pkey();
	}

	return 0;
}

