#ifndef COLORPIPELINE_H_INCLUDED
#define COLORPIPELINE_H_INCLUDED

#include <NITDemosaicing.h>

/** A pipeline is a chain of NITFilter derived classes connected to a NITDevice and ending with a NITObserver        **/
/** For Color cameras, the NITDevice debayer the RAW data from the camera and transmit an RGBA frame to the pipeline **/
/** We don't need further processing                                                                                 **/
void BuildPipeline( NITLibrary::NITDevice* dev, NITLibrary::NITToolBox::NITPlayer& player )
{
    *dev << player;  //The NITPlayer object is connected directly to the NITDevice
}

void BuildPipeline( NITLibrary::NITDevice* dev, NITFilter& user_filter, NITLibrary::NITToolBox::NITPlayer& player, NITLibrary::NITToolBox::NITPlayer& filtered_player )
{
    *dev << player;                          //The NITDevice is connected directly to a player for display
    *dev << user_filter << filtered_player;  //A second pipeline connects the NITDevice to the user filter who is connected to a player for display
}

void RunContinuous( NITLibrary::NITDevice* dev )
{
    //Start the streaming in continuous mode until we call stop
    std::cout << "Capturing a continuous stream activating/deactivating the color filter" << std::endl;
    dev->start();
    ::Sleep( 5000 );
    std::cout << "Deactivating debayering" << std::endl;
    NITLibrary::Demosaicing::activate( dev, false );
    ::Sleep( 5000 );
    std::cout << "Reactivating debayring" << std::endl;
    NITLibrary::Demosaicing::activate( dev, true );
    ::Sleep( 5000 );
    dev->stop();
}

#endif // COLORPIPELINE_H_INCLUDED
