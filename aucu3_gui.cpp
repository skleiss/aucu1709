#pragma once
#include <opencv2\opencv.hpp>
#include "aucu3_gui.h"

GuiHandler::GuiHandler(std::string guiN)
{
	guiName = guiN;
	numFlags = 0;

	std::string n;
	std::vector< std::string > v;

	const std::vector< int > xx = { 30,600 };
	const std::vector< int > yy = { 30,70,110,150 };
	const int dd = 15;

	// flag 0
	n = "Pic.";
	v = { "a0","a1","a2","a3", "a4","b1","b2","b3", "b4","c1" };
	addFlag(0, n, v, yy[0], xx[0], 0);

	// flag 1
	n = "Chn.";
	v = { "gr", "B", "G", "R", "L", "a", "b", "H", "S", "V" };
	addFlag(1, n, v, yy[0] - dd, xx[1], 4);

	// flag 2
	n = "Text.";
	v = { "nil", "illum.", "text.", "both" };
	addFlag(2, n, v, yy[1] - dd, xx[1], 3, -1, 60);

	// flag 3
	n = "Seg.k";
	v = { "50","100","150","200","250","300","350","400","450","500","550" };
	addFlag(3, n, v, yy[1], xx[0], 5);
	// flag 4
	n = "Seg.s";
	v = { "25","50","75","100","125","150","175","200","225","250","275" };
	addFlag(4, n, v, yy[2], xx[0], 4);

	// flag 6
	n = "sib.";
	addFlag(6, n, 7, yy[3], xx[1], 1);

	// flag 7
	n = "smo.";
	addFlag(7, n, 10, yy[3], xx[0], 6);

	// flag 8
	n = "debug";
	v = { "no","yes" };
	addFlag(8, n, v, yy[1] - dd, xx[1] + 320, 1);

	applyProcedures(0);
}

void GuiHandler::addFlag(
	const int flagIdx,
	const std::string flagName,
	const std::vector< std::string > & flagButLabel,
	const int barT, const int barL,
	const int defVal,
	const int butH, 
	const int butW,
	const int butR)
{
	int H, W, R;
	(butW == -1 ? W = butDefW : W = butW);
	(butH == -1 ? H = butDefH : H = butH);
	(butR == -1 ? R = butDefR : R = butR);

	while (flagNames.size() < flagIdx + 1) {
		flagNames.push_back("");
		flagButLabels.push_back({ "" });
		flagNumVals.push_back(0);

		flagVals.push_back(0);

		flagButSpec.push_back({ 0 });
		flagPos.push_back({ 0 });
	}

	flagNames[flagIdx] = flagName;
	flagButLabels[flagIdx] = flagButLabel;
	flagNumVals[flagIdx] = flagButLabel.size();

	flagVals[flagIdx] = defVal;

	flagButSpec[flagIdx] = { H,W,R };
	flagPos[flagIdx] = { barT, barL };

	menuH = std::max(menuH, int(barT + H + butSep));
	menuW = std::max(menuW, int(barL + flagNameW + flagButLabel.size() * W + butSep + 20));
}

void GuiHandler::addFlag(
	const int flagIdx,
	const std::string flagName,
	const int N,
	const int barT, int barL,
	const int defVal,
	const int butH, int butW, int butR)
{
	std::vector< std::string > flagButLabel;
	std::string s;
	for (int i = 0; i <= N; i++) {
		flagButLabel.push_back(std::to_string(i));
	}
	addFlag(flagIdx, flagName, flagButLabel, barT, barL, defVal, butH, butW, butR);
}

cv::Size GuiHandler::writeText(cv::Mat3b & img, std::string text, int x, int y, cv::Scalar textCol, cv::Scalar bgCol, float scX, float scY)
{
	int b;
	cv::Size s = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b);

	cv::Mat3b box((s.height + b), s.width, cv::Vec3b(bgCol[0], bgCol[1], bgCol[2]));
	cv::putText(box, text, cv::Point(0, s.height), cv::FONT_HERSHEY_SIMPLEX, 1, textCol, 2);

	cv::resize(box, box, cv::Size(scX * box.cols, scY * box.rows));

	box.copyTo(img(cv::Rect(x, y, box.cols, box.rows)));
	return s;
}

cv::Mat3b GuiHandler::menu2img()
{
	const double xd = 0.5;
	const double xu = 1. / xd;
	const double yd = 0.7;
	const double yu = 1. / yd;

	cv::Scalar actScal;

	cv::Mat3b ret(yu * menuH, xu * menuW, cv::Vec3b(79, 75, 71));

	for (int k = 0; k < flagNames.size(); k++) {
		if (flagNumVals[k] == 0) {
			continue;
		}

		const int T0 = flagPos[k][0];
		const int L0 = flagPos[k][1];

		actScal = green;
		cv::rectangle(ret, cv::Rect(xu * L0, yu * T0, xu * flagNameW, yu * flagButSpec[k][0]), actScal, -1);

		int b, b2;
		cv::Size s = cv::getTextSize(flagNames[k], cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b);

		cv::putText(ret, flagNames[k], cv::Point(xu*(L0 + 6), yu*(T0 + flagButSpec[k][0] - b)),
			cv::FONT_HERSHEY_SIMPLEX, 1, black, 2);

		for (int i = 0; i < flagNumVals[k]; i++) {
			int L = L0 + flagNameW + 6 + i * flagButSpec[k][1] + 1;
			int T = T0;

			(i == flagVals[k] ? actScal = green : actScal = white);

			cv::Rect R(xu * L, yu *  T, xu * flagButSpec[k][1] - 2, yu *  flagButSpec[k][0]);
			cv::rectangle(ret, R, actScal, -1);

			(i == flagVals[k] ? actScal = black : actScal = gray);

			cv::Size s = cv::getTextSize(flagButLabels[k][i], 
				cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b2);

			T += flagButSpec[k][0] - b;
			L += 0.5*(flagButSpec[k][1] - xd*s.width);
			cv::putText(ret, flagButLabels[k][i], cv::Point(xu * L, yu* T), 
				cv::FONT_HERSHEY_SIMPLEX, 1, actScal, 2);
		}
	}

	cv::resize(ret, ret, cv::Size(xd * ret.cols, yd * ret.rows));

	return ret;
}

void GuiHandler::applyProcedures(const int flagProc)
{
	std::clock_t cillian;
	cillian = std::clock();

	switch (flagProc)
	{
	case 0:
		//cout << " load img" << endl;
		imgSrc = aucu3_loadImage(flagVals[0]);
	case 1:
		//cout << " get channel" << endl;
		imgChn = aucu3_getChannel(imgSrc, flagVals[1]);
	case 2:
		//cout << " do texture analysis" << endl;
		imgText = aucu3_doTextureAnalysis(imgChn, flagVals[2]);
	case 3:
	case 4:
		//cout << " do graph segmentation" << endl;
		grSegOut = aucu3_doGraphSegmentation(imgText, 50 + 50 * flagVals[3], 25 + 25 * flagVals[4]);
		imgSeg = aucu3_drawSegPicture(grSegOut);

		aucu3_doGrSegComponentCalcs(grSegOut, imgText,
			grSegCompSize, grSegCompTextAvg, grSegLargestComp, grSegDiff2largestCompTextAvg);

	case 6: // sib
	case 8: // debug-toggle

			//cout << " find bg" << endl;
		bgComps = aucu3_findBgCompAuto(grSegCompSize, grSegCompTextAvg,
			grSegLargestComp, grSegDiff2largestCompTextAvg, flagVals[6]);

	case 5: // BG sel // TODO

		if (flagVals[8] == 1)
		{
			aucu3_findBgCompAuto_debugPlots(grSegOut, grSegCompSize, grSegCompTextAvg, grSegLargestComp,
				bgComps, findBgDebug_TextAvgs, findBgDebug_PtPlot);
		}

		imgBgFg = aucu3_drawFgBg(grSegOut, bgComps);

	case 7: // smo
			//cout << " do smoothing " << flagVals[7] << endl;
		imgSmooth = aucu3_applyErodeDilate(imgBgFg, flagVals[7]);
		break;
	}
	showDisplay();

	cillian = std::clock() - cillian;
	std::cout << "        time used: " << cillian << "ms" << std::endl;
}

void GuiHandler::applyClick(int x, int y)
{
	int cButton = -1;
	int cFlag = -1;

	for (size_t k = 0; k < flagNames.size(); k++) {
		if (flagPos[k][0] < y && y < flagPos[k][0] + flagButSpec[k][0]) {
			int c0 = flagPos[k][1] + flagNameW;
			int c1 = flagPos[k][1] + flagNameW + flagNumVals[k] * flagButSpec[k][1];
			if (c0 < x && x < c1) {
				cButton = std::floor(double(x - c0) / double(flagButSpec[k][1]));
				flagVals[k] = cButton;
				cFlag = k;

				applyProcedures(cFlag);

				return;
			}
		}
	}

	if (x0 < x && x < x0 + tileP) {
		if (menuH + y1 + tileHR < y && y < menuH + y1 + tileHR + tileP) {
			applyManualBgSelection(float(x - x0) / float(tileP), float(y - (menuH + y1 + tileHR)) / float(tileP));
		}
	}

}

void GuiHandler::applyManualBgSelection(const float s, const float t)
{
	const int c = s * float(grSegOut.cols);
	const int r = t * float(grSegOut.rows);
	const int comp = grSegOut.at<int>(r, c);

	bool foundIt = false;

	for (int i = 0; i < bgComps.size(); i++) {
		if (bgComps[i] == comp) {
			bgComps[i] = bgComps[bgComps.size() - 1];
			foundIt = true;
		}
	}

	(foundIt ? bgComps.pop_back() : bgComps.push_back(comp));

	applyProcedures(5);
	showDisplay();
}

cv::Mat3b GuiHandler::img2tile(const cv::Mat1f & img, int R, int C)
{
	cv::Mat1b ret = aucu3_scaleMat1d(img, 2);
	return img2tile(ret, R, C);
}

cv::Mat3b GuiHandler::img2tile(const cv::Mat3b & img, int R, int C)
{
	cv::Mat3b ret(img);
	cv::resize(ret, ret, cv::Size(C, R));
	return ret;
}

cv::Mat3b GuiHandler::img2tile(const cv::Mat1b & img, int R, int C)
{
	cv::Mat3b ret;
	std::vector< cv::Mat1b > tmpV1b(3);
	tmpV1b[0] = img;
	tmpV1b[1] = img;
	tmpV1b[2] = img;
	cv::merge(tmpV1b, ret);
	cv::resize(ret, ret, cv::Size(C, R));
	return ret;
}

void GuiHandler::showDisplay()
{
	cv::Mat3b m = menu2img();

	cv::Mat3b d(picsR, picsC, cv::Vec3b(gray[0], gray[1], gray[2]));

	cv::Mat3b act;

	const float tsX(0.5);
	const float tsY(0.6);

	act = img2tile(imgSrc, tileP, tileP);
	act.copyTo(d(cv::Rect(x0, y0 + tileHR, tileP, tileP)));
	writeText(d, "source (" + std::to_string(imgSrc.rows) + " x " + std::to_string(imgSrc.cols) + " px)",
		x0, y0, white, gray, tsX, tsY);

	act = img2tile(imgChn, tileP, tileP);
	act.copyTo(d(cv::Rect(x1, y0 + tileHR, tileP, tileP)));
	writeText(d, "channel", x1, y0, white, gray, tsX, tsY);

	act = img2tile(imgText, tileP, tileP);
	act.copyTo(d(cv::Rect(x2, y0 + tileHR, tileP, tileP)));
	writeText(d, "texture", x2, y0, white, gray, tsX, tsY);

	act = img2tile(imgSeg, tileP, tileP);
	act.copyTo(d(cv::Rect(x0, y1 + tileHR, tileP, tileP)));
	std::string txt = "segmentation (k:" + std::to_string(flagVals[3]) + " s:" + std::to_string(flagVals[4]) + ") ";
	txt += std::to_string(grSegCompSize.size()) + " comp.";
	writeText(d, txt, x0, y1, white, gray, tsX, tsY);

	act = img2tile(imgBgFg, tileP, tileP);
	act.copyTo(d(cv::Rect(x1, y1 + tileHR, tileP, tileP)));
	writeText(d, "foreground", x1, y1, white, gray, tsX, tsY);

	act = img2tile(imgSmooth, tileP, tileP);
	act.copyTo(d(cv::Rect(x2, y1 + tileHR, tileP, tileP)));
	writeText(d, "smoothened " + std::to_string(flagVals[7]), x2, y1, white, gray, tsX, tsY);

	if (flagVals[8] == 1) {
		act = img2tile(findBgDebug_TextAvgs, tileP, tileP);
		act.copyTo(d(cv::Rect(x3, y0 + tileHR, tileP, tileP)));
		writeText(d, "debug texture average", x3, y0, white, gray, tsX, tsY);

		act = img2tile(findBgDebug_PtPlot, tileP, tileP);
		act.copyTo(d(cv::Rect(x3, y1 + tileHR, tileP, tileP)));
		writeText(d, "debug texture average", x3, y1, white, gray, tsX, tsY);
	}

	cv::Mat3b display(m.rows + picsR, std::max(m.cols, picsC) + 0, cv::Vec3b(gray[0], gray[1], gray[2]));

	m.copyTo(display(cv::Rect(0, 0, m.cols, m.rows)));
	d.copyTo(display(cv::Rect(0, m.rows, picsC, picsR)));

	menuH = m.rows;
	menuW = m.cols;

	cv::imshow(guiName, display);
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
	for (int i = 0; i < segOut.rows; i++) {
		for (int j = 0; j < segOut.cols; j++) {
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

			ret.at< cv::Vec3b >(i, j) = cv::Vec3b(d2, d1, d0);
		}
	}
	return ret;
}


/** \brief draws the foreground and background

\param[in] grSegOut result from the graph segmentation function
\param[in] bgComp vector listing all the background components

\return black/white image where all components listed in \a bgComp are black and the rest (the foreground) are white
*/
cv::Mat1b aucu3_drawFgBg(const cv::Mat1i & grSegOut, const std::vector<int> & bgComp)
{
	cv::Mat1b ret(grSegOut.rows, grSegOut.cols, uchar(255));
	for (int i = 0; i < grSegOut.rows; i++)	{
		for (int j = 0; j < grSegOut.cols; j++) {
			for (size_t k = 0; k < bgComp.size(); k++) {
				if (bgComp[k] == grSegOut(i, j)) {
					ret(i, j) = uchar(0);
				}
			}
		}
	}
	return ret;
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
	const std::vector< float > & grSegCompTextAvg,
	const int grSegLargestComp,
	const std::vector< int > & bgComp,
	cv::Mat1b & findBgDebug_TextAvgs,
	cv::Mat1b & findBgDebug_PtPlot)
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

	float maxTextAvg = grSegCompTextAvg[0];
	for (size_t k = 1; k < grSegCompTextAvg.size(); k++) {
		if (maxTextAvg < grSegCompTextAvg[k])  maxTextAvg = grSegCompTextAvg[k];
	}

	const size_t numC = grSegCompSize.size();
	const int dy = std::max(1.0, std::min(5.0, 260.0 / float(numC)));

	for (size_t k = 0; k < grSegCompTextAvg.size(); k++) {
		const int ax = 260 * grSegCompTextAvg[k] / maxTextAvg + 30;
		const int ay = (int(k) + 1) * dy + 30;

		for (size_t i = 0; i < bgComp.size(); i++) {
			if (k == bgComp[i]) {
				if (k == grSegLargestComp) {
					cv::line(findBgDebug_PtPlot, cv::Point(ax, 30), cv::Point(ax, 280), cv::Scalar(255));
				}
				cv::rectangle(findBgDebug_PtPlot, cv::Rect(ax - 3, ay - 3, 6, 6), cv::Scalar(255), 1);
			}
		}
		cv::rectangle(findBgDebug_PtPlot, cv::Rect(ax - 1, ay - 1, 2, 2), cv::Scalar(255), 1);
	}

	for (int i = 0; i < NR; i++) {
		for (int j = 0; j < NC; j++) {
			const int c = grSegOut.at<int>(i, j);
			findBgDebug_TextAvgs.at<uchar>(i, j) = uchar(std::round(255.0 * grSegCompTextAvg[c] / maxTextAvg));
		}
	}
}
