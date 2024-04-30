#include "Buffer.h"

// *****************************************************************************
// Ring buffer
// *****************************************************************************

void ringBuffer::create() {
    this->Image.resize(STREAMBUFFERSIZE, vector<uint16_t>(ROWSIZE * COLUMNSIZE));
}

void ringBuffer::saveImage(float* data, size_t buffer, vector<float>& FF)
{
    size_t writeHead = (buffer - 1) % STREAMBUFFERSIZE;
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        this->Image[writeHead].at(i) = (uint16_t)(data[i] * BITDEPTH / FF[i]);
    
    this->readHead = writeHead;

    systemLog::get().write("writeHead " + to_string(writeHead));
}

void ringBuffer::readImage(uint16_t* destination)
{
    if (this->readHead >= 0)
    {
        for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
            destination[i] = this->Image[this->readHead].at(i);

        systemLog::get().write("readHead " + to_string(this->readHead));
    }
}

int64_t ringBuffer::getReadHead() {
    return this->readHead;
}

void ringBuffer::reset() {
    this->readHead = -1;
}

void ringBuffer::destroy() {
    this->Image.clear();
}

// *****************************************************************************
// livePL ring buffer
// *****************************************************************************

void livePLBuffer::create()
{
    ringBuffer::create();
    this->tempImage.resize(ROWSIZE * COLUMNSIZE);
}

void livePLBuffer::savePL(float* data, size_t buffer)
{
    size_t writeHead = (buffer - 1) % STREAMBUFFERSIZE; // Increment writeHead;

    if (buffer % 2 == 1)
    {
        if (!this->writing && !this->copying) // Wait until writing and copying stops
        {
            this->writing = true;
            
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
                this->tempImage.at(i) = data[i] * BITDEPTH;

            this->tempBrightness = calcBrightness(data, 250);
            this->writing = false;
        }
    }
    else
    {
        this->copying = true; // Stop writing from starting
        
        while (writing) ::Sleep(1); // Wait for writing to finish

        if (this->tempBrightness > calcBrightness(data, 250))
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
                this->Image[writeHead].at(i) = (uint16_t)max((float)0, (this->tempImage.at(i) - (data[i] * BITDEPTH)));
        else
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
                this->Image[writeHead].at(i) = (uint16_t)max((float)0, (data[i] * BITDEPTH) - this->tempImage.at(i));

        this->copying = false;
        this->readHead = writeHead; // Increment readHead 
        
        systemLog::get().write("writeHead " + to_string(writeHead));
    }
}

void livePLBuffer::readPL(uint16_t* destination) 
{
    if (this->readHead >= 0)
    {
        while (this->copying) ::Sleep(1);

        for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
            destination[i] = this->Image[this->readHead].at(i);

        systemLog::get().write("readHead " + to_string(this->readHead));
    }
}

void livePLBuffer::destroy()
{
    ringBuffer::destroy();
    this->tempImage.clear();
}

// *****************************************************************************
// PL buffer
// *****************************************************************************

void PLBuffer::create()
{
    this->tempImage.resize(ROWSIZE * COLUMNSIZE);
    this->image1.resize(PLBUFFERSIZE, vector<uint16_t>(ROWSIZE * COLUMNSIZE));
    this->image2.resize(PLBUFFERSIZE, vector<uint16_t>(ROWSIZE * COLUMNSIZE));
}

void PLBuffer::savePL(float* data, size_t buffer)
{
    vector<uint16_t>* ptr;

    size_t writeHead = (buffer - 1) / 2;

    if (buffer % 2 == 1) ptr = &this->image1[writeHead];
    else ptr = &this->image2[writeHead];

    systemLog::get().write("Writing to image1 " + to_string(writeHead));
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
        ptr->at(i) = (uint16_t)(data[i] * BITDEPTH);

    if (buffer % 2 == 0) {
        this->readHead = writeHead; // Increment readHead 
        systemLog::get().write("readHead: " + to_string(this->readHead));
    }
}

void PLBuffer::readPL(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{
    bool brightFirst = (calcBrightness(this->image1[0], 250) > calcBrightness(this->image2[0], 250));

    vector<uint16_t>* ptrBright;
    vector<uint16_t>* ptrDark;

    if (brightFirst) {
        ptrBright = &this->image1[bufferNumber];
        ptrDark = &this->image2[bufferNumber];
    }
    else {
        ptrBright = &this->image2[bufferNumber];
        ptrDark = &this->image1[bufferNumber];
    }

    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = (uint16_t)(((float)ptrBright->at(i) - (float)ptrDark->at(i)) / FF.at(i));
}

void PLBuffer::readBright(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{
    vector<uint16_t>* ptr;

    if (calcBrightness(this->image1[bufferNumber], 250) > calcBrightness(this->image2[bufferNumber], 250)) 
        ptr = &this->image1[bufferNumber];
    else
        ptr = &this->image2[bufferNumber];

    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
            destination[i] = ptr->at(i) / FF.at(i);
}

void PLBuffer::readDark(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{
    vector<uint16_t>* ptr;

    if (calcBrightness(this->image1[bufferNumber], 250) < calcBrightness(this->image2[bufferNumber], 250))
        ptr = &this->image1[bufferNumber];
    else
        ptr = &this->image2[bufferNumber];

    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = ptr->at(i) / FF.at(i);
}

void PLBuffer::readAveragePL(uint16_t* destination, vector<float>& FF, size_t average)
{
    bool brightFirst = (calcBrightness(this->image1[0], 250) > calcBrightness(this->image2[0], 250));
    
    vector<uint16_t>* ptrBright;
    vector<uint16_t>* ptrDark;
    
    if (brightFirst) {
        ptrBright = &this->image1[0];
        ptrDark = &this->image2[0];
    }
    else {
        ptrBright = &this->image2[0];
        ptrDark = &this->image1[0];
    }

    //Copy the first image to average
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        this->tempImage.at(i) = max((float)0, ((float)ptrBright->at(i) - (float)ptrDark->at(i)));

    // Add subsequent images
    for (size_t i = 1; i < average; ++i) 
    {
        if (brightFirst) {
            ptrBright = &this->image1[i];
            ptrDark = &this->image2[i];
        }
        else {
            ptrBright = &this->image2[i];
            ptrDark = &this->image1[i];
        }

        for (size_t j = 0; j < ROWSIZE * COLUMNSIZE; ++j)
            this->tempImage.at(i) = max((float)0, ((float)ptrBright->at(j) - (float)ptrDark->at(j)));
    }

    // Divide by the number of averages
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = (uint16_t)((this->tempImage.at(i) / average) / FF.at(i));
}

void PLBuffer::destroy()
{
    ringBuffer::destroy();
    this->tempImage.clear();
    this->image1.clear();
    this->image2.clear();
}