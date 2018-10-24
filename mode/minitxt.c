#include "../include/define.h"
#include "../include/funcs.h"

int main(int argc, char const **argv)
{
	erawm();
	init();

	if(argc >= 2) {
		edopen(argv[1]);
	}

	edstatmsg("HELP: Ctrl-q = quit");

	while(1) {
		rfscrn();
		pkey();
	}

	return 0;
}

