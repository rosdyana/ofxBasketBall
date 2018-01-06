#pragma once

//������������� �������� � ������
//�� ���� �������� diffBinary �� pbMotionHistory


#include "ofMain.h"
#include <cv.h>
#include "pbValueAnimation.h"

using namespace std;
using namespace cv;


class pbMotionCell
{
public:
	pbMotionCell(void);
	~pbMotionCell(void);
	void setup( cv::Rect_<float> relRect,		//�������������, � ����������� [0, 1]x[0, 1]
					float percent,			//������� ��������� �������� ������ ���� ��������, ���� ���������
					float decaySec			//������������ ������ ����� ���������
					);		
	void updateCamera( float dt, 
		const Mat &imageBinary,			//8-������, �������� 0 � 255
		bool debugDraw );
	void update( float dt );
	void draw( float x, float y, float w, float h );

	float value() const	{ return _value.value(); }				//������� ��������, 0..1
	bool isOn() const { return value() > 0; }
	float lastEnableTime() const { return _lastEnableTime; }		//����� ���������� ��������� (����� ������� ����������)

	cv::Rect_<float> rect() const { return _rect; }

	void reset();			//�������������� ���������� ������

private:
	float _kPercent;
	float _kDecaySec;

	cv::Rect_<float> _rect;

	float _perc;					//�������� � �������� ����������
	pbValueAnimation _value;
	float _lastEnableTime;			

};
