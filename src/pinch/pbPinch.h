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

	Mat _image32;		//32-битная картинка с камеры
	//разность
	Mat _imgLast;
	Mat _imgDiffRGB;
	Mat _imgDiff;

	//история

	//Mat _imgHistory;
	//Mat _imgHistoryBin;

	//поток
	Mat _imgFlowColor, _imgFlow, _imgFlowLast;
	Farneback _motion;
	Mat _blurredX, _blurredY, _blurredAbs;

	//функции
	void updateDiff( float dt, const Mat &image, bool debugDraw );		//разность соседних кадров
	//void updateHistory( float dt, const Mat &image32, bool debugDraw );	//история кадров для вычисл фона

	void updateFlow( float dt, const Mat &image, //const Mat &flow, 
								bool debugDraw );
	void updateCombined( float dt, bool debugDraw ); 

	//эйлеровы траектории
	pbPinchEuler _euler;


};
