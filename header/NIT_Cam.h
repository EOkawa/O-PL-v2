#pragma once

#ifndef NIT_CAM_H_INCLUDED
#define NIT_CAM_H_INCLUDED

#include <Windows.h>                     //For ::Sleep
#include <iostream>
#include <NITManager.h>
#include <NITDevice.h>
#include <NITAutomaticGainControl.h>
#include <NITPlayer.h>
#include <NITFrame.h>

#include "Common.h"
#include "CameraSelector.h"
#include "Acquisition.h"

#ifndef CAMERA_MODEL
#error you must define CAMERA_MODEL in CameraSelector.h.
#endif // CAMERA_MODEL

using namespace std;
using namespace NITLibrary;
using namespace NITLibrary::NITToolBox;  //For the filters and observer

#ifdef USE_GIGE

#include "Common/CreateGigeDevice.h"
#include "Common/GigeConfigObserver.h"
#define CONFIG_OBSERVER GigeConfigObserver

#include "Common/SimplePipeline.h"

#elif defined(USE_USB)

#include "CreateUsbDevice.h"
#include "UsbConfigObserver.h"
#define CONFIG_OBSERVER UsbConfigObserver

#ifdef NEED_AGC   //Some cameras need an adaptation of the RAW data outputted by the camera to be displayed
#include "AgcPipeline.h"
#else
#include "ColorPipeline.h"

#endif // NEED_AGC
#endif // defined(USE_USB)
#endif // NIT_CAM_H_INCLUDED