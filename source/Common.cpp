#include "Common.h"

float calcBrightness(float* data, int kernel)
{
    float brightness = 0;  // Reset the brightness sum

    // This version calculates the brightness of the entire image
/*    for (size_t i = 0; i < (ROWSIZE * COLUMNSIZE); ++i) {
        brightness += data[i];
    }
*/
    // This version calculates the brightness of the middle box
    for (size_t i = 0; i < kernel; i++)
    {
        for (size_t j = 0; j < kernel; j++)
        {
            brightness += (float)(*((data + (ROWSIZE / 2) - (kernel / 2) + j) + (i * COLUMNSIZE)));
        }
    }
    
    return brightness;
}