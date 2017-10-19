#pragma once
#include <opencv2\opencv.hpp>
//#include "aucu3_utils.h"

/**
\brief does texture analysis of \a img, depending on \a textureFlag

\param[in] img matreal containing image
\param[in] textureFlag specifies what to do:\n
0... do nothing\n
1... only remove illumination\n
2... only compute the texture energy\n
3... do both 1 and 2

Before returning, the absolute value of the texture energy is computed and a bilateral filter is applied.

\return matreal where the value of a pixel corresponds to the texture energy of that pixel in \a img.
*/
extern cv::Mat1f aucu3_doTextureAnalysis(const cv::Mat1f & img, const int textureFlag);
