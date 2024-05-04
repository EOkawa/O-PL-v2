#include "Acquisition.h"

MLTiffW tiffWrite;						// handles tiff writes
MLTiffR tiffRead;                       // handles tiff reads

Acquisition::Acquisition() :		    // Connstructor -> init values
    state(eState::none),
    average(5),
    BufferNumber(0),
    FF_valid(false),
    completed(false)
{
    this->FF.resize((ROWSIZE * COLUMNSIZE), 0);
    this->loadFF();
    this->IR_Buffer.create();
    this->livePL_Buffer.create();
    this->PL_Buffer.create();
}

Acquisition::~Acquisition()				// Destructor
{
    this->state = eState::none;
    this->IR_Buffer.destroy();
    this->livePL_Buffer.destroy();
    this->PL_Buffer.destroy();
}

int Acquisition::loadFF()
{
    // Default FF = 1
    for (int i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        this->FF[i] = 1;

    // Open file
    int e = tiffRead.open("FF.tif");
    if (e != 1) {
        systemLog::get().write("Unable to open Flatfield file.Unity Flatfield will be used");
        tiffRead.close();
        return e;
    }

    // Read image
    uint16_t* temp_flatfield = static_cast<uint16_t*>(malloc(ROWSIZE * COLUMNSIZE * sizeof(uint16_t)));
    e = tiffRead.readU16(0, temp_flatfield);
    tiffRead.close();

    if ((e == 1) || (temp_flatfield != NULL))
    {
        systemLog::get().write("Flatfield file FF.tif opened successfully");
        this->FF_valid = true;
        uint16_t max_value = *max_element(temp_flatfield, temp_flatfield + (1080 * 1024));
        systemLog::get().write("Flatfield file max value: " + to_string(max_value) + ". Flatfield will be normalised");

        for (int i = 0; i < ROWSIZE * COLUMNSIZE; ++i)
        {
            if (temp_flatfield[i] == 0) {
                systemLog::get().write("Invalid FF");
                this->FF_valid = false; // If there is at least one bad number, set FF to 1
                return -1;
            }
            else
                this->FF[i] = (float)(temp_flatfield[i]) / (float)max_value;
        }
    }
    free(temp_flatfield);
    temp_flatfield = NULL;
    return e;
}

void Acquisition::setState(const eState newState) {
    this->BufferNumber = 0;
    this->completed = false;
    PL_Buffer.reset();
    this->state = newState;
}

eState Acquisition::getState() const {
    return this->state;
}

int64_t Acquisition::getReadHead()
{
    if (this->state == eState::PL) return PL_Buffer.getReadHead();
    return 0;
}

void Acquisition::save(float* data) 
{
    // This function is called from multiple threads. Need to make sure they don't write to the same memory location
    switch (this->state)
    {
        case eState::IR:
            IR_Buffer.saveImage(data, ++this->BufferNumber, this->FF);
            break;
        case eState::LivePL:
            livePL_Buffer.savePL(data, ++this->BufferNumber);
            break;
        case eState::PL:
            if (++this->BufferNumber <= this->average * 2) PL_Buffer.savePL(data, this->BufferNumber);
            else { 
                this->state = eState::none; 
                this->completed = true;
                systemLog::get().write("PL acquisition complete!");
            }
            break;
        case eState::none:
            break;
    }
}

void Acquisition::getImage(uint16_t* array)
{
    switch (this->state)
    {
        case eState::IR:
            IR_Buffer.readImage(array);
            break;
        case eState::LivePL:
            livePL_Buffer.readPL(array);
            break;
        default:
            break;
    }
}

int32_t Acquisition::getPL(const size_t bufferNumber, uint16_t* array)
{
    if ((this->state == eState::none) && (this->completed)) {
        PL_Buffer.readPL(bufferNumber, array, this->FF);
        return 1;
    }
    else return -1;
}

int32_t Acquisition::getBright(const size_t bufferNumber, uint16_t* array)
{
    if ((this->state == eState::none) && (this->completed)) {
        PL_Buffer.readBright(bufferNumber, array, this->FF);
        return 1;
    }
    else return -1;
}

int32_t Acquisition::getDark(const size_t bufferNumber, uint16_t* array)
{
    if ((this->state == eState::none) && (this->completed)) {
        PL_Buffer.readDark(bufferNumber, array, this->FF);
        return 1;
    }
    else return -1;
}

int32_t Acquisition::getAveragePL(uint16_t* array)
{
    if ((this->state == eState::none) && (this->completed)) {
        PL_Buffer.readAveragePL(array, this->FF, this->average);
        return 1;
    }
    else return -1;
}

void Acquisition::setAverage(const size_t newAverage) {
    this->average = newAverage;
}

bool Acquisition::getPLready() const {
    return this->completed;
}

void Acquisition::reset() {
    this->PL_Buffer.reset();
}