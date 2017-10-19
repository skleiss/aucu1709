#pragma once
#include <opencv2\opencv.hpp>
#include "C:/Libraries/opencv_git/opencv_contrib/modules/ximgproc/include/opencv2/ximgproc.hpp"
#include "aucu3_imgSegmentation.h"

cv::Mat1i aucu3_doGraphSegmentation(const cv::Mat1f & img, const int gSk, const int gSs)
{
	cv::Ptr<cv::ximgproc::segmentation::GraphSegmentation> gs = cv::ximgproc::segmentation::createGraphSegmentation();

	gs->setK(gSk);
	gs->setMinSize(gSs);

	cv::Mat1i ret;
	gs->processImage(img, ret);
	return ret;
}

void aucu3_doGrSegComponentCalcs(const cv::Mat1i & grSegRes,
	const cv::Mat1f & textureEnergy,
	std::vector< int > & grSegCompSize,
	std::vector< float > & grSegCompTextAvg,
	int & grSegLargestComp,
	std::vector< float > & grSegDiff2largestCompTextAvg
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

	for (int i = 0; i < grSegRes.rows; i++) {
		for (int j = 0; j < grSegRes.cols; j++) {
			int act = grSegRes.at<int>(i, j);

			// brutally make sure the vector is of the correct size
			while (grSegCompSize.size() < act + 1) {
				grSegCompSize.push_back(0);
				grSegCompTextAvg.push_back(0.0);
			}
			// add pixel to size and LawAverage
			grSegCompSize[act]++;
			grSegCompTextAvg[act] += float(textureEnergy.at<float>(i, j));
		}
	}

	grSegLargestComp = 0;

	for (size_t i = 0; i < grSegCompSize.size(); i++) {
		grSegCompTextAvg[i] /= float(grSegCompSize[i]);
		if (grSegCompSize[i] > grSegCompSize[grSegLargestComp]) {
			grSegLargestComp = i;
		}
	}

	grSegDiff2largestCompTextAvg.resize(grSegCompTextAvg.size());

	// compute L1-differences to LawAvg of largest component
	for (int k = 0; k < grSegCompTextAvg.size(); k++) {
		grSegDiff2largestCompTextAvg[k] = std::abs(grSegCompTextAvg[k] - grSegCompTextAvg[grSegLargestComp]);
	}
}

std::vector<int> aucu3_findBgCompAuto(
	const std::vector< int > & grSegCompSize,
	const std::vector< float > & grSegCompTextAvg,
	const int grSegLargestComp,
	const std::vector< float > & grSegDiff2largestCompTextAvg,
	const int numSib)
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
	for (int k = 0; k < numSib; k++) {
		int sibling_k = -1;
		float siblings_diff = 0.0;
		
		for (int i = 1; i < grSegCompTextAvg.size(); i++) {
			if (notYetSibling[i]) {
				// only if the component is not already a sibling

				// if nothing found yet, or
				// this one is closer to the largest component than the previously found one
				if (sibling_k == -1 || grSegDiff2largestCompTextAvg[i] < siblings_diff) {
					// currently the closest
					sibling_k = i;
					siblings_diff = grSegDiff2largestCompTextAvg[i];
				}
			}
		}

		if (sibling_k > -1) {
			siblings.push_back(sibling_k);
			notYetSibling[sibling_k] = false;
		}
	}

	return siblings;
}
