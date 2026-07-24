#include "terminal.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

struct termios oldSettings, newSettings;

void cleanupTermios(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
	printf("\033[?25h");
	fflush(stdout);
}
