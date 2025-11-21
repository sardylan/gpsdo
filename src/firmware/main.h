#ifndef GPSDO__FIRMWARE__MAIN__H
#define GPSDO__FIRMWARE__MAIN__H

#include "scheduler.h"

void init();

void program_core0();

void program_core1();

void job_core0();

void job_core1();

void task_oscillator_correction();

void task_display_update();

#endif
