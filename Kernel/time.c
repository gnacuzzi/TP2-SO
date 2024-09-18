#include <time.h>
#include <lib.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void wait_time(int delta){ 
	int currentTicks = ticks;
	while(ticks - currentTicks < delta){
		htl_lib();
	}
}