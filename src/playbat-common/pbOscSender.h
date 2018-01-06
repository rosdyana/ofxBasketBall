#pragma once

//Посылатель OSC-сигналов, с заданным FPS

#include "ofMain.h"
#include "ofxOsc.h"

struct pbOscSenderParams
{
	bool enabled;
	int port;
	string server;
	float fps;
	void load( string fileName );
};

class pbOscSender
{
public:
	pbOscSender();
	~pbOscSender();
	void setup( string fileName );
	void setup( const pbOscSenderParams &param );
	void update( float dt );

	void sendFloat( const string &param, float value );
	void sendFloat2( const string &param, float x, float y );
	void sendString( const string &param, const string &value );
	void sendMessage( ofxOscMessage &m );
	void flush();	//выслать всё, что есть в буфере отправки

	bool enabled()	{ return _param.enabled; }
	string info();	//строка описания
private:
	bool _inited;

	pbOscSenderParams _param;

	ofxOscSender _sender;
	ofxOscBundle _bundle;

	float _time;		//время до следующей посылки
};

extern pbOscSender shared_oscSender;
