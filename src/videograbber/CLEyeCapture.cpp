#include "CLEyeCapture.h"

//------------------------------------------------------------------
CLEyeCapture::CLEyeCapture(CLEyeCameraResolution resolution, CLEyeCameraColorMode mode, int fps,
						   bool mirrorX						   
						   ) :
_resolution(resolution), 
_mode(mode), 
_fps(fps), 
_running(false),
_mirrorX( mirrorX ),
_callback( 0 ),
_callbackParam( 0 )
{
	_cameraGUID = CLEyeGetCameraUUID(0);
	if(_mode == CLEYE_COLOR_PROCESSED || _mode == CLEYE_COLOR_RAW)
		_isColor = true;
	else
		_isColor = false;

	pCapImage = 0;
	_isFrameNew = false;
}

//------------------------------------------------------------------
void CLEyeCapture::setCallback( pbVideoGrabberCallbackType callback, void *param )
{
	_callback = callback;
	_callbackParam = param;
}

//------------------------------------------------------------------
bool CLEyeCapture::Start()
{
	_running = true;
	// Start CLEyeCapture image capture thread
	_hThread = CreateThread(NULL, 0, &CLEyeCapture::CaptureThread, this, 0, 0);
	if(_hThread == NULL)
	{
		MessageBox(NULL,"Could not create capture thread","CLEyeCapture", MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

//------------------------------------------------------------------
void CLEyeCapture::Stop()
{
	_isFrameNew = false;

	if(!_running)	return;
	_running = false;
	WaitForSingleObject(_hThread, 2000);
}

//------------------------------------------------------------------
void CLEyeCapture::Run()
{
	int w, h;	

	// Create camera instances
	_cam = CLEyeCreateCamera(_cameraGUID, _mode, _resolution, _fps);
	if(_cam == NULL)	return;

	// Get camera frame dimensions
	CLEyeCameraGetFrameDimensions(_cam, w, h);
	// Create the OpenCV images
	pCapImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, _isColor ? 4 : 1);

	// Set some camera parameters
	CLEyeSetCameraParameter(_cam, CLEYE_GAIN, 0);
	CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, 511);

	if ( _mirrorX ) {
		CLEyeSetCameraParameter(_cam, CLEYE_HFLIP, 1 );
	}

	// Start capturing
	CLEyeCameraStart(_cam);
	Sleep(100);

	// image capturing loop
	while(_running)
	{
		PBYTE pCapBuffer;
		// Capture camera images
		cvGetImageRawData(pCapImage, &pCapBuffer);
		CLEyeCameraGetFrame(_cam, pCapBuffer);
		if ( _callback ) {
			_callback( Mat(pCapImage), _callbackParam );
		}

		_isFrameNew = true;
		//cvShowImage(OUTPUT_WINDOW, pCapImage);
	}
	Sleep(1000);
	// Stop camera capture
	CLEyeCameraStop(_cam);
	// Destroy camera object
	CLEyeDestroyCamera(_cam);
	// Destroy the allocated OpenCV image
	cvReleaseImage(&pCapImage);
	_cam = NULL;
}

//------------------------------------------------------------------
DWORD WINAPI CLEyeCapture::CaptureThread(LPVOID instance)
{
	// seed the RNG with current tick count and thread id
	srand(GetTickCount() + GetCurrentThreadId());
	// forward thread to Capture function
	CLEyeCapture *pThis = (CLEyeCapture *)instance;
	pThis->Run();
	return 0;
}

//------------------------------------------------------------------
bool CLEyeCapture::setParameter( CLEyeCameraParameter param, int value )
{
	return CLEyeSetCameraParameter(_cam, param, value );
}

int CLEyeCapture::getParameter( CLEyeCameraParameter param )
{
	return CLEyeGetCameraParameter( _cam, param );
}

//------------------------------------------------------------------

IplImage *CLEyeCapture::grabFrame()
{
	_isFrameNew = false;
	return pCapImage;
}

bool CLEyeCapture::isFrameNew()
{
	return _isFrameNew;
}

//------------------------------------------------------------------
