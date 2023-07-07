#ifndef GIGECONFIGOBSERVER_H_INCLUDED
#define GIGECONFIGOBSERVER_H_INCLUDED

#include <NITConfigObserverGige.h>

/** This class permits to track the modifications of the device parameters                                            **/
/** As soon as it is connected to the NITDevice, onNewGroup and onNewPAram is called for each parameter of the device **/
class GigeConfigObserver : public NITLibrary::NITConfigObserverGige
{
    public:
        GigeConfigObserver() : displayNewFrame(false), frameCount(0)
        {
        }

        ~GigeConfigObserver()
    {
        std::cout << "GigeConfigObserver dtor" << std::endl;
    }


        void DisplayNewFrame( bool b )
        {
            frameCount = 0;
            displayNewFrame = b;
        }

    private:

        bool displayNewFrame;
        int frameCount;
        /** Called when the configObserver is connected to the NITDevice  **/
		void     onNewGroup( const char* group_name )
		{
            std::cout << "Group " << group_name << std::endl;
		}
        /** Called when the configObserver is connected to the NITDevice                                            **/
		/** The onNewParam functions provide the group of the parameter and informations about the parameter        **/
		/** All the Gige::xxxParam derives from Gige::Param who provide the name,                                   **/
		/** the visibility(beginner,expert,guru ) of the parameter and also if the parameter is modifiable readable **/
		/** All this functions are called from the thread where the configObserver was connected to the NITDevice   **/
		void     onNewParam( const char* group_name, const Gige::IntParam& param )
		{
		    //IntParam provide also the min,max and step values, and representation of the parameter
            std::cout << "\tInt param " << param.name() << std::endl;
		}
		void     onNewParam( const char* group_name, const Gige::FloatParam& param )
		{
		    //FloatParam provide also the min,max and step values of the parameter
            std::cout << "\tFloat param " << param.name() << std::endl;
		}
		void     onNewParam( const char* group_name, const Gige::EnumParam& param )
		{
 		    //FloatParam provide also the entries of the enumeration
            std::cout << "\tEnum param " << param.name() << std::endl;
		}
		void     onNewParam( const char* group_name, const Gige::BoolParam& param )
		{
            std::cout << "\tBool param " << param.name() << std::endl;
		}
		void     onNewParam( const char* group_name, const Gige::StringParam& param )
		{
            std::cout << "\tString param " << param.name() << std::endl;
		}
		void     onNewParam( const char* group_name, const Gige::CommandParam& param )
		{
            std::cout << "\tCommande param " << param.name() << std::endl;
		}

        /** Called each time a parameter is changed by calling setParamValueOf **/
        /**    for the changed parameter and the dependent parameter if any.    **/
        /** We are in the thread who called setParamValueOf.                    **/
        void onParamChanged(const char *param_name, const char *str_value, float num_value)
        {
            std::cout << "- ConfigObserver: Parameter " << param_name << " changed to " << str_value << std::endl;
        }

        /** Called each time a parameter change imply a range change            **/
        /** We are in the thread who called setParamValueOf.                    **/
        void onParamRangeChanged(const char *param_name, const char *str_values[], const float *num_values, unsigned int array_size,
                                                                                            const char *cur_str_val, float cur_num_val)
        {
            if( array_size == 0 )
                std::cout << "- ConfigObserver: " << param_name << " new Range [empty] value set to " << cur_str_val << std::endl;
            else if( array_size == 1 )
                std::cout << "- ConfigObserver: " << param_name << " new Range [" << str_values[0] << "] value set to " << cur_str_val << std::endl;
            else
                std::cout << "- ConfigObserver: " << param_name << " new Range [" << str_values[0] << "," << str_values[array_size-1] << "] value set to "
                                                                                                                                << cur_str_val << std::endl;

            //If you uncomment the code snippet below you will have the list of all possible values of the parameter in
            //      the current camera configuration.
            //For some parameters like Exposure Time, the list can be very large.
            /*
            for (int i = 0; i < array_size; i++)
            {
                std::cout << str_values[i] << " ; ";
            }
            std::cout << "Value set to " << cur_str_val << std::endl;
            */
        }

        /** Called each time a parameter change imply a change in the range of possible frame rates                        **/
        /** If the frame rate at the time of the call is higher or lower than the new fps range, the frame rate is adapted **/
        /** We are in the thread who called setParamValueOf.                                                               **/
        void onFpsRangeChanged(double new_fpsMin, double new_fpsMax, double new_fps)
        {
            std::cout << "- ConfigObserver: New FPS Range : " << new_fpsMin << " - " << new_fpsMax << std::endl;
            std::cout << "- ConfigObserver: FPS set to " << new_fps << std::endl;
        }

        /** Called when frame rate is changed                                   **/
        /** We are in the thread who called setParamValueOf.                    **/
        void onFpsChanged(double new_fps)
        {
            std::cout << "- ConfigObserver: FPS set to " << new_fps << std::endl;
        }

        /** Called for each received frame                                  **/
        /** status is 0 if the frame is Ok and transmitted to the pipeline  **/
        /** If status is not 0, the frame is dropped                        **/
        /** WE ARE NOT IN THE MAIN THREAD.                                  **/
        void onNewFrame(int status)
        {
            //An output to cout is done only if displayNewFrame as been set to true by calling DisplayNewFrame(true)
            if( displayNewFrame )
                std::cout << "- ConfigObserver: Frame " << ++frameCount << " " << status << std::endl;
        }

        /** Non Uniformity Correction(NUC) is applied to frames from SWIR cameras **/
        /** Called each time a parameter change imply a NUC change                **/
        /** We are in the thread who called setParamValueOf.                      **/
        void onNucChanged(const char* nuc_str, int status)
        {
            std::cout << "- ConfigObserver: Nuc Changed " << nuc_str << std::endl;
        }

        /** Called when an error occurs in an internal thread               **/
        /** WE ARE NOT IN THE MAIN THREAD.                                  **/
        void onInternalError(const NITException &exc)
        {
            std::cout << "- ConfigObserver: Internal Error " << exc.what() << std::endl;
        }

};

#endif // USBCONFIGOBSERVER_H_INCLUDED
