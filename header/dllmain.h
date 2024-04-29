#pragma once

#ifndef DLLMAIN_H_INCLUDED
#define DLLMAIN_H_INCLUDED

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h> //For ::Sleep
#include <iostream>
#include <fstream>
#include <thread>
#include "NIT_Cam.h"
#include "acquisition.h"
#include "extcode.h"
#include "buffer.h"
#include "pch.h"
//#include "StdAfx.h"

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    //DLL Main entry point
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

/*******************************************************************************************/
// External function declarations
/*******************************************************************************************/
// DLL params
extern "C" DllExport int32_t __cdecl Init(uint8_t logEnabled);
extern "C" DllExport int32_t __cdecl Run();

extern "C" DllExport uint8_t __cdecl getState();
extern "C" DllExport int32_t __cdecl setState(uint8_t newState);

extern "C" DllExport void __cdecl getImage(uint16_t * array, int32_t array_length_row, int32_t array_length_col);
extern "C" DllExport void __cdecl getPL(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col);
extern "C" DllExport void __cdecl getBright(size_t index, uint16_t * array, int32_t array_length_row, int32_t array_length_col);
extern "C" DllExport void __cdecl getDark(size_t index, uint16_t * array, int32_t array_length_row, int32_t array_length_col);
extern "C" DllExport void __cdecl getAveragePL(uint16_t * array, int32_t array_length_row, int32_t array_length_col);
extern "C" DllExport int32_t __cdecl getPLReady();

extern "C" DllExport int64_t __cdecl getReadHead();
extern "C" DllExport int32_t __cdecl setPLAverages(size_t newAverages);
extern "C" DllExport double __cdecl getFPS();

extern "C" DllExport void __cdecl Stop();
extern "C" DllExport void __cdecl Uninit();
/*******************************************************************************************/
// Camera params
extern "C" DllExport int32_t __cdecl getCamExposure(double& Exposure);
extern "C" DllExport int32_t __cdecl setCamExposure(double newExposure);
extern "C" DllExport int32_t __cdecl setCamGain(size_t newGain);
extern "C" DllExport int32_t __cdecl setCamFPS(double newFPS);
extern "C" DllExport int32_t __cdecl getCamFPSrange(double& current, double &min_fps, double &max_fps);
extern "C" DllExport int32_t __cdecl setCamTrigger(double enable);
/*******************************************************************************************/
// Global Variables
std::chrono::duration<double> loopTime;		// checks the loop time of the callback function
double FPS;
/*******************************************************************************************/
// Classes
static NITDevice* dev;
Acquisition acq;
/*******************************************************************************************/
#endif // DLLMAIN_H_INCLUDED