#include "memory/mmio.h"
#include "fault/faultCodes.h"

#include <stdint.h>
#include <time.h>
#include <stdio.h>

/* Screenbuf mmio */
#define WIDTH 80
#define HEIGHT 25

#define UPDATE_FREQ 32 /* roughly 30Hz */

#define FB_BASE 0xF0001000

static uint8_t screenbuf[WIDTH][HEIGHT];

static struct timespec lastUpdate;
static struct timespec currentTime;

#define TERM_CLEAR() printf("\033[2J")
#define TERM_GOTO(x,y) printf("\033[%d;%dH", (y), (x))

void displayTerminal() {
	TERM_CLEAR();
	TERM_GOTO(0, 0);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			TERM_GOTO(x+1, y+1);

			putc((char)screenbuf[x][y], stdout);
		}
	}
}

FaultCode writeScreenbuf(uint64_t addr, int bytes, uint64_t val) {
	/* Convert Address into x,y */
	for (int i = 0; i < bytes; i++) {
		uint64_t offset = addr - FB_BASE + i;
		int x = (int)(offset % WIDTH);
		int y = (int)(offset / WIDTH);
		if (y >= HEIGHT) return 1; /* Out of Range, Fault */

		screenbuf[x][y] = (uint8_t)((val >> i*8) & 0xFF);
	}

	/* Show it if enough time has passed */
	clock_gettime(CLOCK_MONOTONIC, &currentTime);

	uint64_t currentTimeMs = (uint64_t)currentTime.tv_sec * 1000 + (uint64_t)currentTime.tv_nsec / 1000000;
	uint64_t lastUpdateMs = (uint64_t)lastUpdate.tv_sec * 1000 + (uint64_t)lastUpdate.tv_nsec / 1000000;

	if (currentTimeMs - lastUpdateMs > UPDATE_FREQ) {
		lastUpdate = currentTime;
		
		displayTerminal();
	}

	return FAULT_NONE; /* didnt fault */
}

FaultCode readScreenbuf(uint64_t addr, int bytes, uint64_t* val) {

	return FAULT_NONE;
}


MmioEntry mmioEntries[1] = {
	{
		.base = FB_BASE,
		.length = WIDTH * HEIGHT,
		.writeMmio = &writeScreenbuf,
		.readMmio = &readScreenbuf
	}
};
