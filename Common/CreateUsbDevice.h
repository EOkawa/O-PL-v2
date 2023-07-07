#ifndef CREATEUSBDEVICE_H_INCLUDED
#define CREATEUSBDEVICE_H_INCLUDED

#include <NITManager.h>

NITLibrary::NITDevice* CreateDevice()
{
    using namespace NITLibrary;
    // NITManager is a class instantiated as a singleton which gather informations about the connected cameras.
    // This is called the discovery process.
    NITManager& nm = NITManager::getInstance();

    // We have to indicate the type of cameras we want to be discovered.
    // By default NITManager try to discover USB devices. So nothing as to be done.

    if( nm.deviceCount() == 0 )
    {
        std::cout << "No NIT camera was discovered" << std::endl;
        return NULL;
    }

    std::cout << "Devices discovered:" << std::endl;
    std::cout << nm.listDevices() << std::endl;

    // Open one of connected device (if exist)
    NITDevice* dev = nm.openOneDevice();	//Open the first device detected by the operating system(index = 0)
    //Alternatives are to call:
    //NITDevice* dev = nm.openDevice( index ); //Index is the position of the camera in the device list
    //NITDevice* dev = nm.openDeviceBySerialNumber( sn ) with sn the serial number of the device.

    return dev;
}

#endif // CREATEUSBDEVICE_H_INCLUDED
