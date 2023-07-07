#pragma once

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <vector>
#include <mutex>
#include "Common.h"

class ringBuffer
{
protected:
        vector<vector<float>> Image;
        size_t writeHead;
        mutex mtx;

        void increment();

    public:
        ringBuffer() :
            writeHead(0)
        {}

        ~ringBuffer() {
            Image.clear();
        }

        // Method declarations
        void create();
        void saveImage(float*);
        size_t getWriteHead();
        void reset();
        void destroy();
};

class livePLBuffer : public ringBuffer
{
    private:
        vector<float> tempImage;
        float tempBrightness;
        bool alternate;

    public:
        livePLBuffer() :
            alternate(false),
            tempBrightness(0)
        {}

        ~livePLBuffer() {
        }

        void create();
        void savePL(float*);
        void reset();
        void destroy();
};

class PLBuffer : public ringBuffer
{
    private:
        vector<float> tempImage;
        vector<vector<float>> brightImage;
        vector<vector<float>> darkImage;
        float tempBrightness;
        bool alternate;
        size_t average;
    public:
        PLBuffer() :
            alternate(false),
            tempBrightness(0),
            average(10)
        {}

        ~PLBuffer() {
        }

        void create();
        void savePL(float*);
        void setAverage(size_t);
        size_t getAverage();
        void reset();
        void destroy();
};

#endif