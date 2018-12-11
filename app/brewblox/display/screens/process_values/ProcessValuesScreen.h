/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ProcessValue.h"
#include "ProcessValueWidget.h"
#include "d4d.hpp"
#include <stdint.h>
#include <vector>

class ProcessValuesScreen {
private:
    static std::vector<ProcessValueWidget> pvWidgets;

public:
    ProcessValuesScreen() = default;
    ~ProcessValuesScreen() = default;

    static void init();
    static void activate();
    static void updateUsb();
    static void updateWiFi();
    static void searchPVs();
    static void updatePVs();
};

extern const D4D_SCREEN* process_values_screen;