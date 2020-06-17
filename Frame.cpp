#include "pch.h"
#include "Frame.h"
#include "LzwEngine.h"
#include "ColorTable.h"





namespace GifLib {

	Frame Frame::parseFrame(std::ifstream& input,
		ImageDescriptor imageDescriptor,
		ColorTable *globalColorTable)
	{

		// check for local color table
		ColorTable *ct;
		if (imageDescriptor.hasLocalColorTable()) {
			 ct = &ColorTable::fromStream(input, imageDescriptor.getLocalColorTableSize());
		}
		else {
			ct = globalColorTable;
		}



		uint8_t lzwCodeLength;

		input.read((char*)&lzwCodeLength, 1);

		//std::cout << "lzw code size: " << (int)lzwCodeLength << std::endl;


		LzwEngine engine(input);

		std::vector<uint8_t> result = engine.decode(lzwCodeLength);
		//std::cout << "should be: " << imageDescriptor.imageWidth * imageDescriptor.imageHeight << std::endl;

		if (imageDescriptor.imageWidth * imageDescriptor.imageHeight != result.size()) {
			//std::cout << "wrong decode" << std::endl;
		}

		Frame frame;

		frame.colorTable = ct;
		frame.imageDescriptor = imageDescriptor;
		frame.indices = result;

		//frame.saveToPgm("lol.pgm");



		return frame;
	}

	void Frame::saveToPgm(const char* filename)
	{/*
		std::ofstream myfile;
		myfile.open(filename);

		myfile << "P2\n";
		myfile << (int)imageDescriptor.imageWidth << " " << (int)imageDescriptor.imageHeight << "\n";

		int rowPos = 0;
		for (auto i : indices) {
			if (++rowPos == imageDescriptor.imageWidth) {
				rowPos = 0;
				myfile << (int)i << "\n";
			}
			else {
				myfile << (int)i << " ";
			}
		}*/
	}

	uint8_t* Frame::getRGB()
	{
		// returns ARGB 
		size_t size = imageDescriptor.imageWidth * imageDescriptor.imageHeight * 4;
		uint8_t *buf = (uint8_t*)malloc(size);

		int i = 0;
		for (auto index : indices) {
			buf[i++] = 255;
			buf[i++] = colorTable->colorTable.at(index).r;
			buf[i++] = colorTable->colorTable.at(index).g;
			buf[i++] = colorTable->colorTable.at(index).b;
		}


		return buf;
	}
	uint8_t* Frame::getIndices()
	{
		size_t size = imageDescriptor.imageWidth * imageDescriptor.imageHeight;
		uint8_t* buf = (uint8_t*)malloc(size);

		memcpy(buf, indices.data(), size);

		return buf;
	}




}

