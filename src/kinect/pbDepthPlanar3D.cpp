#include "pbDepthPlanar3D.h"
#include "highgui.h"
#include "ofxIniSettings.h"

#include "pbDepthPlanar.h"
#include "pbFiles.h"
#include "pbKinector.h"
#include "ofxVectorMath.h"

float kBackAutoSetSec = 3.0;			//������� ����� ��� ����� ������ �������, ���� ���������� ���

//------------------------------------------------------------
XnPoint3D makeXnPoint3D( const ofPoint &p0 ) {
	XnPoint3D p;
	p.X = p0.x;
	p.Y = p0.y;
	p.Z = p0.z;
	return p;
}

void makeXnPoint3DVector( const vector<ofPoint> &p0, vector<XnPoint3D> &p ) {
	int n = p0.size();
	p.resize( n );
	for (int i=0; i<n; i++) {
		const ofPoint &p0i = p0[i];
		XnPoint3D &pi = p[i];
		pi.X = p0i.x;
		pi.Y = p0i.y;
		pi.Z = p0i.z;
	}
}

void makeofPointVector( const vector<XnPoint3D> &p0, vector<ofPoint> &p ) {
	int n = p0.size();
	p.resize( n );
	for (int i=0; i<n; i++) {
		const XnPoint3D &p0i = p0[i];
		ofPoint &pi = p[i];
		pi.x = p0i.X;
		pi.y = p0i.Y;
		pi.z = p0i.Z;
	}
}

//------------------------------------------------------------
//���������� �������� �� ���
inline void backProjectionPoint( const XnPoint3D &p0, const ofxVec3f &norm, float originAxe,
						   XnPoint3D &p, float &height ) {
							   height = norm.x * p0.X + norm.y * p0.Y + norm.z * p0.Z - originAxe;
							   p.X = p0.X - height * norm.x;
							   p.Y = p0.Y - height * norm.y;
							   p.Z = p0.Z - height * norm.z;

}

//---------------------------------------------------------------------------
pbDepthPlanar3D::pbDepthPlanar3D(void)
{
#ifdef TARGET_WIN32 
	InitializeCriticalSection(&critSec); 
#else 
	pthread_mutex_init(&myMutex, NULL); 
#endif 
}

//---------------------------------------------------------------------------
pbDepthPlanar3D::~pbDepthPlanar3D(void)
{
}


//---------------------------------------------------------------------------
void pbDepthPlanar3D::lock(){ 

#ifdef TARGET_WIN32 
	EnterCriticalSection(&critSec); 
#else 
	pthread_mutex_lock(&myMutex); 
#endif 
} 

//------------------------------------------------------------ 
void pbDepthPlanar3D::unlock(){ 

#ifdef TARGET_WIN32 
	LeaveCriticalSection(&critSec); 
#else 
	pthread_mutex_unlock(&myMutex); 
#endif 
} 

//------------------------------------------------------------ 
void pbDepthPlanar3D::setup( const string &paramFileName, const string &calibrFileName )
{
	lock();
	//���������
	_param.w = 320;
	_param.h = 240;

	_param.depthLow = 100;
	_param.depthHigh = 300;

	_param.doBackProjection = false;
	_param.depthCalibrLow = 10; 
	_param.depthCalibrHigh = 300;
	_param.depthRawLow = 10;
	_param.depthRawHigh = 300;

	_param.depthErode = 3;
	_param.depthDilate = 3;
	_param.kWarpedThreshold = 128;	
	_param.blobMinArea = 50;

	_cameraW = 0;	//������ ������������ �� ������
	_cameraH = 0;

	ofxIniSettings ini;
	ini.load( paramFileName );
	_param.w		= ini.get( "Floor.processW", _param.w );	
	_param.h		= ini.get( "Floor.processH", _param.h );	

	_param.doBackProjection	= ini.get( "Floor.doBackProjection", _param.doBackProjection );	
	_param.depthCalibrLow	= ini.get( "Floor.depthCalibrLow", _param.depthCalibrLow );	
	_param.depthCalibrHigh	= ini.get( "Floor.depthCalibrHigh", _param.depthCalibrHigh );	
	_param.depthRawLow		= ini.get( "Floor.depthRawLow", _param.depthRawLow );	
	_param.depthRawHigh		= ini.get( "Floor.depthRawHigh", _param.depthRawHigh );	

	_param.kWarpedThreshold	= ini.get( "Floor.kWarpedThreshold", _param.kWarpedThreshold );
	_param.depthErode		= ini.get( "Floor.kDepthErode", _param.depthErode );	
	_param.depthDilate		= ini.get( "Floor.kDepthDilate", _param.depthDilate );	
	_param.blobMinArea		= ini.get( "Floor.blobMinArea", _param.blobMinArea );	

	_param.backgroundLearnTime = ini.get( "Floor.backgroundLearnTime", 1.0f );


	//���������� �������, ����������� � �������� ������ �������
	loadParam();

	//����������
	_calibrDepth.setup( _param.w, _param.h, calibrFileName );

	_startTime = -1;			//��� �� ����������
	_time = 0;
	_learnBackgroundAtStart = true;
	_learnBackground = false;
	_learnBackgroundTime = 0;
	_backOriginAxe = 0;


	_mask = Mat( cv::Size( _param.w, _param.h ), CV_8UC1 );
	_mask.setTo( 0 );

	unlock();
}

//------------------------------------------------------------ 
void pbDepthPlanar3D::backProjection( Mat &inputMask, Mat &inputDepth, Mat &maskOut )
{
	int w = _param.w;
	int h = _param.h;
	float scale = 1.0 * _cameraW / _param.w;			
	float invScale = 1.0 / scale;	

	//������ ������ ����� � �������� �����������
	if ( _pScreen.size() == 0 ) {
		_pScreen.resize( w * h );
		_pWorld.resize( w * h );
		_pWorldFloor.resize( w * h );
		_pScreenFloor.resize( w * h );
		_pFloor.resize( w * h );
		_pGame.resize( w * h );
	}

	XnPoint3D *pScreen = &_pScreen[0];		//��� �������� �������
	XnPoint3D *pWorld = &_pWorld[0];

	int n = 0;					//���������� �����


	//TODO �������������� - �������� � ���������� �������������� �� ������������� �����
	for ( int y=0; y<h; y++ ) {
		unsigned char *maskY = inputMask.ptr<unsigned char>(y);
		unsigned short *depthY = inputDepth.ptr<unsigned short>(y);
		for ( int x=0; x<w; x++ ) {
			if ( maskY[ x ] > 0 ) {
				XnPoint3D &p = pScreen[ n++ ];
				p.X = x * scale;
				p.Y = y * scale;
				p.Z = depthY[ x ];				
			}
		}
	}

	if ( n > 0 ) {
		int n1 = 0;					//����� �����, ������� ���������� ����� ���������� ������ ����� ���������� �������� �� ���
		//��������� � ������� ����������
		shared_kinect.convertScreenToWorld( n, pScreen, pWorld );

		//���������� ����� �� ���
		//������ �����
		float kBackHeightLow = _param.depthLow;									
		float kBackHeightHigh = _param.depthHigh;

		//TODO ��� �� �������������� �� ��������� ������ - �������� ���� ���
		//vector<float> height( n );
		XnPoint3D *pnt = &_pWorldFloor[0];
		XnPoint3D pntTemp;
		float heightTemp;
		n1 = 0;
		for (int i=0; i<n; i++) {
			backProjectionPoint( pWorld[ i ], _backN, _backOriginAxe, pntTemp, heightTemp );
			//���������� �� ������
			if ( heightTemp >= kBackHeightLow && heightTemp <= kBackHeightHigh ) {
				pnt[n1] = pntTemp;				
				//height[n1] = heightTemp;
				n1++;
			}
		}
		//cout << "n " << n << "\t n1 " << n1 << endl; 
		if ( n1 > 0 ) {
			//��������� ������� � ��������
			shared_kinect.convertWorldToScreen( n1, &_pWorldFloor[0], &_pScreenFloor[0] );
			//cout << "1 ";
			//��������� � ����� � warp-ing
			Mat &img = _tempMaskBackProj;
			if ( img.empty() ) {
				img = Mat( cv::Size( w, h ), CV_8UC1 );
			}
			img.setTo( 0 );
			for ( int i=0; i<n1; i++ ) {
				XnPoint3D &p = _pScreenFloor[i];
				int x1 = p.X * invScale;
				int y1 = p.Y * invScale;
				if ( x1 >= 0 && x1 < w && y1 >= 0 && y1 < h ) {
					img.at<unsigned char>( y1, x1 ) = 255;			//TODO ����� �����������!!!!
					//int h0 = img.at<unsigned char>( y1, x1 );		//����� ���� �� ��������� ������
					//int h_ = min( height[i] * 10, 255.0f );
					//if ( h0 < h_ ) {
					//	img.at<unsigned char>( y1, x1 ) = h_;
					//}
				}
			}
			//imshow( "floor", img );
			//cout << "2 ";
			//Mat imgWarp;
			
			//�������������� � �������				//TODO ����� ��������, �������� map!
			_calibrDepth.transformImage( img, maskOut );


			//imshow( "floor warp", imgWarp );
	
		//cout << "3 ";

			//������� ����������� � �������
			/*{
				_pFloor.resize( n1 );
				XnPoint3D *pnt = &_pScreenFloor[0];
				Point2f *cvPnt = &_pFloor[0];
				for (int i=0; i<n1; i++) {
					cvPnt->x = pnt->X * invScale;
					cvPnt->y = pnt->Y * invScale;
					pnt++;
					cvPnt++;
				}
			}

			//cout << "4 "; 
			_calibrDepth.transformPixels( _pFloor, _pGame );
			//cout << "5 "; 

			//�������� ���������
			{
				Mat img( cv::Size( w, h ), CV_8UC1 );
				img.setTo( 0 );
				for ( int i=0; i<n1; i++ ) {
					Point2f &p = _pGame[i];
					int x1 = ofClamp( p.x, 0, w-1 );
					int y1 = ofClamp( p.y, 0, h-1 );
					img.at<unsigned char>( y1, x1 ) = 255;
				}
				imshow( "test game", img );
			}

			//cout << "end "; 
			cout << endl;*/
		}
	}
}

//------------------------------------------------------------ 

void pbDepthPlanar3D::update( const Mat &depth16, bool calibrMode )
{
	lock();
	
	//calibrMode = true;			//TEST

	float time = ofGetElapsedTimef();
	if ( _startTime < 0 ) _startTime = time;
	float dt = min( time - _time, 0.1f );
	_time = time;

	if ( _cameraW == 0 ) {
		_cameraW = depth16.size().width;
		_cameraH = depth16.size().height;
	}

	//��������� ������ ��� ��������		//TODO � ����� �������� ��������, � ��� ������������� ��������� ��� ������
	int w = _param.w;
	int h = _param.h;
	float scale = 1.0 * _cameraW / _param.w;			
	float invScale = 1.0 / scale;			
	resize( depth16, _depthSmall, cv::Size( w, h ), 0.0, 0.0, CV_INTER_NN );	

	//�������������� ��� ���-��
	if ( _back.empty() ) {
		_back = _depthSmall.clone();
		recalcBackground();
	}

	//����������� ���� ��� ������ ���������� ����� ��������� ������
	if ( _learnBackgroundAtStart && time >= _startTime + kBackAutoSetSec ) {
		learnBackground();
		_learnBackgroundAtStart = false;
	}

	//����� ����������� ����
	if (  _learnBackgroundTime > 0 ) {
		_learnBackgroundTime -= dt;

		if ( _learnBackground ) {
			_back = _depthSmall.clone();		//� ������ ��� - ��������
			_learnBackground = false;
			//cout << "Background inited" << endl;
		}
		else {									//����� - ����� �������
			_back = cv::min( _back, _depthSmall );
			//cout << "Background update" << endl;
		}
		recalcBackground();
	}

	//������� ��������		!! TODO ������ ����� ����� �� ������� unsigned short, ������� �������� ����� �������������� � CV_16SC1
	_tempDiff = _back - _depthSmall;

	//������ ����� �����, ��� ������� �������� � ������ ��������
	float diffLow = _param.depthLow;			//� ��					
	float diffHigh = _param.depthHigh;
	if ( calibrMode ) {
		diffLow = _param.depthCalibrLow;
		diffHigh = _param.depthCalibrHigh;
	}
	if ( _param.doBackProjection && !calibrMode ) {
		diffLow = _param.depthRawLow;
		diffHigh = _param.depthRawHigh;
	}
	cv::inRange( _tempDiff, diffLow, diffHigh, _tempMask );
	//imshow( "filtered", _tempMask );
	//cout << "thresh \t" << diffLow << "\t" << diffHigh << endl;

	//���������� ����� � ����������� �������� ����
	bool doBackProj = !calibrMode && _param.doBackProjection;
	if ( !doBackProj ) {
		//�� ���� �������� ��������
		_calibrDepth.transformImage( _tempMask, _preMask );
	}
	else {
		//�������� ��������
		backProjection( _tempMask, _depthSmall, _preMask );
	}

	//imshow( "raw warp", _preMask );


	//���������� ����� - ������ ���� �� �����������
	if ( !calibrMode ) {
		//������ ����� ��������
		threshold( _preMask, _preMask, _param.kWarpedThreshold, 255, CV_THRESH_BINARY );

		//imshow( "warp thresh", _preMask );

		//������ �� �����
		maskFilter( _preMask, _mask, _tempMaskFiltration );

		//imshow( "mask", _mask );

	}
	else {
		_mask = _preMask.clone();
	}

	unlock();
}



//------------------------------------------------------------
//������������� ��� ��� ����� ����������
void pbDepthPlanar3D::recalcBackground()
{
	if ( !_back.empty() ) {
		int w = _back.size().width;
		int h = _back.size().height;
		int scale = _cameraW / w;

		//���������� ������� ��������� ����� �������� ����
		vector<ofPoint> points = _calibrDepth.points();
		int n = points.size();
		for (int i=0; i<n; i++) {
			ofPoint &p = points[i];
			p.x = ofClamp( p.x, 0, w-1 );
			p.y = ofClamp( p.y, 0, h-1 );
			p.z = _back.at<unsigned short>( int(p.y), int(p.x) );
			p.x *= scale;
			p.y *= scale;
		}
		vector<XnPoint3D> pScreen( n );
		vector<XnPoint3D> pWorld( n );
		makeXnPoint3DVector( points, pScreen );
		shared_kinect.convertScreenToWorld( n, &pScreen[0], &pWorld[0] );

		//���� ����������� ������� �������� ���� ������� ������������ ���� �������� �������������
		//TODO ! ����� ��������, ��� ������������ �������������
		//TODO � ����������� - ������� ��� �� ���� ��������� ������ ����������� �����
		vector<ofPoint> pnt;
		makeofPointVector( pWorld, pnt );
		ofxVec3f N1 = pnt[2] - pnt[0];
		N1.cross( pnt[1] - pnt[0] );
		ofxVec3f N2 = pnt[3] - pnt[0];
		N2.cross( pnt[2] - pnt[0] );
		_backN = -(N1+N2).getNormalized();
		//����������� �����
		ofxVec3f Center( 0, 0, 0 );
		for (int i=0; i<n; i++) {
			Center += pnt[i];
		}
		Center /= n;
		_backOriginAxe = _backN.dot( Center );	//����������� �������� �� ������

	
	}

}

//------------------------------------------------------------ 
const Mat &pbDepthPlanar3D::mask()						//�������������� �����
{
	//��� �� ���� lock, ��� ��� ������ �� ����������
	return _mask;
}

//------------------------------------------------------------ 
vector<ofPoint> pbDepthPlanar3D::getWarpPoints()		//����� ����������
{
	lock();
	vector<ofPoint> p = _calibrDepth.points();
	unlock();
	return p;
}

//------------------------------------------------------------ 
void pbDepthPlanar3D::setWarpPoints( vector<ofPoint> &p )
{
	lock();
	_calibrDepth.setPoints( p );
	recalcBackground();		//������������� ���
	unlock();
}

//------------------------------------------------------------ 
void pbDepthPlanar3D::learnBackground()	//������� ���
{
	lock();
	_learnBackground = true;
	_learnBackgroundTime = _param.backgroundLearnTime;
	unlock();
}

//------------------------------------------------------------ 
void pbDepthPlanar3D::resetCorners()	//�������� ����
{
	lock();
	_calibrDepth.resetPoints();
	recalcBackground();		//������������� ���
	unlock();
}

//-----------------------------------------------------------------
pbDepthPlanar3DParam pbDepthPlanar3D::getParams()			//���������
{
	return _param;
}

//-----------------------------------------------------------------
void pbDepthPlanar3D::setParams( pbDepthPlanar3DParam &param )
{
	_param = param;
	saveParam();
}

//-----------------------------------------------------------------
void pbDepthPlanar3DParam::load( const string &fileName )
{
	vector<pbFiles::PairFloat> list = pbFiles::readPairs( fileName );
	depthLow		= pbFiles::findValue( list, "depthLow", depthLow );
	depthHigh	= pbFiles::findValue( list, "depthHigh", depthHigh );
	blobMinArea = pbFiles::findValue( list, "blobMinArea", blobMinArea );
	depthErode = pbFiles::findValue( list, "depthErode", depthErode );
	depthDilate = pbFiles::findValue( list, "depthDilate", depthDilate );
}

//-----------------------------------------------------------------
void pbDepthPlanar3DParam::save( const string &fileName )
{
	vector<pbFiles::PairFloat> list;
	list.push_back( pbFiles::PairFloat( "depthLow", depthLow ) );
	list.push_back( pbFiles::PairFloat( "depthHigh", depthHigh ) );
	list.push_back( pbFiles::PairFloat( "blobMinArea", blobMinArea ) );
	list.push_back( pbFiles::PairFloat( "depthErode", depthErode ) );
	list.push_back( pbFiles::PairFloat( "depthDilate", depthDilate ) );

	pbFiles::writePairs( list, fileName );
}

//-----------------------------------------------------------------
//������/���������� ����������
void pbDepthPlanar3D::loadParam()
{
	_param.load( "data/_thresh.dat" );
}

//-----------------------------------------------------------------
void pbDepthPlanar3D::saveParam()
{
	_param.save( "data/_thresh.dat" );
}

//-----------------------------------------------------------------
void pbDepthPlanar3D::loadDefaultParam()
{
	_param.load( "data/_threshDefault.dat" );
	saveParam();
}

//-----------------------------------------------------------------
void pbDepthPlanar3D::saveDefaultParam()
{
	_param.save( "data/_threshDefault.dat" );
}


//-----------------------------------------------------------------
//�������� ����� � ������� floodFill
//TODO ������� � ����� ���������
//���� minCount = -1, �� ��� �� ������������. ���������� � maxCount.
void pbDepthPlanar3D::maskDenoiseFloodFill( const Mat &mask, Mat &maskOut, int minCount, int maxCount )
{

	maskOut = mask.clone();
	Mat &img = maskOut;

	//������� �������� �����������
	int w = img.size().width;
	int h = img.size().height;
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			int value = img.at<uchar>(y, x);   
			if ( value == 255 ) {        //���� �������� - 255, �� ��������
				//��������� 200
				cv::Rect rect;        //���� ��������� ���������� �������������
				int count = cv::floodFill( img, cv::Point( x, y ), Scalar( 200 ), &rect );
				bool ok = true;
				if ( minCount >= 0 && count < minCount ) ok = false;
				if ( maxCount >= 0 && count > maxCount ) ok = false;
				if ( !ok ) {	//���� ������ - ��������
					floodFill( img, cv::Point( x, y ), Scalar( 0 ), &rect );
				}
			}
		}
	}
	threshold( img, img, 1, 255, CV_THRESH_BINARY );	//��� ������� � �������� 200 ��������� ������� � 255

}

//-----------------------------------------------------------------
//�������� �����, ���� ����������
void updateMorphMask( Mat &mask, int diam )
{
	if ( mask.size().width != diam ) {
		mask = Mat( cv::Size( diam, diam ), CV_8UC1 );
		mask.setTo( 0 );
		circle( mask, cv::Point( diam/2, diam/2 ), diam/2, 255, -1 );
	}
}

//-----------------------------------------------------------------
void pbDepthPlanar3D::maskFilter( const Mat &mask, Mat &maskOut, Mat &maskTemp )
{
	//������� �������
	//if ( nErode > 0 ) {
	//	maskTemp = mask.clone();
	//	for (int i=0; i < nErode; i++) {
	//		erode( maskTemp, maskTemp, cv::Mat() );		
	//	}
	//}
	//const Mat *input = ( nErode > 0 ) ? &maskTemp : &mask;

	//��������� ������ ����
	maskDenoiseFloodFill( mask, maskOut, _param.blobMinArea, -1 );

	//������� �������
	int nErode = _param.depthErode;
	if ( nErode > 0 ) {
		updateMorphMask( _erodeMask, nErode );
		erode( maskOut, maskOut, _erodeMask );		
		//imshow( "erode", _erodeMask );
	}

	//������� ���������
	int nDilate = _param.depthDilate;
	if ( nDilate > 0 ) {
		updateMorphMask( _dilateMask, nDilate );
		dilate( maskOut, maskOut, _dilateMask );	
		//��������� ���������, ��� ��� ��������� ��� ������� �������� � ����� ���� ������ �����
		threshold( maskOut, maskOut, 254, 255, CV_THRESH_BINARY );	
		//imshow( "dilate", _dilateMask );
	}


}

//-----------------------------------------------------------------
