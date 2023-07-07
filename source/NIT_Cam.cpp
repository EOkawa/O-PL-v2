/*******************************************************************************************/
/*                                NIT Camera Driver v2                                     */
/*                                    by Eric Okawa                                        */
/*******************************************************************************************/

#include "NIT_Cam.h"

/** This function gets called for each new frame **/
class ImgAcquired : public NITObserver
{
    void onNewFrame(const NITFrame& frame) //Called for each new frame
    {
        acq.save(frame.data());
    }
public:
    ~ImgAcquired() 
    {
        std::cout << "MyUserFilter dtor" << std::endl;
        syslog.write("ImgAcquired callback function closed");
    }
};

int main()
{
    CONFIG_OBSERVER config_observer;        //An object who track and display the modifications of the device

    try
    {
        syslog.init(true);                  //Initialising log file

        dev = CreateDevice();               //Open a connection to the camera and create a NITDevice instance
        int connection = CheckConnection(dev);
        if (connection > 0) return connection;
            
        (*dev) << config_observer;          //Connect a NITConfigObserver derived class to the NITDevice.
                                            //As soon as the config observer is connected, the camera transmit his current state.
                                            //For more info see UsbConfigObserver.h, GigeConfigObserver.h and the library documentation.

        ConfigureDevice(dev);               //Set suitable parameters for the current camera

        acq.init();

        ImgAcquired ImgCopy;                //An observer where image is copied out
        *dev << ImgCopy;

        Start(dev);                         //Begins capture by calling start(), ends capture when stop() is called.

        int k = 0;
        while (1)
        {
            acq.reset();

            /** Start capture **/
            switch (k)
            {
                case 0:
                {
                    acq.setState(eState::IR);
                    syslog.write("New state: IR");
                    k = 1;
                    break;
                }
                case 1:
                {
                    acq.setState(eState::LivePL);
                    syslog.write("New state: LivePL");
                    k = 2;
                    break;
                }
                case 2:
                {
                    acq.setState(eState::PL);
                    syslog.write("New state: PL");
                    k = 0;
                    break;
                }
            }
            ::Sleep(10000);
        }
        Quit(dev);

    }
    catch (NITException& exc)
    {
        std::cout << "NITException: " << exc.what() << std::endl;
    }
    return 0;
}

/*
int Init(uint8_t logEnabled)
{
    try
    {
        syslog.init(logEnabled > 0);
        syslog.write("Initialising acquisition...");

        dev = CreateDevice();       //Open a connection to the camera and create a NITDevice instance
        ConfigureDevice(dev);       //Set suitable parameters for the current camera


        acq.Init();                 //Initialise the acquisition class
        if (dev != NULL) {
            syslog.write("Camera Initialised");
            return 1;
        }
        return -1;
    }
    catch (NITException& exc) {
        syslog.write(exc.what());
        return -1;
    }
}
*/

/** Check connection **/
int CheckConnection(NITDevice* dev)
{
    NITLibrary::ConnectorType connector = dev->connectorType();
    switch (connector)
    {
    case NITLibrary::ConnectorType::USB_2:
        syslog.write("Connector type USB 2.0 detected. Please disconnect and reconnect the USB cable");
        cout << "Connector type USB 2.0 detected. Please disconnect and reconnect the USB cable\n";
        return -2;
    case NITLibrary::ConnectorType::USB_3:
        syslog.write("Connector type USB 3.0 detected");
        cout << "Connector type USB 3.0 detected\n";
        return 0;
    default:
        syslog.write("Unknown connector type detected. Terminating program");
        cout << "Unknown connector type detected. Terminating program\n";
        return -3;
    }
}

void Start(NITLibrary::NITDevice* dev)
{
    //Start the streaming in continuous mode until we call stop
    std::cout << "Capturing a continuous stream activating/deactivating the agc filter" << std::endl;
    syslog.write("Initialising acquisition...");
    dev->start();
}
void Stop(NITLibrary::NITDevice* dev)
{
    syslog.write("Stopping acquisition...");
    dev->stop();    // Stop streaming
}
void Quit(NITLibrary::NITDevice* dev)
{
    syslog.write("Uninitialising...");
    acq.uninit();   // Stop acq
    dev->stop();    // Stop streaming
    syslog.write("#############################################\n");
}