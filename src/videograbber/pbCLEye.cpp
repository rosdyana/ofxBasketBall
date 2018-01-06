#include "pbCLEye.h"

#include "baseAppSettings.h"
#ifndef PS3EYELibDisable
#include "CLEyeMulticam.h"
#include "CLEyeCapture.h"
#endif

//-----------------------------------------------------------------------------
void pbCLEye::setup()
{
	_cam = 0;	
	_mirrorX = false;

	#ifndef PS3EYELibDisable

	PARAM_AUTO_GAIN				= CLEYE_AUTO_GAIN;
	PARAM_GAIN					= CLEYE_GAIN;
	PARAM_AUTO_EXPOSURE			= CLEYE_AUTO_EXPOSURE;
	PARAM_EXPOSURE				= CLEYE_EXPOSURE;
	PARAM_AUTO_WHITEBALANCE		= CLEYE_AUTO_WHITEBALANCE;
	PARAM_WHITEBALANCE_RED		= CLEYE_WHITEBALANCE_RED;
	PARAM_WHITEBALANCE_GREEN	= CLEYE_WHITEBALANCE_GREEN;
	PARAM_WHITEBALANCE_BLUE		= CLEYE_WHITEBALANCE_BLUE;
	PARAM_HFLIP					= CLEYE_HFLIP;
	PARAM_VFLIP					= CLEYE_VFLIP;
			
	#endif
	_callback = 0;

}

void pbCLEye::setCallback( pbVideoGrabberCallbackType callback, void *param )
{
	_callback = callback;
	_callbackParam = param;
	#ifndef PS3EYELibDisable

	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( cam ) {
		cam->setCallback( _callback, _callbackParam );
	}
	#endif
}

//-----------------------------------------------------------------------------
int pbCLEye::cameraCount()
{
#ifndef PS3EYELibDisable
	return CLEyeGetCameraCount();
#else
	return 0;
#endif
}

//-----------------------------------------------------------------------------
bool pbCLEye::startCamera(	int device,
						int w, int h,	
						int fps,		
						bool color,		
						bool mirrorX						
						)		
{

	#ifndef PS3EYELibDisable
	//CLEyeCapture *cam = (CLEyeCapture *)_cam;
	CLEyeCameraResolution resolution = ( w == 320 && h == 240 ) ? CLEYE_QVGA : CLEYE_VGA;
	CLEyeCameraColorMode mode = ( color ) ? CLEYE_COLOR_PROCESSED : CLEYE_MONO_PROCESSED;
	
	CLEyeCapture *cam = new CLEyeCapture(resolution, mode, fps, mirrorX );
	cam->setCallback( _callback, _callbackParam );

	_cam = cam;
	


	// start capture
	bool result = cam->Start();	

	return result;
#else
	return false;
#endif
}

//-----------------------------------------------------------------------------
bool pbCLEye::stopCamera()
{
	#ifndef PS3EYELibDisable
	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( !cam ) return false;

	_cam = 0;
	cam->Stop();	
	delete cam;
	#endif

	return true;
}

//-----------------------------------------------------------------------------
bool pbCLEye::isFrameNew()
{
	#ifndef PS3EYELibDisable
	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( !cam ) return false;	
	return cam->isFrameNew();
#else
	return 0;
#endif
}

//-----------------------------------------------------------------------------
Mat &pbCLEye::grabFrame() 
{
#ifndef PS3EYELibDisable
	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( cam ) {
		Mat frame = Mat( cam->grabFrame() );
		cvtColor( frame, _frame, CV_RGBA2BGR );

		/*split( frame, _planes );
		vector<Mat> planes(3);
		planes[0] = _planes[0];
		planes[1] = _planes[1];
		planes[2] = _planes[2];
		merge( planes,  _frame );*/
	}
#endif
	return _frame;
}

//-----------------------------------------------------------------------------
bool pbCLEye::setParameter( CamParamID param, int value )
{
#ifndef PS3EYELibDisable
	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( !cam ) return false;
	return cam->setParameter( (CLEyeCameraParameter)param, value );
#else
	return false;
#endif
}

//-----------------------------------------------------------------------------
int pbCLEye::getParameter( CamParamID param )
{
#ifndef PS3EYELibDisable
	CLEyeCapture *cam = (CLEyeCapture *)_cam;
	if ( !cam ) return 0;
	return cam->getParameter( (CLEyeCameraParameter)param );
#else
	return 0;
#endif

}

//----------------------------------------------------------------------------