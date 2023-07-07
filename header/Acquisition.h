#pragma once

#ifndef ACQUISITION_H_INCLUDED
#define ACQUISITION_H_INCLUDED

#include "Common.h"
#include "Buffer.h"

// State of the acquisition
enum class eState : std::uint8_t
{
	none = 0,
	IR = 1,
	LivePL = 2,
	PL = 3
};

class Acquisition
{
	private:
		// Variable declarations
		eState state;							// current state of the acquisition
		ringBuffer IR_Buffer;					// IR image ringbuffer
		livePLBuffer livePL_Buffer;				// livePL image ringbuffer
		PLBuffer PL_Buffer;						// PL image ringbuffer

	public:
		Acquisition() :
			state(eState::none)
		{}

		~Acquisition()
		{}

	// Method declarations
		void init();
		void setState(eState);
		void save(float*);
		void reset();
		void uninit();
};

#endif
