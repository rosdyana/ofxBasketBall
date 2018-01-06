#pragma once

//Распознавание движения в клетке
//На вход подается diffBinary из pbMotionHistory


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
	void setup( cv::Rect_<float> relRect,		//прямоугольник, в координатах [0, 1]x[0, 1]
					float percent,			//сколько процентов пикселов должно быть включено, чтоб сработало
					float decaySec			//длительность работы после включения
					);		
	void updateCamera( float dt, 
		const Mat &imageBinary,			//8-битное, значения 0 и 255
		bool debugDraw );
	void update( float dt );
	void draw( float x, float y, float w, float h );

	float value() const	{ return _value.value(); }				//Текущее значение, 0..1
	bool isOn() const { return value() > 0; }
	float lastEnableTime() const { return _lastEnableTime; }		//Время последнего включения (после полного выключения)

	cv::Rect_<float> rect() const { return _rect; }

	void reset();			//принудительное выключение ячейки

private:
	float _kPercent;
	float _kDecaySec;

	cv::Rect_<float> _rect;

	float _perc;					//значение с прошлого обновления
	pbValueAnimation _value;
	float _lastEnableTime;			

};
