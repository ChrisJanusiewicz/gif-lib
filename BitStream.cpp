#include "pch.h"
#include "BitStream.h"

BitStream::BitStream(std::ifstream &inputStream)
{
    ifStream = &inputStream;
    position = 8;
    ofStream = nullptr;
    writeOnly = false;
    readOnly = true;
}

BitStream::BitStream(std::ofstream &outputStream)
{
    ofStream = &outputStream;
    position = 8;
    ifStream = nullptr;
    writeOnly = true;
    readOnly = false;
}

uint8_t BitStream::createMask(int width, int offset) {

    uint32_t mask = (1 << width) - 1;
    mask = mask << offset;
    return (uint8_t)mask;
}

void BitStream::nextByte() {
    if (blockSize == 0) {
        // finished reading current block.

        ifStream->read((char*)&blockSize, 1);
        if (blockSize == 0) {
            // there are no more blocks
            std::cout << "I don't think we should ever get here" << std::endl;
        }
        //std::cout << "Advanced to next block. Size: " << (int)blockSize << std::endl;
    }

    ifStream->read((char*)&currentByte, 1);    
    blockSize--;
    // Advance to next byte
}

int BitStream::readBits(uint8_t &bits, int n) {

    if (position >= 8) {
        nextByte();
        position = 0;
    }

    // Only take a maximum of 8 bits at a time
    // Only take a maximum of (8 - position) from given byte
    int nBits = std::min((8 - position), std::min(n, 8));


    uint8_t mask = createMask(nBits, position);
    

    bits = (currentByte & mask) >> position;
    /*
    std::cout << "reading " << n << std::endl;
    std::cout << "current byte: " << (int)currentByte << std::endl;
    std::cout << "position: " << (int)position << std::endl;
    std::cout << "mask: " << (int)mask << std::endl;
    std::cout << "result: " << (int)bits << std::endl;*/

    position += nBits;

    return nBits;
}


void BitStream::write(uint16_t code, int codeSize)
{

}

uint16_t BitStream::read(int codeSize)
{
    uint16_t buffer = 0;
    int bufferPos = 0;

    int bitsRemaining = codeSize;
    int bitsRead = 0;
    int bitsToTake;
    uint8_t bits = 0;


    while (bitsRemaining != 0) {

        bitsRead = readBits(bits, bitsRemaining);
        buffer = buffer | (bits << bufferPos);
        bitsRemaining -= bitsRead;
        bufferPos += bitsRead;

    }

    return buffer;
}
