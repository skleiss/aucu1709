#pragma once
#include <opencv2\opencv.hpp>
#include "aucu3_utils.h"

cv::Mat aucu3_scaleMat1d( const cv::Mat1f & matIn, const int flagRB )
{
	cv::Mat1f m(matIn.rows, matIn.cols);
	matIn.copyTo(m);

	float min = m.at<float>(0, 0);
	float max = min;
	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			if (m.at<float>(i, j) > max) {
				max = m.at<float>(i, j);
			}
			else if (m.at<float>(i, j) < min) {
				min = m.at<float>(i, j);
			}
		}
	}

	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			m.at<float>(i, j) = 255 * (m.at<float>(i, j) - min) / (max - min);
		}
	}

	cv::Mat ret;
	if (flagRB == 1) {
		cv::Mat1f tmp(m);
		tmp.copyTo(ret);
	}

	if (flagRB == 2) {
		cv::Mat1b tmp(m);
		tmp.copyTo(ret);
	}
	return ret;
}

cv::Mat3b aucu3_loadImage(const int imageFlag)
{
	std::string n = "../pics/Gold/gold_" + std::to_string(imageFlag) + ".png";
	cv::Mat3b ret = cv::imread(n, CV_LOAD_IMAGE_COLOR);

	if (!ret.data) {
		std::cout << "ERROR: could not load image!" << std::endl;
	}
	return ret;
}

cv::Mat1b aucu3_getChannel(const cv::Mat src, const int channelFlag)
{
	cv::Mat1b ret;

	cv::Mat3b tmpConverted;
	std::vector<cv::Mat1b> tmpChannels;

	switch (channelFlag)
	{
	case 1: // B
	case 2: // G
	case 3: // R
		cv::split(src, tmpChannels);
		tmpChannels[channelFlag - 1].copyTo(ret);
		break;

	case 4: // L
	case 5: // a
	case 6: // b
		cv::cvtColor(src, tmpConverted, CV_BGR2Lab);
		cv::split(tmpConverted, tmpChannels);
		tmpChannels[channelFlag - 4].copyTo(ret);
		break;

	case 7: // H
	case 8: // S
	case 9: // V

		cv::cvtColor(src, tmpConverted, CV_BGR2HSV);
		cv::split(tmpConverted, tmpChannels);
		tmpChannels[channelFlag - 7].copyTo(ret);

		break;
	case 0: // gray
	default:
		cv::cvtColor(src, ret, CV_BGR2GRAY);

		break;
	}

	return ret;
}




