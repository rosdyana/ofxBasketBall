#pragma once

//��������� ����������� ����

#include "ofMain.h"

class pbNoiseGen
{
public:
	pbNoiseGen(void);
	~pbNoiseGen(void);
	void setup( float amt, float velocity );	//���������� � ��������
	void setup( bool enabled, float amt, float velocity );	//���������� � ��������
	void setup( const string &s );				//������ ���� amt;velocity
	void update( float dt );
	float value();
	float valueUniform() { return value() * 0.5 + 0.5; };		//���� amt=1, �� �������� � [0,1]

	void setAmt( float value ) { _amt = value; }
	void restart();
	
private:
	bool _enabled;
	float _amt, _velocity;
	double _pos;

};

//���-�����
class pbNoiseGen2D {
public:
	void setup( float amtX, float velocityX, float amtY = -1, float velocityY = -1 );	//���������� � ��������
	void setupString( const string &sX, const string &sY = "" );				//������ ���� amt;velocity
	void update( float dt );
	ofPoint pos();
	ofPoint posUniform() { return ofPoint( _noiseX.valueUniform(), _noiseY.valueUniform() ); }  

	void setAmt( ofPoint &amt ) { _noiseX.setAmt( amt.x ); _noiseY.setAmt( amt.y ); }

	void restart();

private:
	pbNoiseGen _noiseX, _noiseY;

};
