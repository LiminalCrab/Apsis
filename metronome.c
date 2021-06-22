#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

struct timeval start, last;

int metro (int c, char  **v)
{
    int bpm;

    if (c < 2 || (bpm = atoi(v[1])) <= 0) bpm = 60;
    if (bpm > 600) 
    {
        fprintf(stderr, "Frequency %d too high!\n", bpm);
    }
    gettimeofday(&start, 0);
    last = start;

    return 0;

}
