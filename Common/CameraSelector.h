#ifndef CAMERASELECTOR_H_INCLUDED
#define CAMERASELECTOR_H_INCLUDED

#define WIDySWIR_640VS      1
#define WiDySWIR_S320VS     2
#define WiDySWIR_320VS      3
#define WiDySenS_640VS      4
#define SenS_640VS          5
#define MC1003              6
#define MC1003_COLOR        7
#define WiDySwir_640GSE     8
#define WiDySenS_640GSTE    9
#define MC1003_1GB         10
#define MC1003_1GF         11

#define SenS_1280VS        12

/** Undefine the line below which correspond to your camera model **/
// USB
//#define CAMERA_MODEL WIDySWIR_640VS     //NSC1201
//#define CAMERA_MODEL WiDySWIR_S320VS    //NSC1201 Cropped
//#define CAMERA_MODEL WiDySWIR_320VS     //NSC1401
//#define CAMERA_MODEL WiDySenS_640VS     //NSC1601
//#define CAMERA_MODEL SenS_640VS         //NSC1902
//#define CAMERA_MODEL MC1003             //MC1003 Gray
//#define CAMERA_MODEL MC1003_COLOR       //MC1003 Color
#define CAMERA_MODEL SenS_1280VS        //NSC1901

// GIGE
//#define CAMERA_MODEL WiDySwir_640GSE    //NSC1201Gige
//#define CAMERA_MODEL WiDySenS_640GSTE   //NSC1601Gige
//#define CAMERA_MODEL MC1003_1GB         //NSC1003GIGE
//#define CAMERA_MODEL MC1003_1GF         //NSC1003cGIGE


#if CAMERA_MODEL == WIDySWIR_640VS        //NSC1201
    #include "ConfigureWiDySwir640VS.h"
#elif CAMERA_MODEL == WiDySWIR_S320VS     //NSC1201 Cropped
    #include "ConfigureWiDySwir640VS.h"
#elif CAMERA_MODEL == WiDySWIR_320VS      //NSC1401
    #include "ConfigureWiDySwir320VS.h"
#elif CAMERA_MODEL == WiDySenS_640VS      //NSC1601
    #include "ConfigureWiDySenS.h"
#elif CAMERA_MODEL == SenS_640VS          //NSC1902
    #include "ConfigureSenS.h"
#elif CAMERA_MODEL == SenS_1280VS         //NSC1901
    #include "ConfigureSenS.h"
#elif CAMERA_MODEL == MC1003              //MC1003 Gray
    #include "ConfigureMC1003.h"
//    #include "AgcPipeline.h"
#elif CAMERA_MODEL == MC1003_COLOR        //MC1003 Color
    #include "ConfigureMC1003.h"
#elif CAMERA_MODEL == WiDySwir_640GSE     //NSC1201Gige
    #include "ConfigureWiDySwir640GSE.h"
#elif CAMERA_MODEL == WiDySenS_640GSTE    //NSC1601Gige
    #include "ConfigureWiDySenS640GSTE.h"
#elif CAMERA_MODEL == MC1003_1GB          //NSC1003GIGE
    #include "ConfigureMC1003GB.h"
#elif CAMERA_MODEL == MC1003_1GF          //NSC1003cGIGE
    #include "ConfigureMC1003GF.h"
#endif // CAMERA_MODEL

#endif // CAMERASELECTOR_H_INCLUDED
