#pragma once

//Сервер для отправки событий OSC с нужной частотой

#include "ofMain.h"
#include "ofxOsc.h"

class pbSenderOSC
{
public:
	pbSenderOSC(void){}
	~pbSenderOSC(void){}

	//если fps = 0, то отправлять сразу, при первой возможности
	void setup( const string &hostname, int port, int fps );
	void close();

	void update();
	void flush();		//срочная отправка

	//!!! В текущей реализации может быть только один address
	void addString(const string &address, const string &message);
private:
	ofxOscSender _sender;
	int _fps;

	float _time;				//время последней отправки
	ofxOscMessage _m;
};
