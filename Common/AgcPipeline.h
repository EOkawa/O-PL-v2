#ifndef AGCPIPELINE_H_INCLUDED
#define AGCPIPELINE_H_INCLUDED

/** A pipeline is a chain of NITFilter derived classes connected to a NITDevice and ending with a NITObserver        **/
/** For USB gray cameras, the RAW output of the camera need to be adapted for display                                **/
/** We apply a NITAutomaticGainControl filter who transform the RAW data frame to a format suitable for display      **/
NITLibrary::NITToolBox::NITAutomaticGainControl agc;

void BuildPipeline( NITLibrary::NITDevice* dev, NITLibrary::NITToolBox::NITPlayer& player )
{
    *dev << agc << player;  //The NITDevice is connected to the agc filter who is connected to the player for display
}

void BuildPipeline( NITLibrary::NITDevice* dev, NITFilter& user_filter, NITLibrary::NITToolBox::NITPlayer& player, NITLibrary::NITToolBox::NITPlayer& filtered_player)
{
    *dev << agc << player;					//The NITDevice is connected to the agc filter and to a player
	    agc << user_filter << filtered_player;  //a second pipeline connects agc to a user filter who is connected to a player for display
}

void RunContinuous( NITLibrary::NITDevice* dev )
{
    //Start the streaming in continuous mode until we call stop
    std::cout << "Capturing a continuous stream activating/deactivating the agc filter" << std::endl;
    dev->start();
    ::Sleep( 5000 );
    std::cout << "Deactivating agc" << std::endl;
    agc.activate( false );
    ::Sleep( 5000 );
    std::cout << "Reactivating agc" << std::endl;
    agc.activate( true );
    ::Sleep( 5000 );
    dev->stop();
}

#endif // AGCPIPELINE_H_INCLUDED
