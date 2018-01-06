#include "pbVideoGrabber.h"

//------------------------------------------------------------
void pbVideoGrabber::setup()
{
	_w = _h = 0;
	_useUSB = false;
	_mirrorX = false;

	_callback = 0;	

	psGrabber.setup();
	
}


//------------------------------------------------------------
void pbVideoGrabber::setCallback( pbVideoGrabberCallbackType callback, void *param )
{
	_callback = callback;
	_callbackParam = param;
	psGrabber.setCallback( _callback, _callbackParam );
}

//------------------------------------------------------------
int pbVideoGrabber::cameraCount()
{
	return 0;
}

//------------------------------------------------------------
bool pbVideoGrabber::startCamera(	int device,
					int w, int h,		
					int fps,
					//bool color,		
					bool mirrorX
				)
{
	//cout << "startCamera..." << endl;
	_w = w;
	_h = h;
	_mirrorX = mirrorX;
	_useUSB = true;
	bool result = false;
	
	
	if ( _useUSB ) {
		vidGrabber.listDevices();
		vidGrabber.setDeviceID( device );
		vidGrabber.setDesiredFrameRate( fps );		
		result = vidGrabber.initGrabber( w, h );
		vidGrabber.setUseTexture( false );

		_frame = Mat( h, w, CV_8UC3 );
	}


	string camType = "USB";

	if ( result ) {
		cout << "Camera: " << camType << " - ok"<< endl;
	}
	else {
		cout << "ERROR: Can not start camera: " << camType << ", " << device << endl;
	}
	return result;
}

//------------------------------------------------------------
bool pbVideoGrabber::stopCamera()
{
	if ( _useUSB ) {
		vidGrabber.close();		
	}
	return true;
}

//------------------------------------------------------------
bool pbVideoGrabber::isFrameNew()
{
	if ( _useUSB ) {
		vidGrabber.update();
		return vidGrabber.isFrameNew();
	}
	return false;
}

//------------------------------------------------------------
Mat _tempVideoGrabber;

Mat &pbVideoGrabber::grabFrameCV()	
{
	if ( _useUSB ) {
		//Webcam
		int w = vidGrabber.getWidth();
		int h = vidGrabber.getHeight();

		IplImage ipl = IplImage( _frame );
		unsigned char *pixels = vidGrabber.getPixels().getData();
		if( w == _w && h == _h ) {	
			if( ipl.width == ipl.widthStep ){
				memcpy( ipl.imageData, pixels, w*h*3 );
			}
			else {			
 		 		for( int i=0; i < h; i++ ) {
            		memcpy( ipl.imageData + (i*ipl.widthStep),
 	                   pixels + (i*w*3),
        	           w*3 );
        		}
			}	
			if ( _mirrorX ) {
				flip( _frame, _frame, 1 );
			}
		}
		else {
			cout << "grabFrameCV: video grabber gives different frame size" << endl;
		}
		
		if ( _callback ) {
			_callback( _frame, _callbackParam );
		}

		return _frame;
	}

	return _tempVideoGrabber;
}
	
//------------------------------------------------------------
Mat &pbVideoGrabber::grabFrameCVUnique( int procW, int procH )
{
	Mat &frame = grabFrameCV();
	cv::Size size = frame.size();
	

	if ( size.width == 0 || size.height == 0 || procW == 0 || procH == 0 
		|| ( size.width == procW && size.height == procH )
		) return frame;
	float scaleX = 1.0 * procW / size.width;
	float scaleY = 1.0 * procH / size.height;
	float scale = cv::max( scaleX, scaleY );
	int interpolation = ( scale >= 1 ) ? INTER_LINEAR : INTER_AREA;
	resize( frame, _resized, cv::Size(), scale, scale, interpolation );

	cv::Size procSize = _resized.size();

	cv::Rect r( (procSize.width - procW)/2, (procSize.height - procH)/2, procW, procH );

	if ( !_cropped.empty() && !(_cropped.size().width == procW && _cropped.size().width == procH ) ) {
		cout << "ERROR grabFrameCVUnique: different frame sizes" << endl;
		ofSleepMillis( 5000 );
		OF_EXIT_APP( 0 );
	}
	_cropped = _resized(r);


	return _cropped;

}

//------------------------------------------------------------
Mat pbVideoGrabber::grabFrameCVCopy( int procW, int procH )
{
	Mat &frame = grabFrameCV();
	cv::Size size = frame.size();

	Mat resized;
	Mat result;

	if ( size.width == 0 || size.height == 0 ) {
		return result;
	}

	float scaleX = 1.0 * procW / size.width;
	float scaleY = 1.0 * procH / size.height;
	float scale = cv::max( scaleX, scaleY );
	int interpolation = ( scale >= 1 ) ? INTER_LINEAR : INTER_AREA;
	resize( frame, resized, cv::Size(), scale, scale, interpolation );

	cv::Size procSize = resized.size();

	cv::Rect r( (procSize.width - procW)/2, (procSize.height - procH)/2, procW, procH );

	result = resized(r);

	return result;
}

//------------------------------------------------------------
Mat &pbVideoGrabber::grabDepthFrameCVFloat()	
{
	return _tempVideoGrabber;
}

//------------------------------------------------------------
Mat &pbVideoGrabber::grabDepthFrameCV8bit()
{
	return _tempVideoGrabber;
}

//------------------------------------------------------------
bool pbVideoGrabber::setParamsAllAuto( bool enable )
{
	if ( _useUSB ) {
		//TODO
		return false;
	}

	return false;
}


//------------------------------------------------------------
bool pbVideoGrabber::setParamGain( int value )
{
	if ( _useUSB ) {
		//TODO
		return false;
	}
	return false;
}

//------------------------------------------------------------
bool pbVideoGrabber::setParamExposure( int value )
{
	if ( _useUSB ) {
		//TODO
		return false;
	}
	return false;
}


//------------------------------------------------------------
