#ifndef SIMPLEPIPELINE_H_INCLUDED
#define SIMPLEPIPELINE_H_INCLUDED

//#include "Demosaicing.h"

/** A pipeline is a chain of NITFilter derived classes connected to a NITDevice and ending with a NITObserver        **/
/** For Gige cameras, the camera can output frames directly displayable                                              **/
/** We don't need further processing                                                                                 **/
void BuildPipeline( NITLibrary::NITDevice* dev, NITLibrary::NITToolBox::NITPlayer& player )
{
    *dev << player;  //The NITPlayer object is connected directly to the NITDevice
}

void BuildPipeline( NITLibrary::NITDevice* dev, NITFilter& user_filter, NITLibrary::NITToolBox::NITPlayer& player, NITLibrary::NITToolBox::NITPlayer& filtered_player )
{
    *dev << player;                          //The NITPlayer object is connected directly to the NITDevice 
    *dev << user_filter << filtered_player;  //A second pipeline connects the NITDevice to the user filter who is connected to a player for display
}

void RunContinuous( NITLibrary::NITDevice* dev )
{
    //Start the streaming in continuous mode until we call stop
    std::cout << "Capturing a continuous stream" << std::endl;
    dev->start();
    ::Sleep( 15000 );
    dev->stop();
}

#endif // SIMPLEPIPELINE_H_INCLUDED
