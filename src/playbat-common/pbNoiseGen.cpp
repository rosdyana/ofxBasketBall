#include "pbNoiseGen.h"
#include "converter.h"

//---------------------------------------------------------------
pbNoiseGen::pbNoiseGen(void)
{
	_enabled = false;
	_amt = 0;
	_velocity = 0;
	_pos = 0;
}

//---------------------------------------------------------------
pbNoiseGen::~pbNoiseGen(void)
{
}

//---------------------------------------------------------------
void pbNoiseGen::setup( float amt, float velocity )	//количество и скорость
{
	setup( true, amt, velocity );
}

//---------------------------------------------------------------
void pbNoiseGen::setup( bool enabled, float amt, float velocity )	//количество и скорость
{
	_enabled = enabled && ( amt != 0 && velocity != 0 );
	_amt		= amt;
	_velocity	= velocity;
	restart();
}

//---------------------------------------------------------------
void pbNoiseGen::restart()
{
	_pos = ofRandom( 0, 1000 );
}


//---------------------------------------------------------------
void pbNoiseGen::setup( const string &s0 )				//строка вида amt;velocity
{
	string s = s0;
	s = Converter::replaceAll( s, " ", "" );
	s = Converter::replaceAll( s, "\t", "" );
	vector<string> list = ofSplitString( s, ";" );
	if ( list.size() >= 2 ) {
		float amt = ofToFloat( list[0] );
		float velocity = ofToFloat( list[1] );
		setup( true, amt, velocity );
	}
	else {
		setup( false, 0, 0 );
	}
}

//---------------------------------------------------------------
void pbNoiseGen::update( float dt )
{
	if ( _enabled ) {
		_pos += _velocity * dt;
	}
}

//---------------------------------------------------------------
float pbNoiseGen::value()
{
	return ( _enabled ) ? ( _amt * ofSignedNoise( _pos ) ) : 0;
}

//---------------------------------------------------------------
void pbNoiseGen2D::setup( float amtX, float velocityX, float amtY, float velocityY )	//количество и скорость
{
	if ( amtY == -1 ) amtY = amtX;
	if ( velocityY == -1 ) velocityY = velocityX;

	_noiseX.setup( amtX, velocityY );
	_noiseY.setup( amtY, velocityY );
}

//---------------------------------------------------------------
void pbNoiseGen2D::setupString( const string &sX, const string &sY )				//строка вида amt;velocity
{
	string sy_ = ( sY == "" ) ? sX : sY;
	_noiseX.setup( sX );
	_noiseY.setup( sy_ );
}

//---------------------------------------------------------------
void pbNoiseGen2D::update( float dt )
{
	_noiseX.update( dt );
	_noiseY.update( dt );
}

//---------------------------------------------------------------
ofPoint pbNoiseGen2D::pos()
{
	return ofPoint( _noiseX.value(), _noiseY.value() );
}

//---------------------------------------------------------------
void pbNoiseGen2D::restart()
{
	_noiseX.restart();
	_noiseY.restart();
}

//---------------------------------------------------------------
