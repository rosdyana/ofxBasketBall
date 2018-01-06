#pragma once

//Ёмул€тор времени - дл€ рендеринга кадров

#include "ofMain.h"

class pbTime
{
public:
	pbTime() { _manual = false; _time = 0; }
	void setup( bool manual = false );
	void update();
	void setTimeSec( float time );

	float time() { return _time; }
	bool manual() { return _manual; }
private:
	bool _manual;
	float _time;

};

//эту функцию должны вызывать все вместо ofGetElapsedTimef();
float pbGetElapsedTimef();	

extern pbTime time_shared;