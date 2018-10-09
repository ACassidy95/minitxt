#include "../include/minitxt.h"
#include "../include/state.h"

int main(int argc, char const **argv)
{
	erawm();
	init();

	while(1) {
		rfscrn();
		drscrn();
		pkey();
	}

	return 0;
}
