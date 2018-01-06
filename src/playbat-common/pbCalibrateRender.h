#pragma once

//Калибровка вывода на экран

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

	void grabBegin();	//включение рисования в буфер
	void grabEnd();		//выключение рисования в буфер
	void draw( float w, float h, float alpha );	//вывод картинки на экран, с учетом искажений, учитывает enable
								//alpha - для целей отладки

	//4 точки, обход точек по часовой стрелке
	void setPoints( const vector<ofxVec2f> &p );
	const vector<ofxVec2f> &points() { return _points; }

	bool inited() { return _inited; }

private:	
	int _w, _h; 

	vector<ofxVec2f> _points;

	bool _inited;
	Mat _transform;		//матрица преобразования


	ofxFBOTexture _buffer;
	
};
