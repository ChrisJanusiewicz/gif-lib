#pragma once
#include "pch.h"
#include <cstdint>
#include <fstream>

namespace GifLib {
    struct GifHeader {
        //Header    6 Bytes
        uint8_t signature[3];
        uint8_t version[3];

        // Logical Screen Descriptor    7 Bytes
        uint16_t width;
        uint16_t height;
        uint8_t packedFields;   // Global Color Table Flag  1 Bit
                                // Color Resolution         3 Bits
                                // Sort Flag                1 Bit
                                // Global Color Table Size  3 Bits
        uint8_t backgroundColorIndex;
        uint8_t pixelAspectRatio;

        bool isValid();
        bool hasGlobalColorTable();
        bool isSorted();


        static GifHeader fromStream(std::ifstream& input);

        size_t getColorResolution();
        size_t getGlobalColorTableSize();
        void print();


    };

}