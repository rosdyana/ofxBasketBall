#pragma once

#include "ofMain.h"
#include "MotionBlob.h"
#include "ofxVectorMath.h"

//Рисование указателей движений пользователя,
//со шлейфом

struct MouseDrawSettings {
	int n;				//максимальное число частиц
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
	virtual void setup( const MouseDrawSettings &settings );	//размеры окна вывода	
	
	virtual void free();						//очистка картинок

	virtual void updateCamera( float dt, MotionBlobs &blobs );	//для данных с камеры
	virtual void update( float dt );
	virtual void draw();

	virtual void getMotionBlobs( MotionBlobs &outBlobs );		//возвращает все живые частицы

private:
	MouseDrawSettings _settings;
	MotionBlobs _blobs;
	vector<MouseParticle> _part;

	ofImage _image;		//картинка с курсором мыши


	int _partCounter;			//круговой счетчик
	void bornParticle( const ofxVec2f &p, const ofxVec2f &v );
	int liveParticlesCount();
};
