#pragma once

//Слои

#include "ofMain.h"
#include "ofxIniSettings.h"
#include "pbLayer.h"

class pbLayers
{
public:
	pbLayers(void);
	~pbLayers(void);
	void setup( ofxIniSettings &ini );
	void setup( const string &fileName );
	void update( float dt );
	void draw( float w, float h, int range ); //range = -1 - рисует слои с z < 0,  range = 1 - рисует слои с z >= 0
	void draw( float w, float h, float z0, float z1 );	//рисует слои из диапазона z0 <= z < z1 
	void setShift( float shiftX, float shiftY = 0 );


	pbLayer &layer( const string &name );
private:
	vector<pbLayer> _layer;
	int findByName( const string &name );

};

extern pbLayers shared_layers;