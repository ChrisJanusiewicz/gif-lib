#include "pch.h"
#include "ColorTable.h"
#include <iostream>

namespace GifLib {
	uint32_t* ColorTable::toVector()
	{
		size_t size = colorTable.size();
		uint32_t* buf = (uint32_t*)malloc(size * 4);

		size_t pos = 0;
		for (size_t i = 0; i < size; i++) {
			Color col = colorTable.data()[i];
			uint32_t i32 = 0xFF000000;
			i32 = i32 | (col.r << 16) | (col.g << 8) | col.b;
			buf[pos++] = i32;
			// buf[pos++] = pos;
		}

		return buf;
	}
	
	ColorTable ColorTable::fromStream(std::ifstream& ifStream, int n)
	{


		ColorTable ct;
		uint8_t r, g, b;
		int bytes_read = 0;

		for (size_t i = 0; i < n; i++) {
			ifStream.read((char*)&r, 1);
			ifStream.read((char*)&g, 1);
			ifStream.read((char*)&b, 1);
			ct.colorTable.push_back(Color(r, g, b));
		}

		std::cout << "Just read a color table... Bytes: " << bytes_read << std::endl;

		return ct;
	}

	ColorTable parseColorTable(std::ifstream& input, size_t colorDepth, size_t paletteSize)
	{


		return ColorTable::fromStream(input, paletteSize);
	}


}