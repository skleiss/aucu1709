#pragma once

#include <opencv2\opencv.hpp>
#include "C:/Libraries/opencv_git/opencv_contrib/modules/ximgproc/include/opencv2/ximgproc.hpp"

#include "fastbilateral.hpp"


/// \typedef defines the type used of scalar reals.
typedef float scalreal;
/// \typedef defines the type used of cv::Mat with real entries
typedef cv::Mat1f matreal;

using namespace std;

/**
\brief Scales a cv::Mat to [0,255]

\param[in] matIn cv::Mat to be scaled
\param[in] flagRB specifies the output format\n
0: matreal
1: cv::Mat1b

\return cv::Mat1d or cv::Mat1b (depending on \a flagRB) with
values between 0 and 255
*/
cv::Mat aucu3_scaleMat1d(const matreal & matIn, const int flagRB = 2)
{
	matreal m(matIn.rows, matIn.cols);
	matIn.copyTo(m);

	scalreal min = m.at<scalreal>(0, 0);
	scalreal max = min;
	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++)
		{
			if (m.at<scalreal>(i, j) > max)
			{
				max = m.at<scalreal>(i, j);
			}
			else if (m.at<scalreal>(i, j) < min)
			{
				min = m.at<scalreal>(i, j);
			}
		}

	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++)
			m.at<scalreal>(i, j) = 255 * (m.at<scalreal>(i, j) - min) / (max - min);

	cv::Mat ret;
	if (flagRB == 1)
	{
		matreal tmp(m);
		tmp.copyTo(ret);
	}

	if (flagRB == 2)
	{
		cv::Mat1b tmp(m);
		tmp.copyTo(ret);
	}
	return ret;
}

/**
\brief load the image specified by \a imageFlag

\param[in] imageFlag integer currently in [0,10]

Loads the image "../pics/Gold/gold_< imageFlag >.png".

\warning The path to the images and the image-names are hard-coded and 

\return image as cv::Mat3b 
*/
cv::Mat3b aucu3_loadImage(const int imageFlag = 0)
{
	std::string n = "../pics/Gold/gold_" + std::to_string(imageFlag) + ".png";
	cv::Mat3b ret = cv::imread(n, CV_LOAD_IMAGE_COLOR);
	
	if (!ret.data)
	{
		cout << "ERROR: could not load image!" << endl;
	}
	return ret;
}

/**
\brief gets a single channel of an image

\param[in] src original image cv::Mat
\param[in] channelFlag specifies which channel to use_\n
				[0=gray, B, G, R, L, a, b, H, S, 9=V]

\return single channel as cv::Mat1b
*/
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

/**
\brief removes illumination of an image

\param[in] imgr single-channel matreal.

Removes the illumination by setting
pixel value = pixel value - average of 5x5 neigbourhood of pixel

\return matreal with removed illumination
*/
matreal aucu3_removeIllumination(const matreal & imgr)
{
	const int NR = imgr.rows;
	const int NC = imgr.cols;
	const int d = 3;

	matreal ret(NR, NC, 0.0);

	int i0, i1, j0, j1;
	scalreal tmp;

	for (int ii = 0; ii < NR; ii++)
	{
		i0 = std::max(0, ii - d);
		i1 = std::min(ii + d + 1, NR - d);

		for (int jj = 0; jj < NC; jj++)
		{
			j0 = std::max(0, jj - d);
			j1 = std::min(jj + d + 1, NC - d);

			tmp = 0.0;

			for (int i = i0; i < i1; i++)
			{
				for (int j = j0; j < j1; j++)
				{
					tmp += imgr(i, j);
				}
			}
			ret(ii, jj) = imgr(ii, jj) - tmp / scalreal((i1 - i0)* (j1 - j0));
		}
	}
	return ret;
}

/**
\brief computes texture energy

\param[in] img matreal single-channel image.

Computes the texture energy a SUM(!) of the
9 Law's texture attributes.

\return matreal containing texture energy of \a img. 
The output matrix is smaller than the input by 4x4 pixels!
*/
matreal aucu3_computeTextureEnergy(const matreal & img)
{
	matreal mask(5, 5, 0.0);
	
	// 1 -3 -1 -5 0
	mask.at<scalreal>(0,0) = 1.0;
	mask.at<scalreal>(0, 1) = -3.0;
	mask.at<scalreal>(0,2) = -1.0;
	mask.at<scalreal>(0,3) = -5.0;
	mask.at<scalreal>(0,4) = 0.0;
	// -3 4 -38 -4 -7
	mask.at<scalreal>(1,0) = -3.0;
	mask.at<scalreal>(1, 1) = 4.0;
	mask.at<scalreal>(1,2) = -38.0;
	mask.at<scalreal>(1,3) = -4.0;
	mask.at<scalreal>(1,4) = 7.0;
	// -1 -38 100 -10 13
	mask.at<scalreal>(2,0) = -1.0;
	mask.at<scalreal>(2, 1) = -38.0;
	mask.at<scalreal>(2,2) = 100.0;
	mask.at<scalreal>(2,3) = -10.0;
	mask.at<scalreal>(2,4) = 13.0;
	// -5 -4 -10 4 -1
	mask.at<scalreal>(3, 0) = -5.0;
	mask.at<scalreal>(3, 1) = -4.0;
	mask.at<scalreal>(3,2) = -10.0;
	mask.at<scalreal>(3,3) = 4.0;
	mask.at<scalreal>(3,4) = -1.0;
	// 0 -7 13 -1 3
	mask.at<scalreal>(4,0) = 0.0;
	mask.at<scalreal>(4, 1) = -7.0;
	mask.at<scalreal>(4,2) = 13.0;
	mask.at<scalreal>(4,3) = -1.0;
	mask.at<scalreal>(4,4) = 3.0;

	const int tR = img.rows - 4;
	const int tC = img.cols - 4;
	
	matreal ret(tR, tC, 0.0);

	for (int ii = 0; ii < tR; ii++)
	{
		for (int jj = 0; jj < tC; jj++)
		{
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					ret(ii, jj) += mask(i, j) * img(ii + i, jj + j);
				}
			}
		}
	}
	return ret;
}

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
matreal aucu3_doTextureAnalysis(const matreal & img, const int textureFlag)
{
	if (textureFlag == 0)
	{
		return matreal(img);
	}
	else
	{
		matreal ret;
		if (textureFlag == 1 || textureFlag == 3)
		{
			ret = aucu3_removeIllumination(img);
		}
		else
		{
			img.copyTo(ret);
		}

		if (textureFlag == 2 || textureFlag == 3)
		{
			ret = aucu3_computeTextureEnergy(ret);
			ret = cv::abs(ret);
			ret = aucu3_scaleMat1d(ret, 1);

			matreal ret2;
			cv_extend::bilateralFilter(ret, ret2, 5, 5);

			return ret2;
		}

		return ret;
	}
}

/** \brief applies graph segmentation algorithm to \a img.

\param[in] img matreal that is to be segmented
\param[in] gSk parameter \a k of the graph segmentation algorithm
\param[in] gSs parameter "min Size" of the graph segmentation algorithm.

The function calls openCV's graph segmentation algorithm with parameters \a gSk and \a gSs.

\return cv::Mat1i where the integer value of a pixel is the index of the component it belongs to.
*/
cv::Mat1i aucu3_doGraphSegmentation(const matreal & img, const int gSk, const int gSs)
{
	cv::Ptr<cv::ximgproc::segmentation::GraphSegmentation> gs = cv::ximgproc::segmentation::createGraphSegmentation();

	gs->setK(gSk);
	gs->setMinSize(gSs);

	cv::Mat1i ret;
	gs->processImage(img, ret);
	return ret;
}

/** \brief visualizes the result of the image segmentation

\param[in] segOut cv::Mat1i where the values in segOut correspond to the index of the component
of the pixel.

Colours each component of the segmented image in a different colour.

\return cv::Mat3b where the components specified in \a segOut are coloured in (hopefully) well-distinguishable colours.
*/
cv::Mat3b aucu3_drawSegPicture(const cv::Mat1i & segOut)
{
	const int p = 32;
	const int q = 0;
	int c;
	int d0, d1, d2;
	cv::Mat3b ret(segOut.rows, segOut.cols);
	for (int i = 0; i < segOut.rows; i++)
		for (int j = 0; j < segOut.cols; j++)
		{
			c = 14 * segOut.at<int>(i, j);
			
			d0 = c % 9;
			c = (c - d0) / 9;
			d1 = c % 9;
			c = (c - d1) / 9;
			d2 = c % 9;
			
			d0 = p * d0 + q;
			d1 = p * d1 + q;
			d2 = p * d2 + q;

			if (d0 == 256) d0 = 255;
			if (d1 == 256) d1 = 255;
			if (d2 == 256) d2 = 255;

			ret.at< cv::Vec3b >(i, j) = cv::Vec3b(d2, d1, d0 );
		}
	return ret;
}

/** \brief computes component sizes and their texture energy averages

\param[in] grSegRes result of the graph segmentation of an image \a img
\param[in] textureEnergy texture energy of the original image \a img

\param[out] grSegCompSize vector containing the size of each component
\param[out] grSegCompTextAvg vector containing the average texture energy of each component
\param[out] grSegLargestComp index of the component with the largest size/area
\param[out] grSegDiff2largestCompTextAvg vector containing \n
|grSegCompTextAvg[i] - grSegCompTextAvg[ grSegLargestComp] |
*/
void aucu3_doGrSegComponentCalcs( const cv::Mat1i & grSegRes, 
	const matreal & textureEnergy,
	std::vector< int > & grSegCompSize, 
	std::vector< scalreal > & grSegCompTextAvg, 
	int & grSegLargestComp, 
	std::vector< scalreal > & grSegDiff2largestCompTextAvg
	)
{
	// investigate the Law's Texture Energy on components
	grSegCompTextAvg.clear();
	grSegCompTextAvg.push_back(0.0);

	// compute the sizes of the components
	// NOTE: number of components not a priori known!
	//std::vector<int> compSize;
	grSegCompSize.clear();
	grSegCompSize.push_back(0);

	for (int i = 0; i < grSegRes.rows; i++)
		for (int j = 0; j < grSegRes.cols; j++)
		{
			int act = grSegRes.at<int>(i, j);

			// brutally make sure the vector is of the correct size
			while (grSegCompSize.size() < act + 1)
			{
				grSegCompSize.push_back(0);
				grSegCompTextAvg.push_back(0.0);
			}
			// add pixel to size and LawAverage
			grSegCompSize[act]++;
			grSegCompTextAvg[act] += scalreal(textureEnergy.at<scalreal>(i, j));
		}

	grSegLargestComp = 0;

	for (size_t i = 0; i < grSegCompSize.size(); i++)
	{
		grSegCompTextAvg[i] /= scalreal(grSegCompSize[i]);
		if (grSegCompSize[i] > grSegCompSize[grSegLargestComp])
		{
			grSegLargestComp = i;
		}
	}

	grSegDiff2largestCompTextAvg.resize(grSegCompTextAvg.size());

	// compute L1-differences to LawAvg of largest component
	for (int k = 0; k < grSegCompTextAvg.size(); k++)
	{
		grSegDiff2largestCompTextAvg[k] = std::abs(grSegCompTextAvg[k] - grSegCompTextAvg[grSegLargestComp]);
	}
}

std::vector<int> aucu3_findBgCompAuto(
	const std::vector< int > & grSegCompSize,
	const std::vector< scalreal > & grSegCompTextAvg,
	const int grSegLargestComp,
	const std::vector< scalreal > & grSegDiff2largestCompTextAvg, 
	const int numSib )
{
	std::vector<int> siblings;
	siblings.push_back(grSegLargestComp);

	std::vector<bool> notYetSibling(grSegCompTextAvg.size());
	std::fill(notYetSibling.begin(), notYetSibling.end(), true);

	notYetSibling[grSegLargestComp] = false;

	// search for the siblings
	// go through all components numOfSiblingsSlide-times
	// i.e., effort is O(numOfSiblingsSlide*n),
	// instead of O(n*log n), if components were sorted by size
	for (int k = 0; k < numSib; k++)
	{
		int sibling_k = -1;
		scalreal siblings_diff = 0.0;
		//double siblings_diff = grSegMaxDiff2largestCompTextAvg[0];
		for (int i = 1; i < grSegCompTextAvg.size(); i++)
		{
			if (notYetSibling[i]) // only if the component is not already a sibling
			{
				// if nothing found yet, or
				// this one is closer to the largest component than the previously found one
				if (sibling_k == -1 || grSegDiff2largestCompTextAvg[i] < siblings_diff)
				{
					// currently the closest
					sibling_k = i;
					siblings_diff = grSegDiff2largestCompTextAvg[i];
				}
			}
		}
		if (sibling_k > -1)
		{
			siblings.push_back(sibling_k);
			notYetSibling[sibling_k] = false;
		}
	}

	return siblings;
}


/** \brief draws two debugging-plots related to texture energy

\param[in] grSegOut result of graph segmentation
\param[in] grSegCompSize vector containing the sizes of each component
\param[in] grSegCompTextAvg vector containing the average texture energy of each component
\param[in] grSegLargestComp index of the component with the largest size/area
\param[in] bgComp vector containing the indices of the background components
\param[out] findBgDebug_TextAvgs grayscale image where each component of \a grSegOut is coloured by its average texture energy
\param[out] findBgDebug_PtPlot point-plot where each component's average texture energy is plotted.\n
x-direction: component's average texture energy\n
y-direction: component's index\n
The background components are highlighted with a box.\
In addition, a vertical line is drawn through the largest component.
*/
void aucu3_findBgCompAuto_debugPlots(
	const cv::Mat1i & grSegOut,
	const std::vector< int > & grSegCompSize,
	const std::vector< scalreal > & grSegCompTextAvg,
	const int grSegLargestComp,
	const std::vector< int > & bgComp,
	cv::Mat1b & findBgDebug_TextAvgs,
	cv::Mat1b & findBgDebug_PtPlot
	)
{
	const int NR = grSegOut.rows;
	const int NC = grSegOut.cols;
	
	cv::Mat1b tmp1(NR, NC, uchar(0));
	tmp1.copyTo(findBgDebug_TextAvgs);
	
	cv::Mat1b tmp2(600, 600, uchar(0));
	
	int tb;
	cv::Size ts = cv::getTextSize("texture value", cv::FONT_HERSHEY_SIMPLEX, 0.7, 2, &tb);
	cv::putText(tmp2, "texture value", cv::Point(580 - ts.width, 60 - tb), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(63), 2);
	cv::putText(tmp2, "component", cv::Point(60, 580), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(63), 2);
	cv::resize(tmp2, tmp2, cv::Size(300, 300));

	tmp2.copyTo(findBgDebug_PtPlot);

	cv::line(findBgDebug_PtPlot, cv::Point(30, 30), cv::Point(290, 30), cv::Scalar(63));
	cv::line(findBgDebug_PtPlot, cv::Point(30, 30), cv::Point(30, 280), cv::Scalar(63));
	
	scalreal maxTextAvg = grSegCompTextAvg[0];
	for (size_t k = 1; k < grSegCompTextAvg.size(); k++)
	{
		if (maxTextAvg < grSegCompTextAvg[k])  maxTextAvg = grSegCompTextAvg[k];
	}

	const size_t numC = grSegCompSize.size();
	const int dy = std::max(1.0, std::min(5.0, 260.0 / scalreal(numC)));

	for (size_t k = 0; k < grSegCompTextAvg.size(); k++)
	{
		const int ax = 260 * grSegCompTextAvg[k] / maxTextAvg + 30;
		const int ay = (k + 1) * dy + 30;

		for (size_t i = 0; i < bgComp.size(); i++)
		{
			if (k == bgComp[i])
			{
				if (k == grSegLargestComp)
				{
					cv::line(findBgDebug_PtPlot, cv::Point(ax, 30), cv::Point(ax, 280), cv::Scalar(255));	
				}
				cv::rectangle(findBgDebug_PtPlot, cv::Rect(ax - 3, ay - 3, 6, 6), cv::Scalar(255), 1);
			}
		}
		cv::rectangle(findBgDebug_PtPlot, cv::Rect(ax - 1, ay - 1, 2, 2), cv::Scalar(255), 1);
	}

	for (int i = 0; i < NR; i++)
	{
		for (int j = 0; j < NC; j++)
		{
			const int c = grSegOut.at<int>(i, j);
			findBgDebug_TextAvgs.at<uchar>(i, j) = uchar(std::round(255.0 * grSegCompTextAvg[c] / maxTextAvg));
		}
	}
}

/** \brief draws the foreground and background

\param[in] grSegOut result from the graph segmentation function
\param[in] bgComp vector listing all the background components

\return black/white image where all components listed in \a bgComp are black and the rest (the foreground) are white
*/
cv::Mat1b aucu3_drawFgBg(const cv::Mat1i & grSegOut, const std::vector<int> & bgComp)
{
	cv::Mat1b ret(grSegOut.rows, grSegOut.cols, uchar(255));
	for (int i = 0; i < grSegOut.rows; i++)
	{
		for (int j = 0; j < grSegOut.cols; j++)
		{
			for (size_t k = 0; k < bgComp.size(); k++)
			{
				if ( bgComp[k] == grSegOut(i,j))
				{
					ret(i, j) = uchar(0);
				}
			}
		}
	}
	return ret;
}

/** \brief erodes and dilates \a img \a numED times

\param[in] img image to be eroded and dilated
\param[in] numED number of erosion and dilation steps

In each step, the image is 
eroded-dilated-dilated-eroded with a square mask and then again 
eroded-dilated-dilated-eroded with a circular mask.

The size of the masks is growing with each step.

\return result.
*/
cv::Mat1b aucu3_applyErodeDilate(const cv::Mat1b & img, const int numED)
{
	cv::Mat1b ret;
	img.copyTo(ret);

	for (int EDi = 1; EDi <= numED; EDi++)
	{
		int ED = 2 * EDi - 1;
		cv::Mat1b element(2 * ED + 1, 2 * ED + 1, uchar(1));

		// use square as mask
		cv::erode(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::dilate(ret, ret, element);
		cv::erode(ret, ret, element);

		// make circle out of it
		for (int i = 0; i < element.rows; i++)
		{
			for (int j = 0; j < element.cols; j++)
			{
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
