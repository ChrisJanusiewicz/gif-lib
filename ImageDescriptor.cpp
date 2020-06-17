#include "pch.h"
#include "ImageDescriptor.h"
#include <iostream>

namespace GifLib {
bool ImageDescriptor::hasLocalColorTable()
{
	return packedFields & 0b10000000;
}

bool ImageDescriptor::getInterlaceFlag()
{
	return packedFields & 0b01000000;
}

bool ImageDescriptor::isSorted()
{
	return packedFields & 0b00100000;
}

size_t ImageDescriptor::getLocalColorTableSize()
{
	return 2 << (packedFields & 0b00000111);
}

ImageDescriptor ImageDescriptor::fromStream(std::ifstream& input)
{
	ImageDescriptor imageDescriptor;
	input.read((char*)&imageDescriptor.imageSeparator, 1);
	input.read((char*)&imageDescriptor.imagePosLeft, 2);
	input.read((char*)&imageDescriptor.imagePosTop, 2);
	input.read((char*)&imageDescriptor.imageWidth, 2);
	input.read((char*)&imageDescriptor.imageHeight, 2);
	input.read((char*)&imageDescriptor.packedFields, 1);
	return imageDescriptor;
}

void ImageDescriptor::print()
{
	std::cout << "Separator: " << imageSeparator << std::endl;
	std::cout << "Position: {" << imagePosLeft << ", " << imagePosTop << "}" << std::endl;
	std::cout << "Size: {" << imageWidth << ", " << imageHeight << "}" << std::endl;
	std::cout << "Packed: " << (int)packedFields << std::endl;
	std::cout << "LCT: " << hasLocalColorTable() << std::endl;
	std::cout << "Interlace: " << getInterlaceFlag() << std::endl;
	std::cout << "Sorted: " << isSorted() << std::endl;
	std::cout << "LCT Size: " << getLocalColorTableSize() << std::endl;
}
}