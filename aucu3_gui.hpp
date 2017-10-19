#pragma once

#include <opencv2\opencv.hpp>

#include "aucu3_procedures.hpp"

/// \typedef defines the type used of scalar reals.
typedef float scalreal;
/// \typedef defines the type used of cv::Mat with real entries
typedef cv::Mat1f matreal;


class guiHandler
{
public:
	guiHandler(std::string guiN)
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

		//// flag 5
		//n = "BGsel";
		//v = { "auto","manual" };
		//addFlag(n, v, yy[2], xx[1], 0, -1, 80);

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
	};

public:
	/// name of the GUI-window
	std::string guiName; 

	// labels of the flags
	std::vector< std::string > flagNames;
	// number of options per flag
	std::vector< int > flagNumVals;
	// labels of the buttons
	std::vector< std::vector< std::string > > flagButLabels;
	// height, width, and number of rows of buttons
	std::vector< std::vector< int >> flagButSpec; // H, W, numRows
	// top and left position of the flag...
	std::vector< std::vector< int >> flagPos; // T, L
	// currently selected flag values
	std::vector< int > flagVals;
	// total number of flags
	int numFlags;

	const int flagNameW = 55; // width of the field containing the flagName
	const int butDefW = 40;   // default button width
	const int butDefH = 32;   // default button height
	const int butDefR = 1;    // default number of rows
	const int butSep = 8;     // default vertical separation of buttons/flags

	int menuW = butSep; // will be computed/updated in addFlag
	int menuH = butSep; // will be computed/updated in addFlag

	cv::Mat3b imgSrc;   // original source image
	cv::Mat1b imgChn;   // selected channel
	matreal imgText;    // image containing texture energy
	cv::Mat1i grSegOut; // result of graph segmentation 
	cv::Mat3b imgSeg;   // image for displaying the coloured components

	// several auxiliary vectors
	std::vector< int > grSegCompSize;
	std::vector< scalreal > grSegCompTextAvg;
	int grSegLargestComp;
	std::vector< scalreal > grSegDiff2largestCompTextAvg;

	// two debugging pictures
	cv::Mat1b findBgDebug_TextAvgs;
	cv::Mat1b findBgDebug_PtPlot;
		
	std::vector<int> bgComps; // vector of background components

	cv::Mat1b imgBgFg;    // image showing background/foreground
	cv::Mat1b imgSmooth;  // smoothened version of imgBgFg

	// some colour-definitions
	const cv::Scalar green = cv::Vec3b(60, 202, 153);
	const cv::Scalar blue = cv::Vec3b(202, 164, 60);
	const cv::Scalar gray = cv::Vec3b(79, 75, 71);
	const cv::Scalar black = cv::Vec3b(0, 0, 0);
	const cv::Scalar white = cv::Vec3b(255, 255, 255);
	const cv::Scalar lightgray = cv::Vec3b(127, 127, 127);

	// -- tiles
	const int tileC = 300;
	const int tileHR = 20;   // num rows of tile header
	const int tileSep = 5;   // separation between tiles
	const int tileP = tileC; // size of picture in tile
	const int tileR = tileC + tileHR; // vertical total size of tile

	const int picsC = 3 * tileC + 4 * tileSep + (tileC + 3 * tileSep);
	const int picsR = 2 * tileR + 3 * tileSep + 20;

	const int x0 = tileSep;
	const int x1 = x0 + tileC + tileSep;
	const int x2 = x1 + tileC + tileSep;
	const int x3 = x2 + tileC + 2 * tileSep;

	const int y0 = tileHR;
	const int y1 = y0 + tileR + tileSep;

public:
	/** \brief add a flag to the menu.

	\param[in] flagName  name displayed on the label next to the buttons
	\param[in] the text on the buttons
	\param[in] barT "top"-coordinate of the upper left corner of the label
	\param[in] barL "left"-coordinate of the upper left corner of the label
	\param[in] defVal default button that is selected when the program starts
	\param[in] butH height of the button (if -1, set to default)
	\param[in] butW width of the button (if -1, set to default)
	\param[in] number of button-rows for this flag (if -1, set to default 1 row)
	*/
	void addFlag(
		const int flagIdx, 
		const std::string flagName,
		const std::vector< std::string > & flagButLabel,
		const int barT, const int barL, 
		const int defVal = 0, 
		const int butH = -1, 
		const int butW = -1, 
		const int butR = -1)
	{
		int H, W, R;
		(butW == -1 ? W = butDefW : W = butW);
		(butH == -1 ? H = butDefH : H = butH);
		(butR == -1 ? R = butDefR : R = butR);

		while (flagNames.size() < flagIdx + 1)
		{
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

		menuH = std::max(menuH, int( barT + H + butSep ));
		menuW = std::max(menuW, int( barL + flagNameW + flagButLabel.size() * W + butSep + 20));
	}

	/** \brief add a flag to the menu.

	\param[in] flagName  name displayed on the label next to the buttons
	\param[in] N number of buttons (will be automatically labelled 0 to N)
	\param[in] barT "top"-coordinate of the upper left corner of the label
	\param[in] barL "left"-coordinate of the upper left corner of the label
	\param[in] defVal default button that is selected when the program starts
	\param[in] butH height of the button (if -1, set to default)
	\param[in] butW width of the button (if -1, set to default)
	\param[in] number of button-rows for this flag (if -1, set to default 1 row)
	*/
	void addFlag(
		const int flagIdx,
		const std::string flagName,
		const int N,
		const int barT, int barL,
		const int defVal = 0,
		const int butH = -1, int butW = -1, int butR = -1)
	{
		std::vector< std::string > flagButLabel;
		std::string s;
		for (int i = 0; i <= N; i++)
		{
			flagButLabel.push_back(std::to_string(i));
		}
		addFlag(flagIdx, flagName, flagButLabel, barT, barL, defVal, butH, butW, butR);
	}

	/**
	\brief writes and rescales text in a box onto an image

	\param[in,out] img 
	\param[in] text  text to be written
	\param[in] x x-coordinate of upper left corner of textbox
	\param[in] y y-coordinate of upper left corner of textbox
	\param[in] textCol colour of the text as BGR-colours
	\param[in] bgCol colour of the background as BGR-colours
	\param[in] scX factor by which the textbox should be scaled in x-direction
	\param[in] scY factor by which the textbox should be scaled in y-direction
	
	Writes \a text into a box with the specified colours, using font cv::FONT_HERSHEY_SIMPLEX with line thickness 2.
	Then rescales that textbox and puts it at position (\a x, \a y) in \a img.

	\return the size of the textbox.
	*/
	cv::Size writeText(cv::Mat3b & img, std::string text, int x, int y, cv::Scalar textCol, cv::Scalar bgCol, scalreal scX, scalreal scY)
	{
		int b;
		cv::Size s = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b);

		cv::Mat3b box( (s.height+b), s.width, cv::Vec3b(bgCol[0], bgCol[1], bgCol[2]));
		cv::putText(box, text, cv::Point(0, s.height), cv::FONT_HERSHEY_SIMPLEX, 1, textCol, 2);
		
		cv::resize(box, box, cv::Size(scX * box.cols, scY * box.rows));
		
		box.copyTo(img(cv::Rect(x, y, box.cols, box.rows)));
		return s;
	}
	
	/**
	\brief creates an image (cv::Mat3b) that shows all the flags and buttons

	\return
	*/
	// this function uses a blown-up menu that is downscaled so that a
	// narrower font is created. A bit brutal, but it seems to work
	cv::Mat3b menu2img() 
	{
		const double xd = 0.5;
		const double xu = 1./xd;
		const double yd = 0.7;
		const double yu = 1. / yd;

		cv::Scalar actScal;

		cv::Mat3b ret(yu * menuH, xu * menuW, cv::Vec3b(79, 75, 71));

		for (int k = 0; k < flagNames.size(); k++)
		{
			if (flagNumVals[k] == 0)
			{
				continue;
			}

			const int T0 = flagPos[k][0];
			const int L0 = flagPos[k][1];
			
			actScal = green;
			cv::rectangle(ret, cv::Rect(xu * L0, yu * T0, xu * flagNameW, yu * flagButSpec[k][0]) , actScal, -1);

			int b, b2;
			cv::Size s = cv::getTextSize(flagNames[k], cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b);

			cv::putText(ret, flagNames[k], cv::Point(xu*(L0 + 6), yu*(T0 + flagButSpec[k][0] - b)), 
				cv::FONT_HERSHEY_SIMPLEX, 1, black, 2);

			for (int i = 0; i < flagNumVals[k]; i++)
			{
				int L = L0 + flagNameW + 6 + i * flagButSpec[k][1] + 1;
				int T = T0;

				(i == flagVals[k] ? actScal = green : actScal = white);
				
				cv::Rect R(xu * L, yu *  T, xu * flagButSpec[k][1] - 2, yu *  flagButSpec[k][0]);
				cv::rectangle(ret, R, actScal, -1);
				
				(i == flagVals[k] ? actScal = black: actScal = gray);

				cv::Size s = cv::getTextSize(flagButLabels[k][i], cv::FONT_HERSHEY_SIMPLEX, 1, 2, &b2);

				T += flagButSpec[k][0] - b;
				L += 0.5*(flagButSpec[k][1] - xd*s.width);
				cv::putText(ret, flagButLabels[k][i], cv::Point(xu * L, yu* T), cv::FONT_HERSHEY_SIMPLEX, 1, actScal, 2);
			}
		}

		cv::resize(ret, ret, cv::Size(xd * ret.cols, yd * ret.rows));

		return ret;
	}

	/**
	\brief starts the image analysis procedures

	At which point the procedure starts depends on \a flagProc:

	0: load image\n
	1: get colour channel from image\n
	2: do texture analysis\n
	3, 4: do graph segmentation and, if the debug plots should be shown, prepare the data for the debug plots\n
	6: find the largest component and its siblings
	7: smoothen the contours
	
	*/
	void applyProcedures(const int flagProc)
	{
		std::clock_t cillian;
		cillian = std::clock();

		switch (flagProc)
		{
		case 0:
			//cout << " load img" << endl;
			imgSrc = aucu3_loadImage( flagVals[0]);
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
			bgComps = aucu3_findBgCompAuto( grSegCompSize, grSegCompTextAvg,
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
		//cillian /= 1000;
		cout << "        time used: " << cillian << endl;
	}

	/**
	\brief takes coordinates of a click and checks for buttons

	\param[in] x and
	\param[in] y are the coordinates of the click

	This function checks whether one of the buttons was hit by the click and 
	then starts the corresponding procedure by calling applyProcedures()
	*/
	void applyClick(int x, int y)
	{
		int cButton = -1;
		int cFlag = -1;

		for (size_t k = 0; k < flagNames.size(); k++)
		{
			if (flagPos[k][0] < y && y < flagPos[k][0] + flagButSpec[k][0])
			{
				int c0 = flagPos[k][1] + flagNameW;
				int c1 = flagPos[k][1] + flagNameW + flagNumVals[k] * flagButSpec[k][1];
				if ( c0 < x && x < c1 )
				{
					cButton = std::floor(double(x - c0) / double(flagButSpec[k][1]));
					flagVals[k] = cButton;
					cFlag = k;

					applyProcedures(cFlag);

					return;
				}
			}
		}

		if (x0 < x && x < x0 + tileP)
		{
			if (menuH + y1 + tileHR < y && y < menuH + y1 + tileHR + tileP)
			{
				applyManualBgSelection(scalreal(x - x0) / scalreal(tileP), scalreal(y - (menuH + y1 + tileHR)) / scalreal(tileP));
			}
		}
		
	}

	void applyManualBgSelection(const scalreal s, const scalreal t)
	{
		const int c = s * grSegOut.cols;
		const int r = t * grSegOut.rows;
		const int comp = grSegOut.at<int>(r, c);

		bool foundIt = false;

		cout << " manual   ( " << s << " , " << t << " )  ->  ( " << c << " , " << r << " ) -> comp " << comp << endl;

		for (int i = 0; i < bgComps.size(); i++)
		{
			if (bgComps[i] == comp)
			{
				bgComps[i] = bgComps[bgComps.size() - 1];
				foundIt = true;
			}
		}

		cout << "u442... fond it is: " << foundIt << endl;

		( foundIt ? bgComps.pop_back() : bgComps.push_back(comp));
		
		cout << "u446" << endl;
		
		applyProcedures(5);
		showDisplay();
	}

	/**
	\brief converts and resizes a matreal to a cv::Mat3b of size \a R x \a C

	\param[in] img image
	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	Calls aucu3_scaleMat1d()

	\return the same (grayscale) image in format cv::Mat3b, resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const matreal & img, int R, int C)
	{
		cv::Mat1b ret = aucu3_scaleMat1d(img, 2);
		return img2tile(ret, R, C);
	}

	/**
	\brief resizes cv::Mat3b to have size \a R x \a C

	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	\return the same image, but resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const cv::Mat3b & img, int R, int C)
	{
		cv::Mat3b ret(img);
		cv::resize(ret, ret, cv::Size(C, R));
		return ret;
	}

	/**
	\brief converts and resizes a cv::Mat1b to a cv::Mat3b of size \a R x \a C

	\param[in] img image
	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	\return the same (grayscale) image as cv::Mat3b, resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const cv::Mat1b & img, int R, int C)
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

	/**
	\brief draws the whole GUI.

	Draws both the menu-section (at the top) and the 6 (or 8, if debug pictures are displayed)
	images and shows them in the window specified by \a guiName.	
	*/
	void showDisplay() 
	{
		cv::Mat3b m = menu2img();
		
		cv::Mat3b d(picsR, picsC, cv::Vec3b(gray[0], gray[1], gray[2]));

		cv::Mat3b act;

		const scalreal tsX = 0.5;
		const scalreal tsY = 0.6;
		
		act = img2tile(imgSrc, tileP, tileP);
		act.copyTo(d(cv::Rect(x0, y0+tileHR, tileP, tileP)));
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

		if (flagVals[8] == 1)
		{
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

};