#pragma once
#include <fstream>
#include <vector>

#define BYTES_PER_COLORMAP_ENTRY	3
#define MAX_DICTIONARY_ENTRY_BITS	12
#define MAX_DICTIONARY_ENTRIES		4096


namespace GifLib {

	typedef uint32_t Color;	// Store color as ARGB
	typedef std::vector<unsigned char> IndexBuffer;

	struct ImageDescriptor
	{
		uint8_t imageSeparator; // Identifies beginning of image descriptor: 0x2C
		uint16_t imagePosLeft;  // image offset from logical screen
		uint16_t imagePosTop;   // image offset from logical screen
		uint16_t imageWidth;
		uint16_t imageHeight;
		uint8_t packedFields;   // Local Color Table Flag   1 Bit
								// Interlace Flag           1 Bit
								// Sort Flag                1 Bit
								// Reserved Bits            2 Bits
								// Local Color Table Size   3 Bits

		bool hasLocalColorTable();
		bool getInterlaceFlag();
		bool isSorted();
		size_t getLocalColorTableSize();

		static ImageDescriptor fromStream(std::ifstream& ifStream);
		void print();
	};


	// List of possible parsing states.
	// TODO: adapt this state list and use it in decoding
	enum class GifParseState{
		GIFType,			// GIF87a or GIF89a
		GIFGlobalHeader,	// LSD, Global color flag
		GIFGlobalColormap,
		GIFImageStart,		// 
		GIFImageHeader,
		GIFImageColormap,
		GIFImageBody,
		GIFLZWStart,
		GIFLZW,				// codesize
		GIFSubBlock,		// 
		GIFExtension,
		GIFControlExtension,
		GIFConsumeBlock,
		GIFSkipBlock,
		GIFDone,
		GIFCommentExtension,
		GIFApplicationExtension,
		GIFNetscapeExtensionBlock,
		GIFConsumeNetscapeExtension,
		GIFConsumeComment
	};


	struct GifGCE
	{
		// Contains parameters used when processing a graphic rendering block
		// The scope of this extension is the first graphic rendering block to follow.
		// The extension contains only one data sub - block.
		// This block is OPTIONAL
		// At most one Graphic Control Extension may precede a graphic rendering block.

		uint8_t packedFields;
		uint16_t delayTime;
		uint8_t transparentColorIndex;

		static GifGCE fromStream(std::ifstream &inputStream);
		void print();

		uint8_t getDisposalMethod();
		bool getUserInputFlag();
		bool getTransparentColorFlag();
	};

	struct GifColorMap {
	public:
		GifColorMap() : defined(false) {};
		const std::vector<Color>& getTable() { return table; };
		void define(std::ifstream& inputStream, int nColors);
		bool isDefined() { return defined; };
	private:
		bool defined;
		std::vector<Color> table;
	};

	struct ImageFrame {
		ImageFrame(unsigned width, unsigned height) : width(width), height(height), data(0) {};
		~ImageFrame();
		unsigned width;
		unsigned height;
		Color* data;

		void print();
	};

	struct GifFrame {
		unsigned xOffset;
		unsigned yOffset;
		unsigned width;
		unsigned height;
		unsigned char transparentIndex;
		unsigned char m_disposalMethod;
		unsigned delay;


	};

	struct LzwContext {

		int lzwsize;	// minimum code size
		int codesize;	// Size of the current code in bits. Max 12
		int codemask;
		int clearcode;	// Code instructing dictionary clear
		int eofcode;	// Code signifying end of data
		int avail;		// Index of next available slot in dictionary
		int oldcode;
		unsigned char firstchar;
		int data;		// 4 byte input buffer
		int bits;		// Number of bits left to read in inBuffer
		int ipass;		// Interlace pass; Ranges 1-4 if interlaced

		IndexBuffer *indexBuffer;

		unsigned short prefix[MAX_DICTIONARY_ENTRIES];
		unsigned char suffix[MAX_DICTIONARY_ENTRIES];
		unsigned short suffixLength[MAX_DICTIONARY_ENTRIES];

		LzwContext();

		void init(int codesize, size_t indexCount);
		bool decode(std::ifstream& inputStream);
	};

	class GifReader
	{

	public:
		GifReader(std::ifstream& inputFileStream);
		~GifReader();

		bool parse();	// Read in header, LSD, global color table

		ImageFrame* getFrame();	// Read in 

		unsigned getWidth() { return imageHeight; };
		unsigned getHeight() { return imageWidth; };

		// TODO: test this; bad code test cases etc
		// make sure if a frame can't be read, no more parsing is attempted
		bool frameAvailable() { return available; };


	private:
		std::ifstream& inputStream;
		GifParseState state;

		bool available;
		bool parseFrame();
		bool renderFrame(unsigned x, unsigned y, unsigned w, unsigned h,
			IndexBuffer &indexBuffer, GifColorMap &colorMap, unsigned char transparentIndex);

		std::vector<Color>* imageBuffer;	// Output buffer of the frame decoding process

		int version;	// 87 for GIF87 or 89 for GIF89

		// Logical Screen Descriptor
		unsigned imageWidth;
		unsigned imageHeight;
		GifColorMap globalColorMap;

	};
}
