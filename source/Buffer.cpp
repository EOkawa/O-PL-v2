#include "Buffer.h"

// *****************************************************************************
// Ring buffer
// *****************************************************************************

void ringBuffer::create()
{
    this->Image.resize(BUFFERSIZE, vector<float>(ROWSIZE * COLUMNSIZE));
    this->writeHead = 0;
}

void ringBuffer::increment()
{
    if (this->writeHead >= BUFFERSIZE - 1) writeHead = 0;
    else ++this->writeHead;
}

void ringBuffer::saveImage(float* data)
{
    mtx.lock();

    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        this->Image[this->writeHead].at(i) = data[i];
    cout << "IR [" << this->writeHead << "] " << this->Image[this->writeHead][0] << endl;

    this->increment();

    mtx.unlock();
}

size_t ringBuffer::getWriteHead()
{
    return this->writeHead;
}

void ringBuffer::reset()
{
    this->writeHead = 0;
    cout << "IR RESET: " << this->writeHead << endl;
}

void ringBuffer::destroy()
{
    this->Image.clear();
    this->writeHead = 0;
}


// *****************************************************************************
// livePL ring buffer
// *****************************************************************************

void livePLBuffer::create()
{
    ringBuffer::create();
    this->tempImage.resize(ROWSIZE * COLUMNSIZE);
    this->alternate = false;
}

void livePLBuffer::savePL(float* data)
{
    mtx.lock(); //This is required because NIT camera is multi-threaded

    if (!this->alternate)
    {
        for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
            this->tempImage.at(i) = data[i];

        this->tempBrightness = calcBrightness(data, 250);
        cout << "TMP " << this->tempImage[0] << ", Brightness " << tempBrightness << endl;

        this->alternate = true;
    }
    else
    {
        if (this->tempBrightness > calcBrightness(data, 250))
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
                this->Image[this->writeHead].at(i) = this->tempImage.at(i) - data[i];
        else
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
                this->Image[this->writeHead].at(i) = data[i] - this->tempImage.at(i);

        cout << "livePL [" << this->writeHead << "] " << this->Image[this->writeHead][0] << endl;

        this->increment();
        this->alternate = false;
    }

    mtx.unlock();
}

void livePLBuffer::reset()
{
    this->alternate = false;
    this->writeHead = 0;
    cout << "LivePL RESET: " << this->alternate << ", " << this->writeHead << endl;
}

void livePLBuffer::destroy()
{
    ringBuffer::destroy();
    this->tempImage.clear();
}

// *****************************************************************************
// livePL ring buffer
// *****************************************************************************

void PLBuffer::create()
{
    ringBuffer::create();
    this->tempImage.resize(ROWSIZE * COLUMNSIZE);
    this->brightImage.resize(BUFFERSIZE, vector<float>(ROWSIZE * COLUMNSIZE));
    this->darkImage.resize(BUFFERSIZE, vector<float>(ROWSIZE * COLUMNSIZE));
    this->writeHead = 0;
    this->alternate = false;
}

void PLBuffer::savePL(float* data)
{
    mtx.lock();

    if (!this->alternate)
    {
        for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
            this->tempImage.at(i) = data[i];

        this->tempBrightness = calcBrightness(data, 250);
        cout << "TMP " << this->tempImage[0] << ", Brightness " << tempBrightness << endl;

        this->alternate = true;
    }
    else
    {
        if (this->tempBrightness > calcBrightness(data, 250))
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
            {
                this->Image[this->writeHead].at(i) = this->tempImage.at(i) - data[i];
                this->brightImage[this->writeHead].at(i) = this->tempImage.at(i);
                this->darkImage[this->writeHead].at(i) = data[i];
            }
        else
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
            {
                this->Image[this->writeHead].at(i) = data[i] - this->tempImage.at(i);
                this->brightImage[this->writeHead].at(i) = data[i]; 
                this->darkImage[this->writeHead].at(i) = this->tempImage.at(i);
            }

        cout << "PL [" << this->writeHead << "] " << this->Image[this->writeHead][0] << endl;

        this->increment();
        this->alternate = false;
    }

    mtx.unlock();
}

void PLBuffer::setAverage(size_t newAverage)
{
    this->average = newAverage;
}

size_t PLBuffer::getAverage()
{
    return this->average;
}

void PLBuffer::reset()
{
    this->alternate = false;
    this->writeHead = 0;
    cout << "PL RESET: " << this->alternate << ", " << this->writeHead << endl;
}

void PLBuffer::destroy()
{
    ringBuffer::destroy();
    this->tempImage.clear();
    this->brightImage.clear();
    this->darkImage.clear();
}