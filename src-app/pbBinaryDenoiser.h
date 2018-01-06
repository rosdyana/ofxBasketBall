#pragma once

#include <cv.h>
#include "ofMain.h"
#include "ofxOpenCv.h"
using namespace std;
using namespace cv;



class pbBinaryDenoiser
{
public:
	void setup( int w, int h );
	
	void denoiseUsingMorphology( IplImage *mask, int openClose, int closeOpen );

	void denoiseUsingContours( IplImage *mask, int openClose, int closeOpen,				
				float minPerimeter = 0.0, bool approx = true, double approxLevel = 0.0 );

	void denoiseUsingFloodFill( IplImage *mask, int openClose, int closeOpen,
		int blackAreaThreshold, int whiteAreaThreshold );



	IplImage *mask() { return _mask; }
	void draw( float x, float y, float w = 0.0, float h = 0.0 );

private:
	int _w, _h;
	IplImage *_mask;

	CvMemStorage* mem_storage;
	CvSeq* contours;

	ofxCvGrayscaleImage _drawImage;
	bool _drawImageReady;

	void morphology( int openClose, int closeOpen );
};

