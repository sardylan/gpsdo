#ifndef GPSDO__FIRMWARE__MAIN__H
#define GPSDO__FIRMWARE__MAIN__H

#include "scheduler.h"

void init();

void program_core0();

void program_core1();

void job_core0();

void job_core1();

scheduler_task_header(oscillator_correction);

scheduler_task_header(display_update);

#endif
