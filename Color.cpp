#include "pch.h"
#include "Color.h"
#include <iostream>
 
namespace GifLib {
	Color::Color(uint8_t r, uint8_t g, uint8_t b) {
		this->r = r;
		this->g = g;
		this->b = b;
	};

	Color Color::fromRGB(uint8_t r, uint8_t g, uint8_t b)
	{
		return Color(r, g, b);
	}

	void Color::print() {
		std::cout << "{" << (int)r << ", " << (int)g << ", " << (int)b << "}" << std::endl;
	}

}