#pragma once
#include "pch.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>
#include "BitStream.h"


class LzwEngine
{


public:
    LzwEngine(std::ifstream &ifStream);
    LzwEngine(std::ofstream &ofStream);

    std::vector<uint8_t> decode(int minLzwCodeSize);
    std::vector<uint8_t> encode();

private:
    void clearDictionary(int size);
    void addDictEntry(std::vector<uint8_t> value);


    int codeSize;
    int nextCode;
    const int MAX_CODE_SIZE = 12;
    const int MAX_DICT_SIZE = 4096;
    uint16_t signalCLEAR;
    uint16_t signalEOF;
    BitStream bitStream;
     
    bool dictFull();
    std::map<std::vector<uint8_t>, uint16_t> dict2;
    std::vector<std::vector<uint8_t>> dict;


};

/*
dictionary: array of string(equivalent to : map of CodeType to string)
S : empty string
k : CodeType
DF : the data file
EF : the encoded file

dictionary = entries for all strings consisting of a single byte

while (could read a new CodeType from EF into k)
{
    if (k > dictionary size)
        cannot decode!
    else
        if (k == dictionary size) // special case
            dictionary[next unused code] = S + S[0]
        else
            if (S isn't empty )
                dictionary[next unused code] = S + dictionary[k][0]

                write dictionary[k] to DF
                S = dictionary[k]*/