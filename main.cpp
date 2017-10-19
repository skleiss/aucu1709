#pragma once

#include <iostream>
#include <fstream>
#include <ctime>

#include <opencv2\opencv.hpp>

#include "aucu3_gui.h"

void mouseCallBackFunc3(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)	{
		GuiHandler* mh = (GuiHandler *)userdata;
		mh->applyClick(x, y);
	}
}

int main()
{
	std::string guiN("gui3");
	GuiHandler mh(guiN);

	cv::namedWindow(guiN, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(guiN, mouseCallBackFunc3, &mh);

	cv::waitKey(0);
	return 0;
}