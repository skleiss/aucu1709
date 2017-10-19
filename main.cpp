#pragma once

#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>

#include <ctime>

#include "opencv2/imgproc.hpp"
//#include "opencv2/segmentation.hpp"
//#include "ximgproc.hpp"



/*
#include "cannyEdgeDetection.h"
#include "ArbelaezStuff.h"
#include "Felzenszwalb1.h"
#include "Felzenszwalb2.h"

#include "watershed1.h"
#include "binaryPicRasterer1.h"
*/



#include "C:/Libraries/opencv_git/opencv_contrib/modules/ximgproc/include/opencv2/ximgproc.hpp"

//#include "archive.h"
//#include "colorFromIdx.h"
//#include "plotAllHistograms.h"
//#include "LawsTextureEnergyMeasure.h"

//#include "highguiTests1.h"

//#include "auxAuCu.h"
//#include "AuCu_GUI_1.h"



//#include "testMouseInteraction.h"
//#include "testFonts.h"

//#include "aucu3_procedures.hpp"
#include "aucu3_gui.hpp"


using namespace std;

void mouseCallBackFunc3(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		//cout << " mouseclick: " << x << "  " << y << endl;
		guiHandler* mh = (guiHandler *)userdata;
		mh->applyClick(x, y);
		
		//mh->showDisplay();
	}
}

int main()
{
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< FILE
	
	std::string guiN("gui3");
	guiHandler mh(guiN);

	cv::namedWindow(guiN, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(guiN, mouseCallBackFunc3, &mh);

	cv::waitKey(0);
	return 0;
}