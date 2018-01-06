#pragma once

//������ ��� �������� ������� OSC � ������ ��������

#include "ofMain.h"
#include "ofxOsc.h"

class pbSenderOSC
{
public:
	pbSenderOSC(void){}
	~pbSenderOSC(void){}

	//���� fps = 0, �� ���������� �����, ��� ������ �����������
	void setup( const string &hostname, int port, int fps );
	void close();

	void update();
	void flush();		//������� ��������

	//!!! � ������� ���������� ����� ���� ������ ���� address
	void addString(const string &address, const string &message);
private:
	ofxOscSender _sender;
	int _fps;

	float _time;				//����� ��������� ��������
	ofxOscMessage _m;
};
