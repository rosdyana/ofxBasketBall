#pragma once

//Фильтр бинарных масок - стабилизатор, для игр типа кувшинок

#include "ofMain.h"
#include "cv.h"
#include "pbCameraCalibrate.h"
using namespace cv;


class pbMaskStabilizer
{
public:
	pbMaskStabilizer(void);
	~pbMaskStabilizer(void);
	void setup( float historySec );	//сколько должна стоять точка, чтоб ее считать сработавшей

	void update( float dt, const Mat &mask );
	Mat &mask();

	bool inited() { return _inited; }

private:
	float kHistorySec;
	Mat _history;
	cv::Size _size;

	Mat _tempMaskFloat;
	Mat _maskResult;

	bool _inited;
	void init( const Mat &mask );

};
