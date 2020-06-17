#include "pch.h"
#include "GraphicControlExtension.h"

namespace GifLib {
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


}