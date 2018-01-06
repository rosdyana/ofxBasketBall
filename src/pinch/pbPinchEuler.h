#pragma once

//�������� ���������� ��� �����.

#include "ofMain.h"
#include <cv.h>
#include "ofxVectorMath.h"


using namespace std;
using namespace cv;


const int kpbPinchLinePointBufferSize = 10;	//������ ������ �������

struct pbPinchLinePointBufferData {
	ofxVec2f p;
	float time;
	void reset() {
		time = -1;
	}
	pbPinchLinePointBufferData() {
		reset();
	}
	pbPinchLinePointBufferData( ofxVec2f p0, float time0 ) {
		p = p0;
		time = time0;
	}
};

struct pbPinchLinePoint {
	void setup( int xa, int ya, float historySec ) {
		p0 = ofxVec2f( xa, ya );
		bufferSize = kpbPinchLinePointBufferSize;
		_historySec = historySec;
		reset();
	}
	void reset() {
		p = p0;
		for ( int i=0; i<bufferSize; i++) {
			buffer[i].reset();
		}
		bufferPos = 0;
		activated = false;
	}

	ofxVec2f p0;
	ofxVec2f p;
	bool activated;

	void pushBuffer();	//�������� ������� ���������, ���� ���������
	ofxVec2f lastBufferPoint();	//����� ������ ����� � ������
	bool checkPoint( float thresh );			//��������, ����������� �� �����

	float _historySec;
	int bufferSize;
	pbPinchLinePointBufferData buffer[ kpbPinchLinePointBufferSize ];
	int bufferPos;
};


class pbPinchEuler {
public:
	void setup( int flowW, int flowH, float activateLength );
	//activateLength - ����� ����� ��� ��� ������������. ���� <= 0 - �� ������������

	//��� �� ����� ����� ��� ����������
	void updateCamera( float dt, const Mat &flowX, const Mat &flowY, const Mat &flowAbs, bool debugDraw = true );
	void update( float dt );
	void draw( float x, float y, float w, float h );

	const vector<pbPinchLinePoint> &points() { return _p; }

	vector<int> activated();		//������ ����������� ������; ��� ��������� ������ updateCamera ��� ��������

private:
	int _w, _h;

	vector<pbPinchLinePoint> _p;	

	float _activateLength;			//����� ����� ��� ������������

};


