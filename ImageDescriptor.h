#pragma once
#include "pch.h"
#include <fstream>
#include <cstdint>

namespace GifLib {
    struct ImageDescriptor
    {
        uint8_t imageSeparator; // Identifies beginning of image descriptor: 0x2C
        uint16_t imagePosLeft;  // image offset from logical screen
        uint16_t imagePosTop;   // image offset from logical screen
        uint16_t imageWidth;
        uint16_t imageHeight;
        uint8_t packedFields;   // Local Color Table Flag   1 Bit
                                // Interlace Flag           1 Bit
                                // Sort Flag                1 Bit
                                // Reserved Bits            2 Bits
                                // Local Color Table Size   3 Bits

        bool hasLocalColorTable();
        bool getInterlaceFlag();
        bool isSorted();
        size_t getLocalColorTableSize();

        static ImageDescriptor fromStream(std::ifstream& ifStream);
        void print();
    };

}