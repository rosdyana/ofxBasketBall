#pragma once

#include "ofMain.h"

class pbKinectMotor
{
public:
	pbKinectMotor(void);
	~pbKinectMotor(void);

	void setup();
	void close();

	void setPosition( float pos );	//pos [0..1]
	void tiltUp();
	void tiltDown();

	void setLed( int led );
private:
	bool _started;
	void *_motor;

	//для плавного перемещения мотора
	float _pos;
	bool _posInited;
	void changePos( float delta );

};
