#pragma once

//���������� ������ �� �����

#include "ofMain.h"
#include "cv.h"
#include "pbScreenFormat.h"
#include "ofxVectorMath.h"
#include "ofxFBOTexture.h"

using namespace cv;
using namespace std;

class pbCalibrateRender
{
public:
	void setup( int w, int h );
	void update();

	void grabBegin();	//��������� ��������� � �����
	void grabEnd();		//���������� ��������� � �����
	void draw( float w, float h, float alpha );	//����� �������� �� �����, � ������ ���������, ��������� enable
								//alpha - ��� ����� �������

	//4 �����, ����� ����� �� ������� �������
	void setPoints( const vector<ofxVec2f> &p );
	const vector<ofxVec2f> &points() { return _points; }

	bool inited() { return _inited; }

private:	
	int _w, _h; 

	vector<ofxVec2f> _points;

	bool _inited;
	Mat _transform;		//������� ��������������


	ofxFBOTexture _buffer;
	
};
