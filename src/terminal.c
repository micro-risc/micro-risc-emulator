#include "terminal.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct termios oldSettings, newSettings;

void cleanupTermios(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}
