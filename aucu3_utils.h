#pragma once
#include <opencv2\opencv.hpp>

/**
\brief Scales a cv::Mat to [0,255]

\param[in] matIn cv::Mat to be scaled
\param[in] flagRB specifies the output format\n
0: matreal
1: cv::Mat1b

\return cv::Mat1d or cv::Mat1b (depending on \a flagRB) with
values between 0 and 255
*/
cv::Mat aucu3_scaleMat1d(const cv::Mat1f & matIn, const int flagRB = 2);

/**
\brief load the image specified by \a imageFlag

\param[in] imageFlag integer currently in [0,10]

Loads the image "../pics/Gold/gold_< imageFlag >.png".

\warning The path to the images and the image-names are hard-coded and

\return image as cv::Mat3b
*/
cv::Mat3b aucu3_loadImage(const int imageFlag = 0);

/**
\brief gets a single channel of an image

\param[in] src original image cv::Mat
\param[in] channelFlag specifies which channel to use_\n
[0=gray, B, G, R, L, a, b, H, S, 9=V]

\return single channel as cv::Mat1b
*/
cv::Mat1b aucu3_getChannel(const cv::Mat src, const int channelFlag);