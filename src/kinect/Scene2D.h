#pragma once

//�������� ��������� ����� � ������������� �����

#include "cv.h"
#include "ofMain.h"
#include "PersonType.h"


using namespace cv;



class Scene2D
{
public:
	void setup( float kTruncDepthMM0	//����������, �� ������� ��������
		
		);

	//����������� ���� unsigned short, �.�. CV_16UC1
	//nearPersonId - �������� ������ ������� �����
	void updateCamera( const Mat &depth, const Mat &labels, const Persons &persons, vector<int> &nearPersonIdLabel );
	void update( float dt );

	void draw();

	const Mat &mask() { return _mask; }
	const Mat &mask8() { return _mask8; }

	void getHeadData( vector<ofPoint> &pos, vector<int> &label );
	const vector<ofPoint> &heads() const { return _heads; }
	
private:

	unsigned short kTruncDepthMM;
	Mat _mask;				//16-������ �����, � ������������. ���� > 0, �� ������ ���-�� ����
	Mat _mask8;			//8-������, ������� �����, >0 ������ ���-�� ����
	Mat _headMask;

	vector<ofPoint> _heads;
	vector<int> _headsLabel;

	//����� ���������� ������, x,y - ��������, z - ����������
	ofPoint headSearch( unsigned short label0, int x0, int y0, const Mat &labels, 
		bool isLastHead, ofPoint lastHead);



};

