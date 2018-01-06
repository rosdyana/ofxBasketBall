#pragma once

//Создание двумерных слоев и распознавание голов

#include "cv.h"
#include "ofMain.h"
#include "PersonType.h"


using namespace cv;



class Scene2D
{
public:
	void setup( float kTruncDepthMM0	//расстояние, на котором обрезать
		
		);

	//изображения типа unsigned short, т.е. CV_16UC1
	//nearPersonId - выходной список близких людей
	void updateCamera( const Mat &depth, const Mat &labels, const Persons &persons, vector<int> &nearPersonIdLabel );
	void update( float dt );

	void draw();

	const Mat &mask() { return _mask; }
	const Mat &mask8() { return _mask8; }

	void getHeadData( vector<ofPoint> &pos, vector<int> &label );
	const vector<ofPoint> &heads() const { return _heads; }
	
private:

	unsigned short kTruncDepthMM;
	Mat _mask;				//16-битная маска, с расстояниями. если > 0, то значит что-то есть
	Mat _mask8;			//8-битная, обычная маска, >0 значит что-то есть
	Mat _headMask;

	vector<ofPoint> _heads;
	vector<int> _headsLabel;

	//найти координаты головы, x,y - экранные, z - расстояние
	ofPoint headSearch( unsigned short label0, int x0, int y0, const Mat &labels, 
		bool isLastHead, ofPoint lastHead);



};

