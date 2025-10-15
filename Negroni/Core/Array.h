#pragma once

#include "Base.h"

#include <malloc.h>

namespace Core
{
    template<typename ValueType>
    class Array
    {
    public:
        Array(u8 size) : size(size), index(0)
        {
            data = (ValueType*)alloca(size * sizeof(ValueType));
        }

        bool push(const ValueType& value)
        {
            if (index >= size) return false;
            data[index++] = value;
            return true;
        }

        ValueType& pop()
        {
            if (index < 1) index = 1;
            ValueType& value = data[--index];
            return value;
        }

        void clear() { index = 0; }
        u8 length() { return index; }
        u8 size() { return size; }

    private:
        ValueType* data;
        u8         size;
        u8         index;
    };
}
