/*******************************************************************************************/
/*                                NIT Camera Driver v2                                     */
/*                                    by Eric Okawa                                        */
/*******************************************************************************************/

#include "dllmain.h"

/** This function gets called for each new frame **/
class ImgAcquired : public NITObserver
{
    void onNewFrame(const NITFrame& frame) {  //Called for each new frame
        acq->save(frame.data());
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

        dev = CreateDevice();           //Open a connection to the camera and create a NITDevice instance
        
        if (dev == NULL) 
        {
            systemLog::get().write("Unable to connect to the camera. MCheck USB connection");
            return -1;                  // -1 means that there are no devices connected
        }
        ConfigureDevice(dev);           //Set suitable parameters for the current camera

        NITLibrary::ConnectorType connector = dev->connectorType(); // Make sure camera is recognised as USB3
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

        systemLog::get().write("Camera Initialised");
        return 1;
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

extern "C" DllExport uint8_t __cdecl getState()
{
    uint8_t state;
    state = (uint8_t)(acq->getState());
    return state;
}

extern "C" DllExport int64_t __cdecl getReadHead() {
    return acq->getReadHead();
}

extern "C" DllExport int32_t __cdecl setState(uint8_t newState)
{
    eState state = static_cast<eState>(newState);

    acq->setState(eState::none);
    Sleep(250);

    switch (state)
    {
        case eState::IR:
            setCamFPS(10); // There is no need for fast acquisition because it is for display only
            setCamTrigger(0); // Set trigger off
            Sleep(250);
            acq->setState(eState::IR);
            systemLog::get().write("New state: IR");
            break;
        case eState::LivePL:
            setCamFPS(10); // There is no need for fast acquisition because it is for display only
            setCamTrigger(1); // Set trigger on
            Sleep(250);
            acq->setState(eState::LivePL);
            systemLog::get().write("New state: LivePL");
            break;
        case eState::PL:
            acq->reset();
            setCamFPS(FPS); // Back to set acquisition speed
            setCamTrigger(1); // Set trigger on
            Sleep(250);
            acq->setState(eState::PL);
            systemLog::get().write("New state: PL");
            break;
        default:
            acq->setState(eState::none);
    }
    return 1;
}

extern "C" DllExport void __cdecl getImage(uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq->getImage(array);
}

extern "C" DllExport int32_t __cdecl getPLReady() 
{
    if (acq->getPLready()) return 1;
    else  return 0;
}
extern "C" DllExport void __cdecl getPL(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq->getPL(bufferNumber, array);
}

extern "C" DllExport void __cdecl getBright(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq->getBright(bufferNumber, array);
}

extern "C" DllExport void __cdecl getDark(size_t bufferNumber, uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq->getDark(bufferNumber, array);
}

extern "C" DllExport void __cdecl getAveragePL(uint16_t * array, int32_t array_length_row, int32_t array_length_col) {
    acq->getAveragePL(array);
}

extern "C" DllExport double __cdecl getFPS() {
    return (double)(1 / loopTime.count());
}

extern "C" DllExport int32_t __cdecl setPLAverages(size_t newAverages)
{
    acq->setAverage(newAverages);
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

extern "C" DllExport int32_t __cdecl setCamFPS(double newFPS)
{
    static eState state = (acq->getState());
    static double readValue;

    double min_fps = dev->minFps();
    double max_fps = dev->maxFps();

    if ((newFPS > max_fps) || (newFPS < min_fps))
    {   
        readValue = dev->fps();
        systemLog::get().write("Unable to set FPS. Current FPS: " + to_string(readValue) + ". min: " + to_string(min_fps) + ", max: " + to_string(max_fps));

        return -2;
    }
    dev->setFps(newFPS);
    dev->updateConfig();

    readValue = dev->fps();
    systemLog::get().write("FPS: " + to_string(readValue));

    if (readValue == newFPS) return 1;
    else return -1;
}

extern "C" DllExport int32_t __cdecl writeCamFPS(double newFPS)
{
    systemLog::get().write("Saving FPS Value: " + to_string(newFPS));
    FPS = newFPS; // Just store value
    return 1;
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

extern "C" DllExport int32_t __cdecl setCamTrigger(uint8_t enable)
{
    try
    {
        string writeValue;
        string readValue;

        if (enable) writeValue = "Output";
        else writeValue = "Disabled";
        systemLog::get().write("Changing Trigger Mode to: " + writeValue);

        dev->setParamValueOf("Trigger Mode", writeValue);   //Turn trigger on or off    
        dev->updateConfig();                                //Data is sent to camera

        readValue = dev->paramStrValueOf("Trigger Mode");   //Get the rows as string value
        systemLog::get().write("Trigger Mode: " + readValue);

        if (readValue == writeValue) return 1;
        else return -1;
    }
    catch (NITException& exc)
    {
        systemLog::get().write(exc.what());
        return -1;
    }
}

extern "C" DllExport void __cdecl Stop()
{
    systemLog::get().write("Stopping acquisition...");
    acq->setState(eState::none);
    ::Sleep(200);
    dev->stop();                                //Stop the capture
    ::Sleep(200);

    systemLog::get().write("Acquisition stopped");
}

extern "C" DllExport void __cdecl Uninit()
{
    setCamTrigger(0);                           // Set trigger off
    systemLog::get().write("Uninitialising...");
    ::Sleep(1000);
    delete acq;                                 // delete acq class from heap
    systemLog::get().write("Program terminated");
    systemLog::get().write("#############################################");
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
}

