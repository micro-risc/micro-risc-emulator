#include "input.h"
#include "emulator.h"
#include <stdint.h>
#include <ncurses.h>

int main(int argc, char** argv) {
    args = parseArgs(argc, argv);

    runEmulator();
}