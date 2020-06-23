#include "pch.h"
#include "GifReader.h"

#include <iostream>


#define IMAGE_DESCRIPTOR_LABEL	0x2C
#define EXTENSION_INTRODUCER	0x21
#define GRAPHIC_CONTROL_LABEL	0xF9
#define GIF_EOF					0x3B


#define PACK_ARGB(R, G, B) 0xFF000000 | R << 16 | G << 8 | B

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

	uint8_t GifGCE::getDisposalMethod()
	{
		return (packedFields & 0b00011100) >> 2;
	}

	bool GifGCE::getUserInputFlag()
	{
		return packedFields & 0b00000010;
	}

	bool GifGCE::getTransparentColorFlag()
	{
		return packedFields & 0b00000001;
	}

	GifGCE GifGCE::fromStream(std::ifstream &inputStream) {
		GifGCE gce;
		inputStream.read((char*)&gce.packedFields, 1);
		inputStream.read((char*)&gce.delayTime, 2);
		inputStream.read((char*)&gce.transparentColorIndex, 1);
		return gce;
	}

	void GifGCE::print()
	{
		std::cout << "Delay time:\t" << (int)delayTime << std::endl;
		std::cout << "User Input:\t" << (int)getUserInputFlag() << std::endl;
		std::cout << "Disposal:\t" << (int)getDisposalMethod() << std::endl;
		std::cout << "Transparent flag:\t" << (int)getTransparentColorFlag() << std::endl;
		std::cout << "Transparent index:\t" << (int)transparentColorIndex << std::endl;
	}

	ImageFrame::~ImageFrame() {
		free(data);
	}

	void ImageFrame::print()
	{
		std::cout << "Image frame: {" << width << ", " << height << "}" << std::endl;
	}

	void GifColorMap::define(std::ifstream &inputStream, int nColors) {
		table.resize(nColors);

		size_t len = 3 * (size_t)nColors;
		// TODO: optimise somehow
		// read directly into vector data and shuffle?

		// Allocate memory to hold gif color data
		// TODO: should this be on the stack or heap?
		// malloc vs new vs unsigned char[]
		unsigned char* buff = (unsigned char*)malloc(len);
		inputStream.read((char*)buff, len);


		unsigned char* currData = buff;
		Color currColor;
		std::vector<Color>::iterator it = table.begin();

		for (int i = 0; i < nColors; i++) {
			currColor = PACK_ARGB(currData[0], currData[1], currData[2]);
			*it++ = currColor;
			currData += 3;
		}

		free(buff);
		defined = true;
	}

	GifReader::~GifReader() {
		delete(imageBuffer);
	}

	GifReader::GifReader(std::ifstream& inputFileStream) :
		inputStream(inputFileStream),
		state(GifParseState::GIFType),
		imageBuffer(0),
		imageHeight(0),
		imageWidth(0),
		available(false),
		version(0)//, globalColorMap()
	{
	}


	bool GifReader::parse()
	{
		// Get header first (13 bytes)
		GifHeader header = GifHeader::fromStream(inputStream);

		imageWidth = header.width;
		imageHeight = header.height;

		// TODO: implementbounds checking
		size_t len = imageWidth * imageHeight;

		// allocate memory for image buffer (ARGB)
		imageBuffer = new std::vector<Color>(len);

		uint32_t test = PACK_ARGB((unsigned char)0xBB, (unsigned char)0xCC, (unsigned char)0xFF);

		header.print();

		// Read global color table if exists
		if (header.hasGlobalColorTable()) {
			globalColorMap.define(inputStream, header.getGlobalColorTableSize());
		}
		available = true;



		return true;
	}

	bool GifReader::renderFrame(unsigned x, unsigned y, unsigned w, unsigned h,
		IndexBuffer &indexBuffer, GifColorMap &colorMap, unsigned char transparentIndex) {

		//TODO: implement interlacing

		std::vector<Color>::iterator rowBuffer;
		int rowOffset;
		int index;
		IndexBuffer::iterator indexIter = indexBuffer.begin();

		//for each scanline
		for (int iy = 0; iy < h; iy++) {

			// Set pointer to the correct offset at the desired scanline
			int offset = ((y + iy) * imageWidth) + x ;
			rowBuffer = imageBuffer->begin() + offset;

			for (int ix = 0; ix < w; ix++) {
				if ((index = *indexIter++) != transparentIndex) {
					*rowBuffer = colorMap.getTable()[index];
				}
				rowBuffer++;

			}
		}		

		return false;
	}

	// parses gif blocks until a frame is encountered and decoded into image buffer
	bool GifReader::parseFrame() {

		uint8_t sentinel;
		LzwContext *lzwContext = new LzwContext;

		ImageDescriptor imageDescriptor;
		GifColorMap localColorTable;
		GifGCE gce;

		uint8_t blockTerminator;
		int codesize = 0;

		// TODO: use state machine for decoding like mozilla/chromium

		while ((sentinel = inputStream.peek()) != EOF) {


			switch (sentinel) {
			case IMAGE_DESCRIPTOR_LABEL	:

				imageDescriptor = ImageDescriptor::fromStream(inputStream);
				std::cout << imageDescriptor.hasLocalColorTable() << std::endl;

				if (imageDescriptor.hasLocalColorTable()) {
					localColorTable.define(inputStream, imageDescriptor.getLocalColorTableSize());
				}


				inputStream.read((char*)&codesize, 1);
				// Read minimum codesize

				// Image data follows
				// Initialise decoder

				// TODO: bounds checking
				lzwContext->init(codesize, imageDescriptor.imageWidth * imageDescriptor.imageHeight);
				lzwContext->decode(inputStream);

				renderFrame(imageDescriptor.imagePosLeft,
					imageDescriptor.imagePosTop,
					imageDescriptor.imageWidth,
					imageDescriptor.imageHeight,
					*(lzwContext->indexBuffer),
					imageDescriptor.hasLocalColorTable() ? localColorTable : globalColorMap,
					gce.transparentColorIndex);
				 
				inputStream.read((char*)&blockTerminator, 1);
				return true;
				break;

			case EXTENSION_INTRODUCER:
				// Read Extension Label
				uint8_t label;
				inputStream.read((char*)&sentinel, 1);
				inputStream.read((char*)&label, 1);

				// TODO: impelment methods for handling other extensions (netscape, comment)

				uint8_t blockSize;
				switch (label) {
				case GRAPHIC_CONTROL_LABEL:
					inputStream.read((char*)&label, 1);
					std::cout << "Expecting size (4): " << (int)label << std::endl;

					gce = GifGCE::fromStream(inputStream);
					//gce.print();

					inputStream.read((char*)&label, 1);
					std::cout << "Expecting block terminator (0): " << (int)label << std::endl;
					break;
				default:
					while (blockSize = inputStream.get()) {
						inputStream.ignore(blockSize);
					}
					break;
				}
				// block terminator read by above loop
				
				break;
			case GIF_EOF:
				// GIF File terminator
				//std::cout << "Encountered GIF File terminator" << std::endl;
				available = false;
				return false;
				break;
			default:
				std::cout << "Encountered bad byte: " << (int)sentinel << std::endl;
				return false;
				break;
			}

		}

		return false;
	}


	ImageFrame* GifReader::getFrame() {

		available = parseFrame();

		ImageFrame* imgFrame = new ImageFrame(imageWidth, imageHeight);
		imgFrame->data = imageBuffer->data();

		return imgFrame;
	}
 
	LzwContext::LzwContext() 
	{
		// TODO: initliase
		indexBuffer = new IndexBuffer(); // initialise on heap
	}


	void LzwContext::init(int lzwsize, size_t indexCount)
	{
		this->lzwsize = lzwsize;
		indexBuffer->resize(indexCount);

		clearcode = 1 << lzwsize;
		eofcode = clearcode + 1;
		avail = eofcode + 1;
		oldcode = -1;
		codesize = lzwsize + 1;
		codemask = (1 << codesize) - 1;
		data = 0;
		bits = 0;
		ipass = 0;

		for (int i = 0; i < clearcode; ++i) {
			suffix[i] = i;
			suffixLength[i] = 1;
		}
	}

	bool LzwContext::decode(std::ifstream &inputStream)
	{

		unsigned char bytesInBlock = 0;
		unsigned char buffer[256];
		unsigned char* nextByte;


		std::vector<unsigned char>::iterator iter = indexBuffer->begin();

		// loop while there are still blocks to read

		while (inputStream.peek() != 0) {
			//read next block
			inputStream.read((char*)&bytesInBlock, 1);
			inputStream.read((char*)buffer, bytesInBlock);
			nextByte = buffer;
			while (bytesInBlock > 0) {
				// refill 32-bit input buffer using next byte from data buffer
				int b = ((int)*nextByte++);
				data += b << bits;
				bits += 8;
				bytesInBlock--;

				// while there are enough bits left in input buffer to decode next index
				while (bits >= codesize) {
					int code = data & codemask;	
					data >>= codesize;
					bits -= codesize;

					if (code == clearcode) {
						// reset dictionary to original state
						codesize = lzwsize + 1;
						codemask = (1 << codesize) - 1;
						avail = eofcode + 1;
						oldcode = -1;
						continue;
					}

					if (code == (eofcode)) {
						// end-of-stream should only appear after the expected amount of data is parsed
						if (iter == indexBuffer->end()) {
							return true;
						}
						return false;
					}

					// real decoding starts here

					const int tempCode = code;
					unsigned short codeLength = 0;
					if (code < avail) {
						// Code has already been seen
						codeLength = suffixLength[code];
						iter += codeLength;
					}
					else if (code == avail && oldcode != -1) {
						// New code just being added to the dictionary
						// One code longer than previous code
						// extra code at the end: first character of last code
						codeLength = suffixLength[oldcode] + 1;
						iter += codeLength;
						*--iter = firstchar;
						code = oldcode;
					}
					else {
						// Invalid code; stop pasing
						return false;
					}


					while (code >= clearcode) {
						*--iter = suffix[code];
						code = prefix[code];
					}
					*--iter = firstchar = suffix[code];

					// Only define new dictionary entry if there EXISTS a previous code
					if (avail < MAX_DICTIONARY_ENTRIES && oldcode != -1) {
						prefix[avail] = oldcode;
						suffix[avail] = firstchar;
						suffixLength[avail] = suffixLength[oldcode] + 1;
						++avail;
						// Increment codesize once we reach maximum value that can be encoded
						// codesize cannot grow larger than MAX_DICTIONARY_ENTRIES (12 for gifs)
						if (!(avail & codemask) && avail < MAX_DICTIONARY_ENTRIES) {
							++codesize;
							codemask += avail;
						}
					}

					oldcode = tempCode;
					iter += codeLength;

				
				}

			}

		}

		return false;
	}

}