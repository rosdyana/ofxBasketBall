#pragma once

//������ 3�-����� �� ����/�����


#include "ofMain.h"
#include "cv.h"
#include "pbCameraCalibrate.h"
#include "ofxVectorMath.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

using namespace cv;

//���������
struct pbDepthPlanar3DParam {
	int w, h;					//������� ��������, � ������� ���������������

	int doBackProjection;		//������ ������� ����� �� ��� - ��������� ������, �� �������� ��������. ��� ���������� - �����������.
	float depthCalibrLow, depthCalibrHigh;	//������ ��������� ������ - ��� ����������
	float depthRawLow, depthRawHigh;	//������ ��������� ������ - ��� ������ ������� ��� ������������� BackProjection
	float depthLow, depthHigh;	//������ ��������� ������
	
	int kWarpedThreshold;		//����� ��������� ����������� warp-�����������
	int depthErode;				//����� ������ ��� �������� �����
	int depthDilate;				//����� ������ ��� �������� �����
	int blobMinArea;			//���������� ����� ��� ���������� �� �������

	float backgroundLearnTime;		//�������� ������� ����������� ����


	void load( const string &fileName );
	void save( const string &fileName );

};

class pbDepthPlanar3D
{
public:
	pbDepthPlanar3D(void);
	~pbDepthPlanar3D(void);

	void setup( const string &paramFileName, const string &calibrFileName );
	void update( const Mat &depth16, bool calibrateMode );

	void loadDefaultParam();
	void saveDefaultParam();
	
	void lock();			//��� ������������� ��������
	void unlock();

	const Mat &mask();			//�������������� �����

	vector<ofPoint> getWarpPoints();		//����� ����������
	void setWarpPoints( vector<ofPoint> &p );

	void setBackProjection( bool value ) { _param.doBackProjection = value; };		//������������ ������ �������� �������� � ������������ ����

	pbDepthPlanar3DParam getParams();			//���������
	void setParams( pbDepthPlanar3DParam &param );

	void learnBackground();	//������� ���
	bool learningBackground() { return _learnBackgroundTime > 0; }	//���� �� �������� ����
	void resetCorners();	//�������� ����

	pbCameraCalibrate &calibrator() { return _calibrDepth; }

private:

	//������
	#ifdef TARGET_WIN32
			CRITICAL_SECTION  critSec;  	//same as a mutex
	#else
			pthread_mutex_t  myMutex;
	#endif

	//���������
	pbDepthPlanar3DParam _param;
	pbCameraCalibrate _calibrDepth;

	void loadParam();
	void saveParam();



	//�����������
	int _cameraW, _cameraH;
	Mat _depthSmall;
	Mat _back;

	ofxVec3f _backN;
	float _backOriginAxe;


	Mat _tempDiff;
	Mat _tempMask;

	Mat _preMask;	

	vector<XnPoint3D> _pScreen, _pWorld, _pWorldFloor, _pScreenFloor;	//����� � �������� � ������� �����������
	vector<Point2f> _pFloor, _pGame;			//��������� � ������� ����������
	Mat _tempMaskBackProj;

	Mat _tempMaskFiltration;
	Mat _mask;
	
	float _startTime;
	float _time;
	bool _learnBackgroundAtStart;	//���� ����� ��� ��� ������
	bool _learnBackground;			//����� ��������� ���
	float _learnBackgroundTime;		//������� ���� �����������, ����� ����� 0 - ���������

	//������������� ��� ��� ����� ����������
	void recalcBackground();

	//������ �������� �������� ��� ����� ������� ���������������� ����
	void backProjection( Mat &inputMask, Mat &inputDepth, Mat &maskOut );

	//��������� �����
	void maskDenoiseFloodFill( const Mat &mask, Mat &maskOut, int minCount, int maxCount );
	void maskFilter( const Mat &mask, Mat &maskOut, Mat &maskTemp );

	Mat _erodeMask, _dilateMask;

};