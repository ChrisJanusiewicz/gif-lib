#pragma once
#include "pch.h"
#include <cstdint>

namespace GifLib {
    struct GifGCE 
    {
        // Contains parameters used when processing a graphic rendering block
        // The scope of this extension is the first graphic rendering block to follow.
        // The extension contains only one data sub - block.
        // This block is OPTIONAL
        // Tt most one Graphic Control Extension may precede a graphic rendering block.

        uint8_t extensionIntroducer;   // Identifies the beginning of an extension. Always 0x21
        uint8_t graphicControlLabel;   // Identifies the current block as a Graphic Control Extension field contains the fixed value 0xF9.

        uint8_t blockSize;              // Block Size - Number of bytes in the block, after the Block Size Field up to but not including Block Terminator. Fixed Value 4
        uint8_t packedFields;
        uint16_t delayTime;
        uint8_t transparentColorIndex;
        uint8_t blockTerminator;

        uint8_t getDisposalMethod();
        bool getUserInputFlag();
        bool getTransparentColorFlag();


    };
}

