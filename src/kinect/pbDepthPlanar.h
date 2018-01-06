#pragma once

//��������� ������� �����������, ��������� �����.
//! ��������, ������ ���� ������ �������, ����� ������������ pbDepthPlanar3D

#include "ofMain.h"
#include "cv.h"
#include "pbCameraCalibrate.h"
using namespace cv;

//���������
struct pbDepthPlanarParam {
	int w, h;					//������� ��������, � ������� ���������������
	float depthLow, depthHigh;	//������ ���������
	int depthErode;				//����� ������ ��� �������� �����
	int filterMinCount;			//���������� ����� ��� ���������� �� �������

};

class pbDepthPlanar
{
public:
	pbDepthPlanar(void);
	~pbDepthPlanar(void);

	void setup( const string &paramFileName, const string &calibrFileName );
	void update( const Mat &depth16 );

	
	void lock();			//��� ������������� ��������
	void unlock();

	const Mat &mask();			//�������������� �����

	vector<ofPoint> getWarpPoints();		//����� ����������
	void setWarpPoints( vector<ofPoint> &p );

	pbDepthPlanarParam getParams();			//���������
	void setParams( pbDepthPlanarParam &param );

	void learnBackground();	//������� ���
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
	pbDepthPlanarParam _param;
	pbCameraCalibrate _calibrDepth;

	void loadParam();
	void saveParam();


	//�����������
	Mat _depthSmall16;
	Mat _depthSmallF;
	Mat _depthWarped;
	Mat _back;
	Mat _backWarped;

	Mat _tempDiff;
	Mat _tempMask;
	Mat _mask;
	
	float _startTime;
	bool _learnBackground;			//����� ��������� ���

	//�������� ����� �� �����
	static void maskDenoiseFloodFill( const Mat &mask, Mat &maskOut, int minCount, int maxCount );

	//���������� �����
	static void makeMask( const Mat &diff, Mat &tempMask, Mat &maskOut, 
		const pbDepthPlanarParam &param,
		bool debugDraw );

	//������������� ��� ��� ����� ����������
	void recalcBackground();


};
