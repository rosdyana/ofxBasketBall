#pragma once

#include "ofMain.h"
#include "MotionBlob.h"
#include "ofxVectorMath.h"

//��������� ���������� �������� ������������,
//�� �������

struct MouseDrawSettings {
	int n;				//������������ ����� ������
	int colorR;
	int colorG;
	int colorB;
	float duration;
	float appearDuration;
	float velocityKoef;
	float sizeKoef;
	bool useImage;
	string imageFileName;
	

	MouseDrawSettings();

};

struct MouseParticle {
	ofxVec2f p, v;
	float bornTime;
	float length;
	bool enabled;
	MouseParticle() {
		enabled = false;
		bornTime = -100.0;
	}
};


class MouseDraw
{
public:
	virtual void setup( const MouseDrawSettings &settings );	//������� ���� ������	
	
	virtual void free();						//������� ��������

	virtual void updateCamera( float dt, MotionBlobs &blobs );	//��� ������ � ������
	virtual void update( float dt );
	virtual void draw();

	virtual void getMotionBlobs( MotionBlobs &outBlobs );		//���������� ��� ����� �������

private:
	MouseDrawSettings _settings;
	MotionBlobs _blobs;
	vector<MouseParticle> _part;

	ofImage _image;		//�������� � �������� ����


	int _partCounter;			//�������� �������
	void bornParticle( const ofxVec2f &p, const ofxVec2f &v );
	int liveParticlesCount();
};
