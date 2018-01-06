#pragma once

//������ �������

#include "ofMain.h"

class pbRecorderString
{
public:
	pbRecorderString(void);
	~pbRecorderString(void);

	void save( const string &fileName );
	void load( const string &fileName );

	void reset();
	string readNext( bool useFPS = true );	//������������ �� ������� �������

	void clear();
	void push( const string &data );

	bool empty() { return _data.size() == 0; }

	float fps() { return _fps; };
private:
	vector<string> _data;
	int _read;					//��������� ����������� �������

	float _fps;

	float _readTime;

};
