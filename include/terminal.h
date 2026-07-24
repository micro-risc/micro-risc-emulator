#pragma once

#include <termios.h>

extern struct termios oldSettings, newSettings;


void cleanupTermios();


