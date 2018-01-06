#pragma once


#include "ofMain.h"
#include <cv.h>
#include "ofxVectorMath.h"
#include "pbPinchEuler.h"
#include "Farneback.h"


using namespace std;
using namespace cv;




class pbPinch
{
public:
	void setup( const string &settingsFileName, int flowW, int flowH );
	void updateCamera( float dt, //const Mat &flow, 
								const Mat &image, bool debugDraw = true );
	void update( float dt );
	void draw( float x, float y, float w, float h );
private:
	int _w, _h;

	Mat _image32;		//32-������ �������� � ������
	//��������
	Mat _imgLast;
	Mat _imgDiffRGB;
	Mat _imgDiff;

	//�������

	//Mat _imgHistory;
	//Mat _imgHistoryBin;

	//�����
	Mat _imgFlowColor, _imgFlow, _imgFlowLast;
	Farneback _motion;
	Mat _blurredX, _blurredY, _blurredAbs;

	//�������
	void updateDiff( float dt, const Mat &image, bool debugDraw );		//�������� �������� ������
	//void updateHistory( float dt, const Mat &image32, bool debugDraw );	//������� ������ ��� ������ ����

	void updateFlow( float dt, const Mat &image, //const Mat &flow, 
								bool debugDraw );
	void updateCombined( float dt, bool debugDraw ); 

	//�������� ����������
	pbPinchEuler _euler;


};
