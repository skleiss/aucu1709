#pragma once
#include "aucu3_patchSmoothing.h"
#include <opencv2\opencv.hpp>

cv::Mat1b aucu3_applyErodeDilate(const cv::Mat1b & img, const int numED)
{
	cv::Mat1b ret;
	img.copyTo(ret);

	for (int EDi = 1; EDi <= numED; EDi++) {
		int ED = 2 * EDi - 1;
		cv::Mat1b element(2 * ED + 1, 2 * ED + 1, uchar(1));

		// use square as mask
		cv::erode(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::erode(ret, ret, element);

		// make circle out of it
		for (int i = 0; i < element.rows; i++) {
			for (int j = 0; j < element.cols; j++) {
				if ((i - ED)*(i - ED) + (j - ED)*(j - ED) > ED * ED + 0.9) element.at<uchar>(i, j) = uchar(0);
			}
		}

		cv::erode(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::erode(ret, ret, element);
	}
	return ret;
}
