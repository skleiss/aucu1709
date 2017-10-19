#pragma once

#include <opencv2\opencv.hpp>

#include "aucu3_textureAnalysis.h"
#include "aucu3_utils.h"
#include "aucu3_imgSegmentation.h"
#include "aucu3_patchSmoothing.h"


/** \brief visualizes the result of the image segmentation

\param[in] segOut cv::Mat1i where the values in segOut correspond to the index of the component
of the pixel.

Colours each component of the segmented image in a different colour.

\return cv::Mat3b where the components specified in \a segOut are coloured in (hopefully) well-distinguishable colours.
*/
cv::Mat3b aucu3_drawSegPicture(const cv::Mat1i & segOut);


/** \brief draws the foreground and background

\param[in] grSegOut result from the graph segmentation function
\param[in] bgComp vector listing all the background components

\return black/white image where all components listed in \a bgComp are black and the rest (the foreground) are white
*/
cv::Mat1b aucu3_drawFgBg(const cv::Mat1i & grSegOut, const std::vector<int> & bgComp);

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
	cv::Mat1b & findBgDebug_PtPlot);


/**
\brief Handles the GUI (handles clicks, calls functions, shows pictures)

The layout of the GUI (such as sizes and positions of the buttons and pictures,
descriptions, etc.) is defined in this class.

The functions needed for image analysis and manipulation are also called
in this class.
*/

class GuiHandler
{
public:
	/// Constructor.
	/// \param[in] guiN name of the cv::namedWindow with the GUI
	GuiHandler(std::string guiN);

private:
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
	cv::Mat1f imgText;    // image containing texture energy
	cv::Mat1i grSegOut; // result of graph segmentation 
	cv::Mat3b imgSeg;   // image for displaying the coloured components

	// several auxiliary vectors
	std::vector< int > grSegCompSize;
	std::vector< float > grSegCompTextAvg;
	int grSegLargestComp;
	std::vector< float > grSegDiff2largestCompTextAvg;

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
		const int butR = -1);

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
		const int butH = -1, int butW = -1, int butR = -1);

	/**
	\brief takes coordinates of a click and checks for buttons

	\param[in] x and
	\param[in] y are the coordinates of the click

	This function checks whether one of the buttons was hit by the click and
	then starts the corresponding procedure by calling applyProcedures()
	*/
	void applyClick(int x, int y);

private:
	/*
	\brief writes and rescales text in a box onto an image

	\param[in,out] img 
	\param[in] text  text to be written
	\param[in] x x-coordinate of upper left corner of textbox
	\param[in] y y-coordinate of upper left corner of textbox
	\param[in] textCol colour of the text as BGR-colours
	\param[in] bgCol colour of the background as BGR-colours
	\param[in] scX factor by which the textbox should be scaled in x-direction
	\param[in] scY factor by which the textbox should be scaled in y-direction
	
	Writes \a text into a box with the specified colours, using font 
	cv::FONT_HERSHEY_SIMPLEX with line thickness 2.
	Then rescales that textbox and puts it at position (\a x, \a y) in \a img.

	\return the size of the textbox.
	*/
	cv::Size writeText(cv::Mat3b & img, std::string text, int x, int y, 
		cv::Scalar textCol, cv::Scalar bgCol, float scX, float scY);
	
	/**
	\brief creates an image (cv::Mat3b) that shows all the flags and buttons

	\return
	*/
	// this function uses a blown-up menu that is downscaled so that a
	// narrower font is created. A bit brutal, but it seems to work
	cv::Mat3b menu2img();
	
	/**
	\brief starts the image analysis procedures

	At which point the procedure starts depends on \a flagProc:

	0: load image\n
	1: get colour channel from image\n
	2: do texture analysis\n
	3, 4: do graph segmentation and, if the debug plots should be shown, 
	      prepare the data for the debug plots\n
	6: find the largest component and its siblings
	7: smoothen the contours
	*/
	void applyProcedures(const int flagProc);
	
	
	void applyManualBgSelection(const float s, const float t);

	/**
	\brief converts and resizes a cv::Mat1f to a cv::Mat3b of size \a R x \a C

	\param[in] img image
	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	Calls aucu3_scaleMat1d()

	\return the same (grayscale) image in format cv::Mat3b, resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const cv::Mat1f & img, int R, int C);
	
	/**
	\brief resizes cv::Mat3b to have size \a R x \a C

	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	\return the same image, but resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const cv::Mat3b & img, int R, int C);

	/**
	\brief converts and resizes a cv::Mat1b to a cv::Mat3b of size \a R x \a C

	\param[in] img image
	\param[in] R number of rows of the target image
	\param[in] C number of columns of the target image

	\return the same (grayscale) image as cv::Mat3b, resized to have \a R rows and \a C columns.
	*/
	cv::Mat3b img2tile(const cv::Mat1b & img, int R, int C);
	
	/**
	\brief draws the whole GUI.

	Draws both the menu-section (at the top) and the 6 (or 8, if debug pictures are displayed)
	images and shows them in the window specified by \a guiName.	
	*/
	void showDisplay();

};


