#include "mmio.h"

#include <time.h>

/* Screenbuf mmio */
#define WIDTH 80
#define HEIGHT 25

#define UPDATE_FREQ 16 /* roughly 60Hz */

static uint8_t screenbuf[WIDTH][HEIGHT];

static struct timespec lastUpdate;
static struct timespec currentTime;

void displayTerminal() {
	for (int y = 0; y < WIDTH; y++) {
		for (int x = 0; x < WIDTH; x++) {
			
		}
	}
}

bool writeScreenbuf(uint64_t addr, int bytes, uint64_t val) {
	
	/* Show it if enough time has passed */
	clock_gettime(CLOCK_MONOTONIC, &currentTime);

	uint64_t currentTimeMs = (uint64_t)currentTime.tv_sec / 1000 + (uint64_t)currentTime.tv_nsec * 1000000;
	uint64_t lastUpdateMs = (uint64_t)lastUpdate.tv_sec / 1000 + (uint64_t)lastUpdate.tv_nsec * 1000000;

	if (currentTimeMs - lastUpdateMs > UPDATE_FREQ) {
		lastUpdate = currentTime;
		
	}

	return 0;
}

bool readScreenbuf(uint64_t addr, int bytes, uint64_t* val) {
	return 0;
}


MmioEntry mmioEntries[1] = {
	{
		.base = 0xF0001000,
		.length = 2000,
		.writeMmio = &writeScreenbuf,
		.readMmio = &readScreenbuf
	}
};
