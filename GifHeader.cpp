#include "pch.h"
#include "GifHeader.h"
#include <iostream>

namespace GifLib {
	GifHeader GifHeader::fromStream(std::ifstream& input)
	{
		GifHeader header;

		input.read((char*)header.signature, 3);
		input.read((char*)header.version, 3);
		input.read((char*)&header.width, 2);
		input.read((char*)&header.height, 2);
		input.read((char*)&header.packedFields, 1);
		input.read((char*)&header.backgroundColorIndex, 1);
		input.read((char*)&header.pixelAspectRatio, 1);

		return header;
	}

	bool GifHeader::isValid()
	{
		// TODO: check signature and version etc
		return true;
	}

	bool GifHeader::hasGlobalColorTable()
	{
		return packedFields & 0b10000000;
	}

	bool GifHeader::isSorted()
	{
		return packedFields & 0b00001000;
	}

	size_t GifHeader::getColorResolution()
	{
		return ((packedFields & 0b01110000) >> 4) + 1;
	}

	size_t GifHeader::getGlobalColorTableSize()
	{
		return 2 << (packedFields & 0b00000111);
	}

	void GifHeader::print() {
		std::cout << "GIF Header: " << signature << version << std::endl;
		std::cout << "LSD: " << (unsigned short)width << " x " << height << std::endl;
		std::cout << "Packed: " << (int)packedFields << std::endl;
		std::cout << "GCT: " << hasGlobalColorTable() << std::endl;
		std::cout << "Sorted: " << isSorted() << std::endl;
		std::cout << "Color Res: " << getColorResolution() << std::endl;
		std::cout << "GCT Size: " << getGlobalColorTableSize() << std::endl;
		std::cout << "BCI: " << (int)backgroundColorIndex << std::endl;
		std::cout << "PAR: " << (int)pixelAspectRatio << std::endl;
	}


}
