#include "pbKinect.h"

Mat _pbKinectNullMat;

/*
#include "../../../_Kinect/Kinect-win32.h"
#include "../../../_Kinect/Kinect-Utility.h"

class Listener;
Kinect::KinectFinder *_KF = 0;
Kinect::Kinect *_K = 0;
Listener *_L = 0;


//-----------------------------------------------------------------------------
void pbKinect::KinectDepthToWorld(float &x, float &y, float &z)
{
	Kinect::KinectDepthToWorld( x, y, z );
}

void pbKinect::KinectWorldToRGBSpace(float &x, float &y, float z)
{
	Kinect::KinectWorldToRGBSpace(x, y, z);
}

float pbKinect::KinectDepthValueToZ(unsigned short Depth)
{
	return Kinect::Kinect_DepthValueToZ(Depth);
	//return 100.0f/(-0.00307f * (float)Depth + 3.33f);
}

unsigned short pbKinect::KinectZToDepthValue( float z )
{
	//Обращение формулы z = 100.0f/(-0.00307f * (float)Depth + 3.33f)
	return ( 100.0f / z - 3.33f ) / (-0.00307f);
};

bool pbKinect::KinectIsDepthValid(unsigned short Depth)
{
	return Kinect::Kinect_IsDepthValid(Depth);
}

//-----------------------------------------------------------------------------
//Построить маску допустимых значений
void pbKinect::KinectDepthToValidMask( const Mat &depth, Mat &validMask )
{
	inRange( depth, KinectDepthRangeLow, KinectDepthRangeHigh, validMask );
}

//Конвертировать к floating-point
void pbKinect::KinectDepthToFloatDepth( const Mat &depth, Mat &floatDepth )
{
	depth.convertTo( floatDepth, CV_32FC1, -1.0 /  pbKinect::KinectDepthRangeHigh, 1.0 );
}

//-----------------------------------------------------------------------------
// the listener callback object. Implement these methods to do your own processing

class Listener: public Kinect::KinectListener
{
public:
	Listener(): Kinect::KinectListener()
	{
		_colorFrameNew = false;
		_depthFrameNew = false;		
	}

	bool isFrameNew() { return _colorFrameNew && _depthFrameNew; }

	Mat &grabColorFrameCV()
	{
		_colorFrameNew = false;
		//TODO тут используется глобальная переменная _K
		Kinect::Kinect *K = _K;
		_colorFrame = Mat( cv::Size( Kinect::KINECT_COLOR_WIDTH, Kinect::KINECT_COLOR_HEIGHT ), CV_8UC3, K->mColorBuffer ); 
		return _colorFrame;
	}

	Mat &grabDepthFrameCV()
	{
		_depthFrameNew = false;
		//TODO тут используется глобальная переменная _K
		Kinect::Kinect *K = _K;
		_depthFrame = Mat( cv::Size( Kinect::KINECT_DEPTH_WIDTH, Kinect::KINECT_DEPTH_HEIGHT ), CV_16UC1, K->mDepthBuffer );
		return _depthFrame;
	}


	virtual void KinectDisconnected(Kinect::Kinect *K) 
	{
		printf("Kinect disconnected!\n");
	};

	// Depth frame reception complete. this only means the transfer of 1 frame has succeeded. 
	// No data conversion/parsing will be done until you call "ParseDepthBuffer" on the kinect 
	// object. This is to prevent needless processing in the wrong thread.
	virtual void DepthReceived(Kinect::Kinect *K) 
	{
		K->ParseDepthBuffer();		

		_depthFrameNew = true;

		//cout << "depth" << endl;

		// K->mDepthBuffer is now valid and usable!
		// see Kinect-Demo.cpp for a more complete example on what to do with this buffer


		//::glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,640,480, GL_ALPHA,GL_UNSIGNED_SHORT, mKinect->mDepthBuffer);
		//unsigned short Depth = mKinect->mDepthBuffer[i];
		//if (Kinect::Kinect_IsDepthValid(Depth))
		//float D = Kinect::Kinect_DepthValueToZ(Depth);
		//Kinect::KinectDepthToWorld(x,y,z);;

	};

	// Color frame reception complete. this only means the transfer of 1 frame has succeeded. 
	// No data conversion/parsing will be done until you call "ParseColorBuffer" on the kinect 
	// object. This is to prevent needless processing in the wrong thread.
	virtual void ColorReceived(Kinect::Kinect *K) 
	{
		K->ParseColorBuffer();
		_colorFrameNew = true;

		//cout << "color" << endl;

		// K->mColorBuffer is now valid and usable!
		// see Kinect-Demo.cpp for a more complete example on what to do with this buffer


		//::glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,640,480, GL_RGB,GL_UNSIGNED_BYTE, mKinect->mColorBuffer);

	};

	// not functional yet:
	virtual void AudioReceived(Kinect::Kinect *K) {};
private:
	bool _colorFrameNew;
	bool _depthFrameNew;	

	Mat _colorFrame, _depthFrame;
};

//-----------------------------------------------------------------------------
void initKF() 
{
	if ( !_KF ) {
		_KF = new Kinect::KinectFinder();
	}
}

void freeKF() {
	if ( _KF ) {
		delete _KF;
		_KF = 0;
	}
}


//-----------------------------------------------------------------------------
void pbKinect::setup()
{
	_cam = 0;	
	_mirrorX = false;


}

void pbKinect::close()
{
	freeKF();
}

//-----------------------------------------------------------------------------
int pbKinect::cameraCount()
{
	initKF();
	return _KF->GetKinectCount();
}

//-----------------------------------------------------------------------------
bool pbKinect::startCamera(	int device,
						   int w, int h,	//320 x 240 или 640 x 480
						   int fps,		//320 x 240: 30, 60, 90, 120, 150, 187
						   //640 x 480: 30, 60, 75
						   bool color,		//1- или 4-байтный
						   bool mirrorX						
						   )		
{


	bool result = true;

	_mirrorX = mirrorX;

	initKF();

	_K = _KF->GetKinect();
	if (_K == 0)
	{
		cout << "ERROR getting Kinect...\n";
		return false;
	};

	// create a new Listener instance
	_L = new Listener();

	// register the listener with the kinect. Make sure you remove the 
	// listener before deleting the instance! A good place to unregister 
	// would be your listener destructor.
	_K->AddListener(_L);

	// SetMotorPosition accepts 0 to 1 range
	//_K->SetMotorPosition(1);

	// Led mode ranges from 0 to 7, see the header for possible values
	_K->SetLedMode(Kinect::Led_Green);
	


	return result;
}

//-----------------------------------------------------------------------------
bool pbKinect::stopCamera()
{
	if ( _K ) {  
		// remove and delete the listener instance
		_K->RemoveListener(_L);
		delete _L;
		_L = 0;

		//turn the led off
		_K->SetLedMode(Kinect::Led_Off);

		delete _K;
		_K = 0;
	}

	return true;
}

//-----------------------------------------------------------------------------
bool pbKinect::isFrameNew()
{
	if ( _L ) {
		return _L->isFrameNew();
	}	
	return false;
}

Mat _tempKinect;

//-----------------------------------------------------------------------------
Mat &pbKinect::grabColorFrameCV() //Быстрая. Картинку не удалять и не менять
{
	if ( _L ) {
		Mat &frame = _L->grabColorFrameCV();
		if ( _mirrorX ) {
			flip( frame, _frame, 1 );
			return _frame;
		}
		else return frame;
	}
	return _tempKinect;
}

//-----------------------------------------------------------------------------
Mat &pbKinect::grabDepthFrameCVRaw() //Быстрая. Картинку не удалять и не менять
{
	if ( _L ) {
		Mat &frame = _L->grabDepthFrameCV();
		if ( _mirrorX ) {
			flip( frame, _depthFrame, 1 );
			return _depthFrame;
		}
		else return frame;
	}
	return _tempKinect;
}

//----------------------------------------------------------------------------
Mat &pbKinect::grabDepthFrameCVFloat()	//Точность высокая, значения 0..1
{
	Mat &depthRaw = grabDepthFrameCVRaw();	
	KinectDepthToFloatDepth( depthRaw, _depthFrameFloat );
	return _depthFrameFloat;
}

//----------------------------------------------------------------------------
Mat &pbKinect::grabDepthFrameCV8bit()	//Точность низкая, для визуализации
{
	Mat &depthFloat = grabDepthFrameCVFloat();
	depthFloat.convertTo( _depthFrame8bit, CV_8UC1, 255.0, 0.0 );
	return _depthFrame8bit;
}

//----------------------------------------------------------------------------
*/