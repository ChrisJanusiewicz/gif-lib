#pragma once
#include "pch.h"
#include <string>
#include <vector>
#include <iostream>
#include "Frame.h"


namespace GifLib {

    
    class Frame;
    class Gif {
    public:

        Gif();
        Gif(std::vector<Frame> &frames, ColorTable &colorTable);

        int getFrameCount();
        void setColorTable(ColorTable colorTable);
        ColorTable* getColorTable();
        std::vector<Frame> getFrames();

        void addFrame(Frame f);


        static void parseBlocks(std::ifstream& input);
        static Gif parseGif(std::ifstream& input);
        
    private:
        ColorTable globalColorTable;
        std::vector<Frame> frames;
    };





}