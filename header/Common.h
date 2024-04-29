#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#pragma once

// Constants that are common throughout the program
#define ROWSIZE 1024 //Camera vertical resolution
#define COLUMNSIZE 1280 // Camera horizontal resolution
#define BITDEPTH 65535 //Camera bit depth
#define STREAMBUFFERSIZE 8 //Used for Vido and LivePL
#define PLBUFFERSIZE 40 //Used for PL

#include <iostream>
#include "Log.h"

using namespace std;

// Functions
float calcBrightness(float*, int);

#endif