#include "pbOscSender.h"
#include "ofxIniSettings.h"

pbOscSender shared_oscSender;


//------------------------------------------------
void pbOscSenderParams::load( string fileName )
{
	ofxIniSettings ini;
	ini.load( ofToDataPath( fileName ) );
	string section = "OSC.";
	enabled		= ini.get( section + "enabled", 1 );
	port		= ini.get( section + "port", 12345 );
	server		= ini.get( section + "server", string("127.0.0.1") );
	fps			= ini.get(section + "FPS", 30.0f );
}


//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
pbOscSender::pbOscSender(void)
{
	_inited = false;
	_param.enabled = false;
}

//------------------------------------------------
pbOscSender::~pbOscSender(void)
{
}

//------------------------------------------------
void pbOscSender::setup( string fileName )
{
	pbOscSenderParams param;
	param.load( fileName );
	setup( param );
}

//------------------------------------------------
void pbOscSender::setup( const pbOscSenderParams &param )
{
	if ( !_inited ) {
		_inited = true;
		_param = param;
		if ( enabled() ) {
			cout << "OSC sender:   " << _param.server << " : " << _param.port << "  fps=" << _param.fps << endl;
			_sender.setup( _param.server, _param.port );
			_time = 0;
		}
	}

}

//------------------------------------------------
string pbOscSender::info()
{
	string s = "OSC - ";
	if ( enabled() ) {
		s += "server: " + _param.server + ", port: " + ofToString(_param.port) + ", FPS: " + ofToString( _param.fps );
	}
	else {
		s += "Disabled";
	}	
	return s;
}

//------------------------------------------------
void pbOscSender::update( float dt )
{
	if ( enabled() ) {
		if ( _param.fps > 0 ) {			//в режиме fps == 0 отправка происходит мгновенно
			_time -= dt;
			//если пора отправлять и есть что отправить - отправляем
			bool bundleEmpty = ( _bundle.getMessageCount() == 0 ) && ( _bundle.getBundleCount() == 0 );
			if ( _time <= 0 && !bundleEmpty ) {
				flush();
			}
		}
	}
}

//------------------------------------------------
void pbOscSender::flush()
{
	if ( enabled() ) {
		_sender.sendBundle( _bundle );
		_bundle.clear();
		if ( _param.fps > 0 ) {
			_time = 1.0 / _param.fps;
		}
	}
}

//------------------------------------------------
void pbOscSender::sendMessage( ofxOscMessage &m )
{
	if ( enabled() ) {
		_bundle.addMessage( m );
		if ( _param.fps == 0 ) {
			flush();		
		}
	}
}

//------------------------------------------------
void pbOscSender::sendFloat( const string &param, float value )
{
	ofxOscMessage m;
	m.setAddress( "/" + param );
	m.addFloatArg( value );
	sendMessage( m );
}

//------------------------------------------------
void pbOscSender::sendFloat2( const string &param, float x, float y )
{
	ofxOscMessage m;
	m.setAddress( "/" + param );
	m.addFloatArg( x );
	m.addFloatArg( y );
	sendMessage( m );
}

//------------------------------------------------
void pbOscSender::sendString( const string &param, const string &value )
{
	ofxOscMessage m;
	m.setAddress( "/" + param );
	m.addStringArg( value );
	sendMessage( m );
}

//------------------------------------------------
