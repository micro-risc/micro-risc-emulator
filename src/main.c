#include "input.h"
#include "emulator.h"
#include "terminal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


int main(int argc, char** argv) {
	/* Setup Termios */
	tcgetattr(STDIN_FILENO, &oldSettings);

	newSettings = oldSettings;
	/* Changed Settings */
	newSettings.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
	printf("\033[?25l");
	fflush(stdout);	

	/* Bind Termios cleanup to exit */
	atexit(cleanupTermios);

	/* Parse Arguments */
	args = parseArgs(argc, argv);
	
    runEmulator();

	exit(0);
}
