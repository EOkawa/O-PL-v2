#pragma once

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <vector>
#include <mutex>
#include <algorithm>
#include "Common.h"

using namespace std;

/*******************************************************************************************/
// Buffer classes. ringBuffer parent class has the basic memory buffer and the children
// classes are more specialised (LivePL and PL). They retain the images acquired by the
// camera and perform image processing tasks such as subtract 2 images to create PL
// images and apply FF
/*******************************************************************************************/
class ringBuffer
{
    protected:
        vector<vector<uint16_t>> Image;
        int64_t readHead;
        mutex mtx;

    public:
        ringBuffer() :
            readHead(-1)
        {}

        ~ringBuffer() {
            Image.clear();
        }

        // Method declarations
        void create();
        void saveImage(float*, size_t, vector<float>&);
        void readImage(uint16_t*);
        void reset();
        int64_t getReadHead();
        void destroy();
};

class livePLBuffer : public ringBuffer
{
    private:
        vector<float> tempImage;
        uint16_t tempBrightness;
        bool writing;
        bool copying;

    public:
        livePLBuffer() :
            tempBrightness(0),
            writing(false),
            copying(false)
        {}

        ~livePLBuffer() {
        }

        void create();
        void savePL(float*, size_t);
        void readPL(uint16_t*);
        void destroy();
};

class PLBuffer : public ringBuffer
{
    private:
        vector<float> tempImage;
        vector<vector<uint16_t>> brightImage;
        vector<vector<uint16_t>> darkImage;
        float tempBrightness;
    public:
        PLBuffer() :
            tempBrightness(0)
        {}

        ~PLBuffer() {
        }

        void create();
        void savePL(float*, size_t);
        void readPL(size_t, uint16_t*, vector<float>&);
        void readBright(size_t, uint16_t*, vector<float>&);
        void readDark(size_t, uint16_t*, vector<float>&);
        void readAveragePL(uint16_t*, vector<float>&, size_t);
        void destroy();
};

#endif