
#include "pch.h"
#include "LzwEngine.h"





std::vector<uint8_t>  LzwEngine::encode()
{
    /*
    std::vector<uint8_t> data;

    std::vector<uint16_t> codeStream;

    size_t c = 0;

    auto next = data.begin() + 1;

    std::vector<uint8_t> current(*data.begin());
    while (next != data.end()) {
        uint16_t code;
        std::map<std::vector<uint8_t>, uint16_t>::iterator entry;

        if ((entry = dict.find(current)) != dict.end()) {
            code = (*entry).second;
        }
        else {

        }


        std::cout << current.data() << " " << *next << std::endl;
    }*/


    return std::vector<uint8_t>();
}

LzwEngine::LzwEngine(std::ifstream &ifStream) : bitStream(ifStream)
{
    dict = std::vector<std::vector<uint8_t>>(MAX_DICT_SIZE);
}

LzwEngine::LzwEngine(std::ofstream &ofStream) : bitStream(ofStream)
{
}

void LzwEngine::clearDictionary(int size)
{

    int maxEntry = (1 << (size));
    for (size_t i = 0; i < maxEntry; i++) {
        dict.data()[i] = std::vector<uint8_t>{(uint8_t)i};
    }
    signalCLEAR = maxEntry;
    signalEOF = maxEntry + 1;

    nextCode = maxEntry + 2;
    codeSize = size + 1;
}

void LzwEngine::addDictEntry(std::vector<uint8_t> value) {
    //std::cout << "Current code size: " << codeSize << std::endl;
    //std::cout << "Maximum code: " << (1 << codeSize) - 1 << std::endl;


    //std::cout << "Added new code: " << dictPos << " - ";


    dict.data()[nextCode++] = value;

    if (nextCode == (1 << codeSize) && !dictFull()) {
        //if we just output the biggest possible code, increase codeSize
        codeSize++;
        if (codeSize > 12) {
            std::cout << "shouldn't get to 13" << std::endl;
        }
        //std::cout << "DictPos: " << nextCode << " Increasing code size to: " << codeSize << std::endl;
    }


}

bool LzwEngine::dictFull()
{
    return nextCode >= MAX_DICT_SIZE;
}

std::vector<uint8_t> LzwEngine::decode(int minLzwCodeSize)
{
    bool eof = false;

    if (minLzwCodeSize > 8) {
        std::cout << "Something is wrong" << std::endl;
    }

    clearDictionary(minLzwCodeSize);

    std::vector<uint8_t> indexStream;
    codeSize = minLzwCodeSize + 1;

    //std::cout << "CLEAR: " << (int)signalCLEAR << std::endl;
    //std::cout << "EOF: " << (int)signalEOF << std::endl;
    

    int code, prevCode;
    std::vector<uint8_t> indices, prevIndices;

    // Read clear
    code = (int)bitStream.read(codeSize);


    // read first code 
    code = (int)bitStream.read(codeSize);

    // insert first index into table
    indices = dict.data()[code];
    indexStream.insert(indexStream.end(), indices.begin(), indices.end());
    
    int iCode = 1;

    while (!eof) {
        iCode++;
        prevIndices = indices;
        prevCode = code;

        code = (int)bitStream.read(codeSize);
        indices.clear();

        ///std::cout << "Read code: " << code << std::endl;

        if (code == signalEOF) {
            //std::cout << "Encountered EOF " << iCode << std::endl;
            //std::cout << "index stream length: " << indexStream.size() << std::endl;

            eof = true;
        }
        else if (code == signalCLEAR) {
            //std::cout << "Encountered CLEAR " << " dict size: " << nextCode << " " << iCode << std::endl;
            clearDictionary(minLzwCodeSize);

            // read first code 
            code = (int)bitStream.read(codeSize);
            //std::cout << "First code: " << code << std::endl;

            // insert first index into table
            indices = dict.data()[code];
            indexStream.insert(indexStream.end(), indices.begin(), indices.end());
        }
        else if (!dictFull()) {
            
            if (code >= nextCode) {
                //std::cout << "Could not find code in dict" << std::endl;
                indices.insert(indices.end(), prevIndices.begin(), prevIndices.end());
                indices.push_back(*prevIndices.begin());
                addDictEntry(indices);

                indexStream.insert(indexStream.end(), indices.begin(), indices.end());
            }
            else {
                indices = dict.data()[code];

                indexStream.insert(indexStream.end(), indices.begin(), indices.end());

                prevIndices.push_back(*indices.begin());
                addDictEntry(prevIndices);

            }
        }
        else {
            //std::cout << "Dict full! code: " << code << " loop " << iCode << std::endl;
            indices = dict.data()[code];
            indexStream.insert(indexStream.end(), indices.begin(), indices.end());
        }

    }


    return indexStream;



}
