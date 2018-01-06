#pragma once


#include "cv.h"
#include "pbVideoCallback.h"

using namespace cv;
using namespace std;

class pbCLEye
{
public:
	//Setup
	void setup();
	void setCallback( pbVideoGrabberCallbackType callback, void *param );
	int cameraCount();
	bool startCamera(	int device,
						int w, int h,		
						int fps,			
						bool color,					
						bool mirrorX						
					);		

	bool stopCamera();

	//Grab
	bool isFrameNew();
	Mat &grabFrame();	
	
	//Camera params
	typedef int CamParamID;
	bool setParameter( CamParamID param, int value );
	int getParameter( CamParamID param );

	CamParamID PARAM_AUTO_GAIN				;	// [false, true]
	CamParamID PARAM_GAIN					;	// [0, 79]
	CamParamID PARAM_AUTO_EXPOSURE			;	// [false, true]
	CamParamID PARAM_EXPOSURE				;	// [0, 511]
	CamParamID PARAM_AUTO_WHITEBALANCE		;	// [false, true]
	CamParamID PARAM_WHITEBALANCE_RED		;	// [0, 255]
	CamParamID PARAM_WHITEBALANCE_GREEN		;	// [0, 255]
	CamParamID PARAM_WHITEBALANCE_BLUE		;	// [0, 255]

	CamParamID PARAM_HFLIP					;   // [false, true]
	CamParamID PARAM_VFLIP					;   // [false, true]

private:
	Mat _frame;	
	//vector<Mat> _planes;

	void *_cam;
	bool _mirrorX;

	pbVideoGrabberCallbackType _callback;
	void *_callbackParam;

};
