#pragma once
#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include "ImageDescriptor.h"
#include "ColorTable.h"

namespace GifLib {


	struct Frame
	{
		// Each image in the Data Stream is composed of an Image Descriptor,
		// an optional Local Color Table, and the image data.

		
		std::vector<uint8_t> indices;
		ImageDescriptor imageDescriptor;
		ColorTable *colorTable;

		void saveToPgm(const char * filename);
		uint8_t* getRGB();
		uint8_t* getIndices();

		static Frame parseFrame(std::ifstream& input,
			ImageDescriptor imageDescriptor,
			ColorTable *globalColorTable);

	};



}

