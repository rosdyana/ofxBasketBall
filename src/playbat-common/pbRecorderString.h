#pragma once

//«апись скелета

#include "ofMain.h"

class pbRecorderString
{
public:
	pbRecorderString(void);
	~pbRecorderString(void);

	void save( const string &fileName );
	void load( const string &fileName );

	void reset();
	string readNext( bool useFPS = true );	//использовать ли счетчик времени

	void clear();
	void push( const string &data );

	bool empty() { return _data.size() == 0; }

	float fps() { return _fps; };
private:
	vector<string> _data;
	int _read;					//положение считывающей головки

	float _fps;

	float _readTime;

};
