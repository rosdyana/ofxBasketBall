#pragma once

//����� ����� �� �����

#include "ofMain.h"

class pbScreenLog
{
public:
	pbScreenLog(void) {};
	~pbScreenLog(void) {};

	//secToAddEmptyLines - ����� ������� ������ ��������� ������ ������ ���� �� ���� ���������� ���������
	//���� 0, �� �� ���������
	void setup( const string &fontFileName, int fontSize, float secToAddEmptyLines = 0.0, int capacity = 100 );
	void update();		//���� �������� ���� �������� ������ ������
	void clear();
	void add( const string &message, int color );

	void draw( ofRectangle rect, int bR = 0, int bG = 0, int bB = 0, int bA = 0 );

private:
	ofTrueTypeFont _font;

	vector<string> _messageText;
	vector<int> _messageColor;
	float _secToAddEmptyLines;		//����� ������� ������ ��������� ������ ������ ���� �� ���� ���������� ���������
	int _capacity;

	float _lastAddTime;				//����� ����� ���� ��������� ��������� ���������
	
};
