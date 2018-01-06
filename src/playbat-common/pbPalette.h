#pragma once

//Палитра из картинки, для плавной смены цветов

#include "ofMain.h"


class pbPalette
{
public:
	pbPalette(void);
	~pbPalette(void);
	void setup( string imageFile );
	ofColor color( const ofPoint &p ) const;		//точка [0,1]x[0,1]
	void setColor( const ofPoint &p ) const;

private:
	ofImage _palImage;					//палитра
	unsigned char *_palData;			
	int _palW, _palH, _palBpp;

};
