#pragma once
#include "pch.h"
#include <fstream>
#include <algorithm>
#include <iostream>



class BitStream
{
public:
	BitStream(std::ifstream &inputStream);
	BitStream(std::ofstream &outputStream);
	void write(uint16_t code, int codeSize);
	uint16_t read(int codeSize);

private:
	std::ifstream *ifStream;
	std::ofstream *ofStream;

	uint8_t blockSize = 0;
	int position;
	uint8_t currentByte;
	bool readOnly;
	bool writeOnly;

	void nextByte();
	uint8_t createMask(int width, int offset);
	int readBits(uint8_t &bits, int n);


};

