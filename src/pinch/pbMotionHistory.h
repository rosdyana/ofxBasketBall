#pragma once

//Вычисление истории для детектора движений


#include "ofMain.h"
#include <cv.h>
#include "ofxVectorMath.h"

using namespace std;
using namespace cv;

class pbMotionHistory
{
public:
	pbMotionHistory(void);
	~pbMotionHistory(void);

	void setup( float timeFilter = 0.1f, 			//Параметр фильтра низких чатот, чем меньше - тем длиннее история
				float thresh = 30.0f / 255.0f		//Порог бинаризации разности истории и текущего кадра, от 0 до 1
		);	
	void updateCamera( float dt, const Mat &image, bool debugDraw );
	void update( float dt );
	const Mat &image32bit() { return _imageHist32; }
	const Mat &image8bit();
	const Mat &diffBinary() { return _diffBin8; }	//разница между историей и текущим кадром, бинарная

private:
	Mat _image32;		//входное изобр, 32-битное
	Mat _imageHist32;	//выходное изобр
	Mat _imageHist8;	//выходное изобр
	
	float _kFilter;
	float _kThresh;

	Mat _diff;
	vector<Mat> _diffPlanes;
	Mat _diffGray;
	Mat _diffBin32, _diffBin8;


};
