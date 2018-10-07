#include "../include/minitxt.h"

int main(int argc, char const **argv)
{
	erawm();

	while(1) {
		char c = '\0';
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			kwerror("read");
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d('%c')\r\n", c, c);
		}

		if(c == 'q')	break;
	}

	return 0;
}