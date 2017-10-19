#pragma once
#include <opencv2\opencv.hpp>

#include "fastbilateral.hpp"
#include "aucu3_textureAnalysis.h"
#include "aucu3_utils.h"

using namespace std;

/*
\brief removes illumination of an image

\param[in] imgr single-channel cv::Mat1f.

Removes the illumination by setting
pixel value = pixel value - average of 7x7 neigbourhood of pixel

\return cv::Mat1f with removed illumination
*/
cv::Mat1f aucu3_removeIllumination(const cv::Mat1f & imgr)
{
	const int NR = imgr.rows;
	const int NC = imgr.cols;
	const int d = 3;

	cv::Mat1f ret(NR, NC, 0.0);

	int i0, i1, j0, j1;
	float tmp;

	for (int ii = 0; ii < NR; ii++) {
		i0 = std::max(0, ii - d);
		i1 = std::min(ii + d + 1, NR - d);

		for (int jj = 0; jj < NC; jj++) {
			j0 = std::max(0, jj - d);
			j1 = std::min(jj + d + 1, NC - d);

			tmp = 0.0;

			for (int i = i0; i < i1; i++) {
				for (int j = j0; j < j1; j++) {
					tmp += imgr(i, j);
				}
			}
			ret(ii, jj) = imgr(ii, jj) - tmp / float((i1 - i0)* (j1 - j0));
		}
	}
	return ret;
}

/*
\brief computes texture energy

\param[in] img cv::Mat1f single-channel image.

Computes the texture energy a SUM(!) of the
9 Law's texture attributes.

\return cv::Mat1f containing texture energy of \a img.
The output matrix is smaller than the input by 4x4 pixels!
*/
cv::Mat1f aucu3_computeTextureEnergy(const cv::Mat1f & img)
{
	cv::Mat1f mask(5, 5, 0.0);

	// 1 -3 -1 -5 0
	mask.at<float>(0, 0) = 1.0;
	mask.at<float>(0, 1) = -3.0;
	mask.at<float>(0, 2) = -1.0;
	mask.at<float>(0, 3) = -5.0;
	mask.at<float>(0, 4) = 0.0;
	// -3 4 -38 -4 -7
	mask.at<float>(1, 0) = -3.0;
	mask.at<float>(1, 1) = 4.0;
	mask.at<float>(1, 2) = -38.0;
	mask.at<float>(1, 3) = -4.0;
	mask.at<float>(1, 4) = 7.0;
	// -1 -38 100 -10 13
	mask.at<float>(2, 0) = -1.0;
	mask.at<float>(2, 1) = -38.0;
	mask.at<float>(2, 2) = 100.0;
	mask.at<float>(2, 3) = -10.0;
	mask.at<float>(2, 4) = 13.0;
	// -5 -4 -10 4 -1
	mask.at<float>(3, 0) = -5.0;
	mask.at<float>(3, 1) = -4.0;
	mask.at<float>(3, 2) = -10.0;
	mask.at<float>(3, 3) = 4.0;
	mask.at<float>(3, 4) = -1.0;
	// 0 -7 13 -1 3
	mask.at<float>(4, 0) = 0.0;
	mask.at<float>(4, 1) = -7.0;
	mask.at<float>(4, 2) = 13.0;
	mask.at<float>(4, 3) = -1.0;
	mask.at<float>(4, 4) = 3.0;

	const int tR = img.rows - 4;
	const int tC = img.cols - 4;

	cv::Mat1f ret(tR, tC, 0.0);

	for (int ii = 0; ii < tR; ii++) {
		for (int jj = 0; jj < tC; jj++) {
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					ret(ii, jj) += mask(i, j) * img(ii + i, jj + j);
				}
			}
		}
	}
	return ret;
}

cv::Mat1f aucu3_doTextureAnalysis(const cv::Mat1f & img, const int textureFlag)
{
	if (textureFlag == 0) {
		return cv::Mat1f(img);
	}
	else {
		cv::Mat1f ret;
		if (textureFlag == 1 || textureFlag == 3) {
			ret = aucu3_removeIllumination(img);
		}
		else {
			img.copyTo(ret);
		}

		if (textureFlag == 2 || textureFlag == 3) {
			ret = aucu3_computeTextureEnergy(ret);
			ret = cv::abs(ret);
			ret = aucu3_scaleMat1d(ret, 1);

			cv::Mat1f ret2;
			cv_extend::bilateralFilter(ret, ret2, 5, 5);

			return ret2;
		}

		return ret;
	}
}