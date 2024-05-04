#ifndef ACQUISITION_H_INCLUDED
#define ACQUISITION_H_INCLUDED
#pragma once

#include <Windows.h>
#include "Common.h"
#include "tiffImage.h"
#include "Buffer.h"
#include "Log.h"

// State of the acquisition
enum class eState : std::uint8_t
{
	none = 0,
	IR = 1,
	LivePL = 2,
	PL = 3
};

enum class plState : std::uint8_t
{
	idle = 0,
	tempWriting = 1,
	tempCompleted = 2,
	plWriting = 3,
	plCompleted
};

/*******************************************************************************************/
// Main acquisition class. This class manages the acquisition state and distributes
// work to the correct buffers
/*******************************************************************************************/
class Acquisition
{
	private:
		// Variable declarations
		eState state;					// current state of the acquisition
		ringBuffer IR_Buffer;			// IR image ringbuffer
		livePLBuffer livePL_Buffer;		// livePL image ringbuffer
		PLBuffer PL_Buffer;				// PL image ringbuffer
		vector<float> FF;				// vector that stores the FF
		size_t average;					// number of PL images to average
		size_t BufferNumber;			// keeps track of the incoming images
		bool FF_valid;					// boolean indicating if FF is valid
		bool completed;

		// Method declatations
		int loadFF();

	public:
		Acquisition() :					// Constructor -> init values
			state(eState::none),
			average(5),
			BufferNumber(0),
			FF_valid(false),
			completed(false)
		{}

		~Acquisition()					// Destructor
		{}

		// Method declarations
		void init();
		eState getState() const;
		int64_t getReadHead();
		void getImage(uint16_t*);
		int32_t getPL(const size_t, uint16_t*);
		int32_t getBright(const size_t, uint16_t*);
		int32_t getDark(const size_t, uint16_t*);
		int32_t getAveragePL(uint16_t*);
		bool getPLready() const;
		void save(float*);
		void setState(const eState);
		void setAverage(const size_t);
		void reset();
		void uninit();
};

#endif
