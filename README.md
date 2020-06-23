# gif-lib

Simple one-header GIF decoding library.
Built with pure C++ with no external dependencies.
The aim of the project was to learn about the GIF file format, LZW encoding and to write a quick and easy to use library capable of decoding GIF images. 

## Features

- Parses GIF image files
- Extracts each consecutive frame when `GifReader::getFrame()` is called
- Frame data as a struct containing width, height and a `uint32_t` data array (ARGB format, A=255)

## Usage

`GifReader` is initialised with an `std::ifstream` object.
The header, LSD and global color table must first be parsed using `GifReader::parse()`
Calling `GifReader::getFrame()` when `GifReader::frameAvailable()` is false results in undefined behaviour

```cpp
#include <fstream>
int main()
{
  using namespace GifLib;
  
  std::ifstream input("C:/sample.gif", std::ios::binary); // must include std::ios::binary
  
  GifReader reader(input);  // initialise new GifReader using ifstream
  
  reader.parse(); // parse the header, LSD and global colour table 
  
  ImageFrame* img;
  
  while (reader.frameAvailable()) { // While there are frames left in the file
  
    // the ImageFrame struct holds frame data in the ARGB format (uint32_t*)
    img = reader.getFrame(); // parse, decode the next frame
    
    // img->width;
    // img->height;
    // img->data;
  }
}
```

## TODO:

- Implement gif encoding
- Alter parser to behave more like a state machine using the existing enum of possible parsing states `enum GifParseState`
- Test the method `GifReader::frameAvailable()` further.
- Optimise:
  - Reading color table - use stack or heap?
  - Rendering the next frame onto the GIF image buffer
- Implement interlacing support for rendering
- Implement methods for handling extensions (netscape, comment, plain text)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Lzw decoding routine based off [original Netscape code](https://chromium.googlesource.com/chromium/blink.git/+/master/Source/platform/image-decoders/gif/) (later Mozilla.org, and then used by Chromium)
