/*
 * Copyright 2015 BrewPi/Elco Jacobs.
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

#include "temperatureFormats.h"
#include "ControllerMixins.h"
#include "Interface.h"

class SetPoint : public virtual Interface, public SetPointMixin{
public:
    SetPoint() = default;
    virtual ~SetPoint() = default;
    virtual temp_t read() const = 0;
    virtual void write(temp_t val) = 0;
    virtual update_t update(const update_t & t) = 0;
friend class SetPointMixin;
};

class SetPointSimple final : public SetPoint, public SetPointSimpleMixin {
public:
    temp_t setting;

    SetPointSimple() : setting(temp_t::disabled()){}
    SetPointSimple(temp_t _value) : setting(_value){}
    ~SetPointSimple() = default;

    /**
     * Accept function for visitor pattern
     * @param dispatcher Visitor to process this class
     */
    virtual void accept(VisitorBase & v) override final {
    	v.visit(*this);
    }

    virtual temp_t read() const override final {
        return setting;
    }

    virtual void write (temp_t val) override final {
        setting = val;
    }

    virtual update_t update(const update_t & t) override final {
        return update_t_max(); // no updates needed
    }

friend class SetPointSimpleMixin;
};

class SetPointMinMax final : public SetPoint, public SetPointMinMaxMixin {
public:
    SetPointMinMax(temp_t val = temp_t::disabled()) : value(val),
                                                      min(temp_t::min()),
                                                      max(temp_t::max()){}
    ~SetPointMinMax() = default;

    /**
     * Accept function for visitor pattern
     * @param dispatcher Visitor to process this class
     */
    virtual void accept(VisitorBase & v) override final {
    	v.visit(*this);
    }

    virtual temp_t read() const override final {
        return value;
    }
    virtual void write (temp_t val) override final {
        if(val < min){
            value = min;
        }
        else if(val > max){
            value = max;
        }
        else{
            value = val;
        }
    }
    void setMin(temp_t t){
        min = t;
    }
    void setMax(temp_t t){
        max = t;
    }
    temp_t getMin(){
        return min;
    }
    temp_t getMax(){
        return max;
    }

    virtual update_t update(const update_t & t) {
        return update_t_max(); // no updates needed
    }

private:
    temp_t value;
    temp_t min;
    temp_t max;
friend class SetPointMinMaxMixin;
};



// immutable SetPoint, always reading for example 'invalid' to indicate that the setpoint has not been configured
class SetPointConstant final : public SetPoint, public SetPointConstantMixin {
public:
    SetPointConstant(const temp_t val): value(val){}
    ~SetPointConstant() = default;

    /**
     * Accept function for visitor pattern
     * @param dispatcher Visitor to process this class
     */
    virtual void accept(VisitorBase & v) override final {
    	v.visit(*this);
    }

    virtual temp_t read() const override final{
        return value;
    }
    virtual void write(temp_t val) override final { // does nothing
    }

    virtual update_t update(const update_t & t) override final {
        return update_t_max(); // no updates needed
    }

private:
    const temp_t value;

friend class SetPointConstantMixin;
};
