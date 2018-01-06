#include "pbTime.h"

pbTime time_shared;

//-------------------------------------------------------------
void pbTime::setup( bool manual )
{
	_manual = manual;
	_time = 0;
}

//-------------------------------------------------------------
void pbTime::update()
{
	if ( !_manual ) { 
		_time = ofGetElapsedTimef();
	}
}

//-------------------------------------------------------------
void pbTime::setTimeSec( float timeSec )
{
	if ( _manual ) {
		_time = ofGetElapsedTimef();
	}
}

//-------------------------------------------------------------
float pbGetElapsedTimef()
{
	return time_shared.time();
}

//-------------------------------------------------------------

