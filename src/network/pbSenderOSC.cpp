#include "pbSenderOSC.h"

//---------------------------------------------------------
void pbSenderOSC::setup( const string &hostname, int port, int fps )
{
	_sender.setup( hostname, port );
	_time = 0;
	_fps = fps;

	cout << "Starting OSC sender: hostname=" << hostname << " port=" << ofToString( port ) << " fps=" << fps << endl;
}

//---------------------------------------------------------
void pbSenderOSC::close( )
{
	//_sender.close();
}

//---------------------------------------------------------
void pbSenderOSC::update()
{
	if ( _fps > 0 && _m.getNumArgs() > 0 ) {	//есть что посылать
		float time = ofGetElapsedTimef();
		if ( time >= _time + 1.0 / _fps ) {
			flush();
		}
	}
}

//---------------------------------------------------------
//срочная отправка
void pbSenderOSC::flush()
{
	_time = ofGetElapsedTimef();
	_sender.sendMessage( _m );
	_m.clear();
}

//---------------------------------------------------------
void pbSenderOSC::addString(const string &address, const string &message)
{
	_m.setAddress( address );
	_m.addStringArg( message );
	if ( _fps == 0 ) {
		_sender.sendMessage( _m );
		_m.clear();
	}
}

//---------------------------------------------------------


//---------------------------------------------------------


//---------------------------------------------------------
