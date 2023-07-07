#ifndef CONFIGUREWIDYSWIR640VS_H_INCLUDED
#define CONFIGUREWIDYSWIR640VS_H_INCLUDED


/** This sample configuration is specific to WiDySWIR 640-VS cameras                **/
/*                                                                                   */
/* SWIR cameras need to have a Non Uniformity Correction( NUC ) to show best results */
/* NUC files can be found in the USB key provided with your camera in a folder named */
/*      SNxxxx with xxxx the serial number of your camera                            */
/* Copy this SNxxx folder in the working directory of this application               */
/* When the NITDevice object is constructed, he looks for the presence of this folder*/
/*      enable automatically the Nuc processing                                      */
void ConfigureDevice( NITLibrary::NITDevice* dev )
{
    using namespace std;

    /* Parameters can be set by the function setParamValueOf                   */
    /* Parameters can be read by the functions paramValueOf or paramStrValueOf */
    /* The below code show you some use cases                                  */
    /* Each time a parameter is changed, the ConfigObserver is called          */

		//We can query the parameters of the camera in two ways
		//Query as string value
		cout << "Mode : " << dev->paramStrValueOf( "MODE" ) << endl;                //Note the presence "Str"
		//Query as double
		cout << "Exposure Time : " << dev->paramValueOf( "ExposureTime" ) << endl; //Note the absence of "Str"
		//Naturally a parameter who have a string representation should be queried as string (Ex: "Mode")
		//     if you call this kind of parameter as double, you will have the index value of the value in the list.
		//     Example for Mode: if the available modes are 'Global Shutter' and 'Gated' the returned value will be
		//     0.0 for 'Global shutter' and 1.0 for 'Gated'.

		//You can set the parameter values by string or by number( if number, the type must be precise ).
		//Parameters can be chained.
        //    You can write:
        //       dev->setParamValueOf("Param1", value1);
        //       dev->setParamValueOf("Param2", value2);
        //       dev->updateConfig();
        //    Or
        //       dev->setParamValueOf("Param1", value1).setParamValueOf("Param2", value2).updateConfig();
        //
		//Parameters are sent to the camera only when updateConfig is called
		unsigned int sensor_width = dev->sensorWidth();
		unsigned int sensor_height = dev->sensorHeight();
		dev->setParamValueOf( "First Column", 0u );               //Note the strict typing of the value
		dev->setParamValueOf( "NumberOfColumns", sensor_width );  //Note the strict typing of the value
		//dev->setParamValueOf( "NumberOfColumns", 640 );         //This line cannot be compiled error would ambiguous type for value
		dev->setParamValueOf( "First Line", 0u );                 //Set Y position of begin of frame
		dev->setParamValueOf( "Number of Lines", sensor_height ); //Set frame height
		dev->setParamValueOf( "Pixel Clock", "25MHz" );           //Set clock frequency
		dev->setParamValueOf( "Mode", "Global Shutter" );         //Set Mode
        dev->setParamValueOf( "Exposure Time", 5000.0 );          //Set Exposure Time
		dev->updateConfig();                                      //Data is sent to the device

        //If a parameter name is unknown or a parameter value is out of range, a NITExcpetion will be thrown
		//If you uncomment the line below a NITException will be thrown and catched at the end of the current try block
		//dev->setParamValueOf("Unknown Parameter", 40 );


		// Fps configuration: set fps to a mean value
		// The min and max values of the fps change with the width,height,mode,pixelClock and exposure time.
		// If fpsMin or fpsMax change and the current fps is out of range, the fps is automatically set to the nearest in range value(minFps or maxFps)
		double min_fps = dev->minFps();
		double max_fps = dev->maxFps();
		cout << min_fps << " <= " << dev->fps() << " <= " << max_fps << endl;

		dev->setFps((min_fps + max_fps) / 2 );
		dev->updateConfig();                             //Data is sent to the device param 'true'
}

#define USE_USB
#define NEED_AGC

#endif // CONFIGUREWIDYSWIR640VS_H_INCLUDED
