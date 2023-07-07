#include "Acquisition.h"

void Acquisition::init()
{
    this->state = eState::none;
	this->IR_Buffer.create();
    this->livePL_Buffer.create();
    this->PL_Buffer.create();
}

void Acquisition::setState(eState newState)
{
    this->state = newState;
    cout << "State changed to: [" << (int)newState << "]" << endl;
}

void Acquisition::save(float* data)
{
    switch (this->state)
    {
        case eState::IR:    // Just add image to the buffer
        {
            IR_Buffer.saveImage(data);
            break;
        }
        case eState::LivePL:
        {
            livePL_Buffer.savePL(data);
            break;
        }
        case eState::PL:
        {
            PL_Buffer.savePL(data);
            if (PL_Buffer.getWriteHead() >= PL_Buffer.getAverage())
                this->setState(eState::none);
            break;
        }
        case eState::none:
        {
            break;
        }
    }
}

void Acquisition::reset()
{
    this->IR_Buffer.reset();
    this->livePL_Buffer.reset();
    this->PL_Buffer.reset();
}

void Acquisition::uninit()
{
    this->state = eState::none;
    this->IR_Buffer.destroy();
    this->livePL_Buffer.destroy();
}