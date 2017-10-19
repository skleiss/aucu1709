#pragma once
#include <opencv2\opencv.hpp>

/** \brief applies graph segmentation algorithm to \a img.

\param[in] img cv::Mat1f that is to be segmented
\param[in] gSk parameter \a k of the graph segmentation algorithm
\param[in] gSs parameter "min Size" of the graph segmentation algorithm.

The function calls openCV's graph segmentation algorithm with parameters \a gSk and \a gSs.

\return cv::Mat1i where the integer value of a pixel is the index of the component it belongs to.
*/
extern cv::Mat1i aucu3_doGraphSegmentation(const cv::Mat1f & img, const int gSk, const int gSs);


/* \brief computes component sizes and their texture energy averages

\param[in] grSegRes result of the graph segmentation of an image \a img
\param[in] textureEnergy texture energy of the original image \a img

\param[out] grSegCompSize vector containing the size of each component
\param[out] grSegCompTextAvg vector containing the average texture energy of each component
\param[out] grSegLargestComp index of the component with the largest size/area
\param[out] grSegDiff2largestCompTextAvg vector containing \n
|grSegCompTextAvg[i] - grSegCompTextAvg[ grSegLargestComp] |
*/
void aucu3_doGrSegComponentCalcs(const cv::Mat1i & grSegRes,
	const cv::Mat1f & textureEnergy,
	std::vector< int > & grSegCompSize,
	std::vector< float > & grSegCompTextAvg,
	int & grSegLargestComp,
	std::vector< float > & grSegDiff2largestCompTextAvg);

/* \brief Automatically tries to find the background

\param[in] grSegCompSize
\param[in] grSegCompTextAvg
\param[in] grSegLargestComp
\param[in] grSegDiff2largestCompTextAvg
\param[in] numSib number of "siblings"

Chooses the largest component (specified by \a grSegLargestComp) as the 
background and searches for the \a numSib additoinal components that have the
most similar texture energy average.

\return vector of size <a>(numSib + 1)</a>with the indices of the background-components
*/
std::vector<int> aucu3_findBgCompAuto(
	const std::vector< int > & grSegCompSize,
	const std::vector< float > & grSegCompTextAvg,
	const int grSegLargestComp,
	const std::vector< float > & grSegDiff2largestCompTextAvg,
	const int numSib);