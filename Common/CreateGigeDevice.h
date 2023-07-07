#ifndef CREATEGIGEDEVICE_H_INCLUDED
#define CREATEGIGEDEVICE_H_INCLUDED

#include <NITManager.h>
#include <NITGige.h>

NITLibrary::NITDevice* CreateDevice()
{
    using namespace NITLibrary;
    // NITManager is a class instantiated as a singleton which gather informations about the connected cameras.
    // This is called the discovery process.
    NITManager& nm = NITManager::getInstance();

    // We have to indicate the type of cameras we want to be discovered.
    // The function use take as first parameter the connector type used for the discovery.
    // For GIGE, we can also set the number of cameras to discover( default 1) and the time we try to discover cameras(default 15seconds)
    nm.use( GIGE );
    if( nm.deviceCount() == 0 )
    {
        std::cout << "No NIT GIGE camera was discovered" << std::endl;
        return NULL;
    }

    std::cout << "Devices discovered:" << std::endl;
    std::cout << nm.listDevices() << std::endl;

    // Open one of connected device (if exist)
    NITDevice* dev = nm.openOneDevice();	                   //Open the first device detected by the library(index = 0)
    //Alternatives are to call:
    //NITDevice* dev = nm.openDevice( index );                 //Index is the position of the camera in the device list
    //NITDevice* dev = nm.openDeviceBySerialNumber( sn )       //With sn the serial number of the device.
    //For Gige devices we can also call
    //NITDevice* dev = Gige::openByMacAddress( <MacAddress> ); //With MacAddress a string in this form: "70:b3:d5:2a:c0:46"
    //NITDevice* dev = Gige::openByIpAddress( <IpAddress> );   //With IpAddress a string in this form: "192.168.12.247"

    return dev;
}


#endif // CREATEGIGEDEVICE_H_INCLUDED
