#pragma once

//���������� ������� ��� ��������� ��������


#include "ofMain.h"
#include <cv.h>
#include "ofxVectorMath.h"

using namespace std;
using namespace cv;

class pbMotionHistory
{
public:
	pbMotionHistory(void);
	~pbMotionHistory(void);

	void setup( float timeFilter = 0.1f, 			//�������� ������� ������ �����, ��� ������ - ��� ������� �������
				float thresh = 30.0f / 255.0f		//����� ����������� �������� ������� � �������� �����, �� 0 �� 1
		);	
	void updateCamera( float dt, const Mat &image, bool debugDraw );
	void update( float dt );
	const Mat &image32bit() { return _imageHist32; }
	const Mat &image8bit();
	const Mat &diffBinary() { return _diffBin8; }	//������� ����� �������� � ������� ������, ��������

private:
	Mat _image32;		//������� �����, 32-������
	Mat _imageHist32;	//�������� �����
	Mat _imageHist8;	//�������� �����
	
	float _kFilter;
	float _kThresh;

	Mat _diff;
	vector<Mat> _diffPlanes;
	Mat _diffGray;
	Mat _diffBin32, _diffBin8;


};
