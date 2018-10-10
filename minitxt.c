#include "minitxt.h"

int main(int argc, char const **argv)
{
	erawm();
	init();

	while(1) {
		rfscrn();
		pkey();
	}

	return 0;
}
