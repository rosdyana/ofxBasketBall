#pragma once

//���������� ����������� ������.
//��������� ���������� �������� �������� ���� ����


#include "ofMain.h"
#include "cv.h"
using namespace cv;

class pbOpticalIntegrator
{
public:
	void setup( int historyLength );
	void update( const Mat &image, int frame, bool debugDraw = false );

	//���������� �������� �������������� �� frame0 �� frame1.
	//���������� ����� ����������� �� ����� ��������������
	//�������, ��������, � ������� ����� ���������� ���� ����������� �������������
	//w, h - ������� ������, � ������� ����� rect.
	//���� � ���, ��� ��� �������� ���������� ����� ������ ����������� �� ������� �����������
	ofRectangle integrateRect( const ofRectangle &rect, float w, float h, int frame0, int frame1 );

private:
	//������� ����������� ������, ��� �������������� ��������� ����
	int _n;						//����� �������
	int _optPos;				//��������� � �������
	vector<int>_optFrame;
	vector<Mat>_optFlow;


	Mat _prevGray, _currentGray;	//��������, ���������� �� �������� ��������
	Mat _flow;						//���������� ����� �� ������ ����� (� ���������� ������)
	Mat _flowBoxed;					//���������� ���������� �����


};
