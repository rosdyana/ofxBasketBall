#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"


struct FCContour {
	int size;
	ofPoint *pnt;
};



struct FindContoursData {
	int minLength;		
	int maxLength;		
	int reduce;			
	bool debugDraw;	
	FindContoursData() {
		minLength = 1;
		maxLength = 10000000;
		reduce = 1;
		debugDraw = false;
	}
};


class FindContours
{
public:
	void setup( int storageSize );	
		
	void reset();			
	void update( IplImage *grayImage, const FindContoursData &data);			
	int size();				
	FCContour item( int i );
private:
	vector<ofPoint> _pnt;		
	vector<int> _beginIndex;	
	int _size;					
	int _pntCount;				
	int _storageSize;


	//opencv
	ofxCvGrayscaleImage *_tempImage;			
	CvMemStorage	*_cv_storage;				

	void convert( CvSeq* contours, const FindContoursData &data );

};




