/*******************************************************************************************/
/*                                NIT Camera Driver v2                                     */
/*                                    by Eric Okawa                                        */
/*******************************************************************************************/

#include "dllmain.h"

/** This function gets called for each new frame **/
class ImgAcquired : public NITObserver
{
    void onNewFrame(const NITFrame& frame) {  //Called for each new frame
        acq.save(frame.data());
    }
public:
    ~ImgAcquired()
    {
        systemLog::get().write("MyUserFilter dtor");
        systemLog::get().write("ImgAcquired callback function closed");
    }
};

/** Init function to start up the camera **/
extern "C" DllExport int __cdecl Init(uint8_t logEnabled)
{
    try
    {
        systemLog::get().init(logEnabled > 0);
        systemLog::get().write("Initialising acquisition...");

        dev = CreateDevice();       //Open a connection to the camera and create a NITDevice instance
        ConfigureDevice(dev);       //Set suitable parameters for the current camera

        NITLibrary::ConnectorType connector = dev->connectorType();
        switch (connector)
        {
            case NITLibrary::ConnectorType::USB_2:
                systemLog::get().write("Connector type USB 2.0 detected. Please disconnect and reconnect the USB cable");
                return -2;
            case NITLibrary::ConnectorType::USB_3:
                systemLog::get().write("Connector type USB 3.0 detected");
                break;
            default:
                systemLog::get().write("Unknown connector type detected. Terminating program");
                return -3;
        }

        acq.init();                 //Initialise the acquisition class
        if (dev != NULL) {
            systemLog::get().write("Camera Initialised");
            return 1;
        }
        return -1;
    }
    catch (NITException& exc) {
        systemLog::get().write(exc.what());
        return -1;
    }
}

/** Function stays here until destroyed **/
extern "C" DllExport int __cdecl Run()
{
    try
    {
        /* For debug purposes. It opens an extra window where the image is displayed
            ImgAcquired ImgCopy;                        //A custom filter where image is copied out
            NITPlayer filtered_player("Filtered view"); //A viewer who display the transformed frame
            *dev << ImgCopy << filtered_player;
        */
        systemLog::get().write("Initialising acquisition...");

        ImgAcquired ImgCopy;
        (*dev) << ImgCopy;

        dev->start();                               //Run the capture forever
        dev->waitEndCapture();
    }
    catch (NITException& exc) {
        systemLog::get().write(exc.what());
        return -1;
    }
    return 1;
}

/** Check connection type. Camera should only work on USB3**/
static int CheckConnection(NITDevice* dev)
{
    NITLibrary::ConnectorType connector = dev->connectorType();
    switch (connector)
    {
        case NITLibrary::ConnectorType::USB_2:
            systemLog::get().write("Connector type USB 2.0 detected. Please disconnect and reconnect the USB cable");
            return -2;
        case NITLibrary::ConnectorType::USB_3:
            systemLog::get().write("Connector type USB 3.0 detected");
            return 0;
        default:
            systemLog::get().write("Unknown connector type detected. Terminating program");
            return -3;
      }
}

extern "C" DllExport uint8_t __cdecl getState()
{
    uint8_t state;
    state = (uint8_t)(acq.getState());
    return state;
}

extern "C" DllExport int64_t __cdecl getReadHead() {
    return acq.getReadHead();
}

extern "C" DllExport int32_t __cdecl setState(uint8_t newState)
{
    eState state = static_cast<eState>(newState);

    acq.setState(eState::none);
    Sleep(500);

    switch (state)
    {
        case eState::IR:
        {
            setCamFPS(10); // There is no need for fast acquisition because it is for display only
            acq.setState(eState::IR);
            systemLog::get().write("New state: IR");
            break;
        }
        case eState::LivePL:
        {
            setCamFPS(10); // There is no need for fast acquisition because it is for display only
            acq.setState(eState::LivePL);
            systemLog::get().write("New state: LivePL");
            break;
        }
        case eState::PL:
        {
            acq.reset();
            acq.setState(eState::PL);
            systemLog::get().write("New state: PL");
            break;
        }
        default:
            acq.setState(eState::none);
    }
    return 1;
}

extern "C" DllExport void __cdecl getImage(uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq.getImage(array);
}

extern "C" DllExport int32_t __cdecl getPLReady() 
{
    if (acq.getPLready()) return 1;
    else  return 0;
}
extern "C" DllExport void __cdecl getPL(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq.getPL(bufferNumber, array);
}

extern "C" DllExport void __cdecl getBright(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq.getBright(bufferNumber, array);
}

extern "C" DllExport void __cdecl getDark(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq.getDark(bufferNumber, array);
}

extern "C" DllExport void __cdecl getAveragePL(uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq.getAveragePL(array);
}

extern "C" DllExport double __cdecl getFPS() {
    return (double)(1 / loopTime.count());
}

extern "C" DllExport int32_t __cdecl setPLAverages(size_t newAverages)
{
    acq.setAverage(newAverages);
    return 1;
}

extern "C" DllExport int32_t __cdecl setCamGain(size_t newGain)
{
    try
    {
        if (newGain == 1) dev->setParamValueOf("AnalogGain", "High"); //Set Analog gain H
        else dev->setParamValueOf("AnalogGain", "Low"); //Set Analog gain L
        dev->updateConfig(); //Data is sent to camera

        return 1;
    }
    catch (NITException& exc)
    {
        string err = exc.what();
        systemLog::get().write("NITException" + err);
        return -1;
    }
}

extern "C" DllExport void __cdecl Stop()
{
    systemLog::get().write("Stopping acquisition...");
    acq.setState(eState::none);
    ::Sleep(200);
    dev->stop();                                //Stop the capture
    ::Sleep(200);

    systemLog::get().write("Acquisition stopped");
}

extern "C" DllExport void __cdecl Uninit()
{
    Quit(dev);
    systemLog::get().write("Program terminated");
    systemLog::get().write("#############################################");
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
}

void Stop(NITLibrary::NITDevice* dev)
{
    systemLog::get().write("Stopping acquisition...");
    dev->stop();    // Stop streaming
}

void Quit(NITLibrary::NITDevice* dev)
{
    systemLog::get().write("Uninitialising...");
    ::Sleep(2000);
    acq.uninit();   // Stop acq
}

extern "C" DllExport int32_t __cdecl setCamFPS(double newFPS)
{
    static eState state = (acq.getState());
    static double readValue;

    if (state == eState::PL)
    {
        double min_fps = dev->minFps();
        double max_fps = dev->maxFps();

        if ((newFPS > max_fps) || (newFPS < min_fps)) return -2;

        FPS = newFPS;
        dev->setFps(newFPS);
        dev->updateConfig();

        readValue = dev->fps();
        systemLog::get().write("FPS: " + to_string(readValue));

        if (readValue == newFPS) return 1;
        else return -1;
    }
    else
    {
        if (newFPS > 30) newFPS = 30;

        dev->setFps(newFPS);
        dev->updateConfig();

        readValue = dev->fps();
        systemLog::get().write("FPS: " + to_string(readValue));

        return 1;
    }
    return -1;
}

extern "C" DllExport int32_t __cdecl getCamFPSrange(double &current, double &min_fps, double &max_fps)
{
    min_fps = dev->minFps();
    max_fps = dev->maxFps();
    current = dev->fps();
    return 1;
}

extern "C" DllExport int32_t __cdecl getCamExposure(double& Exposure)
{
    try
    {
        Exposure = dev->paramValueOf("ExposureTime");
        systemLog::get().write("Exposure Time Read: " + to_string(Exposure));
        return 1;
    }
    catch (NITException& exc)
    {
        systemLog::get().write(exc.what());
        return -1;
    }
}

extern "C" DllExport int32_t __cdecl setCamExposure(double newExposure)
{
    double readExposure;

    try
    {       
        dev->setParamValueOf("Exposure Time", newExposure);     //Set Exposure Time
        dev->updateConfig();                                    //Data is sent to camera

        readExposure = dev->paramValueOf("ExposureTime");
        systemLog::get().write("Exposure Time Set: " + to_string(readExposure));

        if (readExposure == newExposure) return 1;
        else return -1;
    }
    catch (NITException& exc)
    {
        systemLog::get().write(exc.what());
        return -1;
    }
}

extern "C" DllExport int32_t __cdecl setCamTrigger(double enable)
{
    double readValue;

    if (enable) dev->setParamValueOf("Trigger Mode", "Output");     //Turn trigger on
    else dev->setParamValueOf("Trigger Mode", "Disabled");   //Turn trigger off
    dev->updateConfig();                                    //Data is sent to camera

    readValue = dev->paramValueOf("Trigger Mode");
    systemLog::get().write("Trigger Mode: " + to_string(readValue));

    return 1;
}