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

#define DllImport extern "C" __declspec( dllimport )
#define DllExport extern "C" __declspec( dllexport )

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
DllExport int32_t __cdecl Init(uint8_t);
DllExport int32_t __cdecl Start();

DllExport uint8_t __cdecl getState();
DllExport int32_t __cdecl setState(uint8_t);

DllExport void __cdecl getImage(uint16_t*, int32_t, int32_t);
DllExport void __cdecl getPL(size_t, uint16_t*, int32_t, int32_t);
DllExport void __cdecl getBright(size_t, uint16_t*, int32_t, int32_t);
DllExport void __cdecl getDark(size_t, uint16_t*, int32_t, int32_t);
DllExport void __cdecl getAveragePL(uint16_t*, int32_t, int32_t);
DllExport int32_t __cdecl getPLReady();

DllExport int64_t __cdecl getReadHead();
DllExport int32_t __cdecl setPLAverages(size_t);
DllExport double __cdecl getFPS();

DllExport void __cdecl Stop();
DllExport void __cdecl Uninit();
/*******************************************************************************************/
// Camera params
DllExport int32_t __cdecl getCamExposure(double&);
DllExport int32_t __cdecl setCamExposure(double);
DllExport int32_t __cdecl setCamGain(size_t);
DllExport int32_t __cdecl setCamFPS(double);
DllExport int32_t __cdecl writeCamFPS(double);
DllExport int32_t __cdecl getCamFPSrange(double&, double&, double&);
DllExport int32_t __cdecl setCamTrigger(uint8_t);
/*******************************************************************************************/
// Global Variables
std::chrono::duration<double> loopTime;		// checks the loop time of the callback function
static bool finished = false;
static double FPS;
/*******************************************************************************************/
// Functions
void Run();
/*******************************************************************************************/
// Classes
static NITDevice* dev;
static Acquisition* acq = new Acquisition();       // Create acq class in heap
/*******************************************************************************************/
#endif // DLLMAIN_H_INCLUDED