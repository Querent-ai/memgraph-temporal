#pragma once

#include "storage/model/properties/integral.hpp"

class Int64 : public Integral<Int64>
{
public:
    static constexpr Flags type = Flags::Int64;

    Int64(int64_t value) : Integral(Flags::Int64), value(value) {}

    int64_t const &value_ref() const { return value; }

    int64_t value;
};
