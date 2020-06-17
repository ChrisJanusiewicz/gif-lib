#pragma once
#include "pch.h"
#include <vector>
#include <fstream>
#include "Color.h"




namespace GifLib {
    struct ColorTable
    {
        std::vector<Color> colorTable;
        uint32_t* toVector();
        static ColorTable fromStream(std::ifstream& ifStream, int n);
    };
}

