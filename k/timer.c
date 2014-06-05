#include "timer.h"

unsigned long ticks = 0;

unsigned long gettick()
{
    return ticks;
}

void tick()
{
    ++ticks;
}
