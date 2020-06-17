#pragma once
#include "pch.h"
#include <cstdint>

namespace GifLib {

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        Color(uint8_t r, uint8_t g, uint8_t b);

        static Color fromRGB(uint8_t r, uint8_t g, uint8_t b);
        void print();
    };
}

