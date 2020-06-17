#pragma once
#include "pch.h"
#include "Gif.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Frame.h"
#include "GifHeader.h"
#include "ImageDescriptor.h"
#include "ColorTable.h"

namespace GifLib {

	

	

	Gif Gif::parseGif(std::ifstream& input)
	{
		// Get header first (13 bytes)
		Gif gif;
		GifHeader header = GifHeader::fromStream(input);
		ColorTable globalColorTable;

		header.print();

		// Read global color table if exists
		if (header.hasGlobalColorTable()) {
			globalColorTable = ColorTable::fromStream(input, header.getGlobalColorTableSize());
			//for (size_t i = 0; i < 4; i++) {
			//	globalColorTable.colorTable[i].print();
			//}
			gif.setColorTable(globalColorTable);
		}

		std::vector<Frame> frames;

		uint8_t sentinel;

		while ((sentinel = input.peek()) != 0x3B) {

			// std::cout << "Press Enter to read next block" << std::endl;
			// std::cin.get();

			Frame f;
			switch (sentinel) {
			case 0x2C:
				//std::cout << "Encountered Image Descriptor" << std::endl;
				ImageDescriptor imageDescriptor = ImageDescriptor::fromStream(input);
				f = Frame::parseFrame(input, imageDescriptor, gif.getColorTable());
				f.colorTable = gif.getColorTable();
				//frames.push_back(f);
				std::cout << "Pushed frame..." << std::endl;
				gif.addFrame(f);
				break;

			case 0x21:
				// Extension Introducer

				// Read Extension Label
				uint8_t label;
				input.read((char*)&sentinel, 1);
				input.read((char*)&label, 1);

				// Read Block Size
				uint8_t blockSize;
				input.read((char*)&blockSize, 1);

				//std::cout << "Encountered Extension " << (int)label << std::endl;
				//std::cout << "Skipping " << (int)blockSize << " bytes..." << std::endl;

				// Skip bytes for now, don't read
				input.ignore(blockSize);
				Gif::parseBlocks(input);

				break;
			case 0x3B:
				// GIF File terminator
				std::cout << "Encountered GIF File terminator." << std::endl;
				break;
			default:
				std::cout << "Encountered bad byte: " << (int)sentinel << std::endl;
				
				break;
			}

			uint8_t blockTerminator;
			input.read((char*)&blockTerminator, 1);

			if (blockTerminator != 0) {

				std::cout << "Encountered bad block terminator: " << (int)blockTerminator << std::endl;
				std::cout << "We may have read the block incorrectly..." << std::endl;
			}

		} 
		

		return gif;
	}


	void Gif::parseBlocks(std::ifstream& input) {
		uint8_t blockSize;
		

		while (input.peek() != 0) {

			input.read((char*)&blockSize, 1);

			std::cout << "Another block" << std::endl;
			std::cout << "Skipping " << (int)blockSize << " bytes..." << std::endl;

			// for now discard the data
			input.ignore(blockSize);
		}


	}

	

	Gif::Gif() {
		this->frames = std::vector<Frame>();
	}

	void Gif::addFrame(Frame f) {
		frames.push_back(f);
	}

	ColorTable* Gif::getColorTable() {
		return &globalColorTable;
	}
	Gif::Gif(std::vector<Frame> &frames, ColorTable &colorTable)
	{
		this->frames = frames;
		this->setColorTable(colorTable);
	}

	void Gif::setColorTable(ColorTable colorTable) {
		this->globalColorTable = colorTable;
	}

	int Gif::getFrameCount()
	{
		return this->frames.size();
	}

	std::vector<Frame> Gif::getFrames() {
		return frames;
	}

}

