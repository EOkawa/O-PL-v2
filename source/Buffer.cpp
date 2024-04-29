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
    this->brightImage.resize(PLBUFFERSIZE, vector<uint16_t>(ROWSIZE * COLUMNSIZE));
    this->darkImage.resize(PLBUFFERSIZE, vector<uint16_t>(ROWSIZE * COLUMNSIZE));
}

void PLBuffer::savePL(float* data, size_t buffer)
{
    if (buffer%2 == 1)
    {
        systemLog::get().write("Writing to temp buffer " + to_string(buffer/2));
        for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++)
            this->tempImage.at(i) = data[i] * BITDEPTH;
        this->tempBrightness = calcBrightness(data, 250);
    }
    else
    {
        size_t writeHead = (buffer/2) - 1;
        systemLog::get().write("Starting to copy " + to_string(writeHead));

        if (this->tempBrightness > calcBrightness(data, 250)) {
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++) {
                this->brightImage[writeHead].at(i) = (uint16_t)(this->tempImage.at(i));
                this->darkImage[writeHead].at(i) = (uint16_t)(data[i] * BITDEPTH);
            }
        }
        else {
            for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; i++) {
                this->brightImage[writeHead].at(i) = (uint16_t)(data[i] * BITDEPTH);
                this->darkImage[writeHead].at(i) = (uint16_t)(this->tempImage.at(i));
            }
        }
        this->readHead = writeHead; // Increment readHead 
        systemLog::get().write("Copied writeHead " + to_string(buffer - 1) + ", readHead: " + to_string(this->readHead));
    }
}

void PLBuffer::readPL(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{ 
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = ((float)this->brightImage[bufferNumber].at(i) - (float)this->darkImage[bufferNumber].at(i)) / FF.at(i);
}

void PLBuffer::readBright(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = this->brightImage[bufferNumber].at(i) / FF.at(i);
}

void PLBuffer::readDark(size_t bufferNumber, uint16_t* destination, vector<float>& FF)
{
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = this->darkImage[bufferNumber].at(i) / FF.at(i);
}

void PLBuffer::readAveragePL(uint16_t* destination, vector<float>& FF, size_t average)
{
    //Copy the first image to average
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        this->tempImage.at(i) = max( (float)0, ((float)this->brightImage[0].at(i) - (float)this->darkImage[0].at(i)) );

    // Add subsequent images
    for (size_t i = 1; i < average; ++i) {
        for (size_t j = 0; j < ROWSIZE * COLUMNSIZE; ++j)
            this->tempImage.at(i) = max((float)0, ((float)this->brightImage[i].at(j) - (float)this->darkImage[i].at(j)));
    }

    // Divide by the number of averages
    for (size_t i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        destination[i] = (uint16_t)((this->tempImage.at(i) / average) / FF.at(i));

    cout << destination[100] << endl;

}

void PLBuffer::destroy()
{
    ringBuffer::destroy();
    this->tempImage.clear();
    this->brightImage.clear();
    this->darkImage.clear();
}