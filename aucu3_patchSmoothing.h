#pragma once
#include <opencv2\opencv.hpp>

/** \brief erodes and dilates \a img \a numED times

\param[in] img image to be eroded and dilated
\param[in] numED number of erosion and dilation steps

In each step, the image is
eroded-dilated-dilated-eroded with a square mask and then again
eroded-dilated-dilated-eroded with a circular mask.

The size of the masks is growing with each step.

\return result.
*/
cv::Mat1b aucu3_applyErodeDilate(const cv::Mat1b & img, const int numED);