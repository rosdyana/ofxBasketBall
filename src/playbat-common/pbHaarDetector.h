#pragma once


#include "ofMain.h"
#include "cv.h"
using namespace cv;
using namespace std;


class pbHaarDetector
{
public:
	void setup( const string &cascadeFileName,		
						    const string &internalId,	
							float minSizeRel	
							);
	

	vector<cv::Rect> find( const Mat &img, bool debugDraw );
private:
	CascadeClassifier _cascade;
	float kMinSizeRel;

	Mat _gray;
	Mat _debugDrawImg;

	string _internalId;

};
