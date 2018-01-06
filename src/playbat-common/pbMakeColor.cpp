#include "pbMakeColor.h"

//-------------------------------------------------------------------
ofColor makeColor( int r, int g, int b, int a )
{
	ofColor color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

//-------------------------------------------------------------------
void ofSetOfColor( const ofColor &color )
{
	ofSetColor( color.r, color.g, color.b, color.a );
}

//-------------------------------------------------------------------
//Загрузить палитру
vector<ofColor> loadPalette( const string &imageFileName )
{
	vector<ofColor> list;
	ofImage image;
	image.loadImage( imageFileName );
	int w = image.width;
	int h = image.height;
	if ( w > 0 && h > 0 && ( image.bpp == 24 || image.bpp == 32 ) ) {
		list.resize( w * h );
		unsigned char *data = image.getPixels();
		for (int i=0; i<w*h; i++) {
			int index = image.bpp / 8 * i;
			int a = ( image.bpp == 32 ) ? data[ index+3] : 255;
			ofColor color = makeColor( data[ index ], data[ index+1 ], data[ index+2 ], a );
			list[i] = color;
		}
	}
	else {
		cout << "ERROR loadPalette, bad image " << imageFileName << endl;
	}

	return list;
}

//-------------------------------------------------------------------
