#pragma once

#include "ofMain.h"
#include <cv.h>
#include "cvaux.h"

using namespace std;
using namespace cv;

class pbBackgrDetector
{
public:
	pbBackgrDetector(void);
	~pbBackgrDetector(void);

	void setup();
	void close();
	void reinit();					//������������� �������� ������������� ���� (��������� �� ������� ������ �����
	void updateCamera( float dt, const Mat &image, bool debugDraw );

	void update( float dt );

	const Mat &foreground();	//��������� ������� ��������� �����
	const Mat &background();	//������� ���
	
private:
	void free();

	Mat _image;	

	CvBGStatModel* bg_model;

	//�������� ��� �������� �������������
	Mat _background;
	Mat _foreground;

};
