#pragma once

#include <cv.h>

#include "ofMain.h"
#include "pbForegroundImpl.h"
#include "ofxOpenCv.h"

using namespace std;
using namespace cv;


//---------------------------------------
class pbForeground
{
public:
	void setup( int w, int h );
	bool load( const string &fileName );
	void save( const string &fileName );
	void autoAdjust();
	void autoAdjustDisable();
	
	void startLearn( float learnDelay, float learnDuration, const string &saveToFileName );			
	void update( IplImage *inputRGBImage, IplImage *resultGrayImage );	

	bool learning();			
	float learningProgress( bool truncate = true );

	IplImage *mask() { return _mask; }
	void draw( float x, float y, float w = 0.0, float h = 0.0 );
private:
	bool _learning;
	float _learningStart;
	float _learningDuration;
	int _w, _h; 
	int _ch;

	IplImage* _yuvImage;
	vector<codeBook> _codeBook;

	void drawCodeBook( IplImage *resultGrayImage );
	
	IplImage *_drawGrayImage;
	IplImage *_mask;

	ofxCvGrayscaleImage _drawImage;
	bool _drawImageReady;

	string _saveFileName;

	void clear();
};
