#pragma once

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <vector>
#include <mutex>
#include <algorithm>
#include "Common.h"

/*******************************************************************************************/
// Buffer classes. ringBuffer parent class has the basic memory buffer and the children
// classes are more specialised (LivePL and PL). They retain the images acquired by the
// camera and perform image processing tasks such as subtract 2 images to create PL
// images and apply FF
/*******************************************************************************************/
class ringBuffer
{
    protected:
        std::vector<std::vector<uint16_t>> Image;
        int64_t readHead;
        std::mutex mtx;

    public:
        ringBuffer() :
            readHead(-1)
        {}

        ~ringBuffer() {
            Image.clear();
        }

        // Method declarations
        void create();
        void saveImage(float*, const size_t, std::vector<float>&);
        void readImage(uint16_t*) const;
        void reset();
        int64_t getReadHead();
        void destroy();
};

class livePLBuffer : public ringBuffer
{
    private:
        std::vector<float> tempImage;
        uint16_t tempBrightness;
        bool writing;
        bool copying;

    public:
        livePLBuffer() :
            tempBrightness(0),
            writing(false),
            copying(false)
        {}

        ~livePLBuffer() 
        {}

        void create();
        void savePL(float*, const size_t);
        void readPL(uint16_t*) const;
        void destroy();
};

class PLBuffer : public ringBuffer
{
    private:
        std::vector<float> tempImage;
        std::vector<std::vector<uint16_t>> image1; // Odd images
        std::vector<std::vector<uint16_t>> image2; // Even images
        float tempBrightness;
    public:
        PLBuffer() :
            tempBrightness(0)
        {}

        ~PLBuffer() {
        }

        void create();
        void savePL(float*, const size_t);
        void readPL(const size_t, uint16_t*, std::vector<float>&);
        void readBright(const size_t, uint16_t*, std::vector<float>&);
        void readDark(const size_t, uint16_t*, std::vector<float>&);
        void readAveragePL(uint16_t*, std::vector<float>&, const size_t);
        void destroy();
};

#endif