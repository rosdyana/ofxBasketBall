#include "pbPalette.h"

//-----------------------------------------------------------
pbPalette::pbPalette(void)
{
	_palData = 0;
}

//-----------------------------------------------------------
pbPalette::~pbPalette(void)
{
}

//-----------------------------------------------------------
void pbPalette::setup( string imageFile )
{
	_palImage.loadImage( imageFile );
	_palData = _palImage.getPixels();
	_palW = _palImage.width;
	_palH = _palImage.height;
	_palBpp = _palImage.bpp / 8;
}

//-----------------------------------------------------------
ofColor pbPalette::color( const ofPoint &p ) const		//точка [0,1]x[0,1]
{
	int x = ofClamp( p.x * _palW, 0, _palW - 1 );
	int y = ofClamp( p.y * _palH, 0, _palH - 1 );
	int index = (x + _palW * y) * _palBpp;
	ofColor c;
	c.r = _palData[ index ];
	c.g = _palData[ index + 1];
	c.b = _palData[ index + 2];
	return c;
}

//-----------------------------------------------------------
void pbPalette::setColor( const ofPoint &p ) const
{
	ofColor col = color( p );
	ofSetColor( col.r, col.g, col.b );
}

//-----------------------------------------------------------
