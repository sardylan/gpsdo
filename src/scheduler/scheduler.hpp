/*
 * GPS Dominated Oscillator for RaspberryPi Pico (RP2040)
 * Copyright (C) 2026  Luca Cireddu <sardylan@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __SCHEDULER__SCHEDULER__H
#define __SCHEDULER__SCHEDULER__H

#include <vector>

#include "task.hpp"

namespace gpsdo::scheduler {
    class Scheduler {
    public:
        Scheduler();

        ~Scheduler();

        void addTask(TaskJob taskJob, uint64_t interval = 1000000);

        void execute();

    private:
        std::vector<Task> tasks;
    };
}

#endif
