/*
 * Copyright 2016 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
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

#pragma once
#include <stdint.h>
#include "ControllerMixins.h"
#include "VisitorBase.h"
#include <limits>

using update_t = uint32_t;
inline update_t update_t_max(){
    return std::numeric_limits<update_t>::max();
}

class Interface :
    public InterfaceMixin
{
public:
	Interface() = default;
    virtual ~Interface() = default;
public:
    virtual update_t update(const update_t & t) = 0;
	virtual void accept(VisitorBase & v) = 0;
};
