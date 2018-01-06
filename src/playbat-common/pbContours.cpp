#include "pbContours.h"
#include "pbFiles.h"

//--------------------------------------------------------------
void drawContour( const vector<ofPoint> &c, int color, int lineWidth, bool closed ) 
{
	ofSetColor( color );
	ofSetLineWidth( lineWidth );
	int n = c.size();
	int n1 = ( closed  ) ? n : (n-1);
	for (int i=0; i<n1; i++) {
		ofLine( c[i].x, c[i].y, c[(i+1)%n].x, c[(i+1)%n].y );
	}
	ofSetLineWidth(1);
}


//--------------------------------------------------------------
void writeContour( const string &fileName, const vector<ofPoint> &contour )
{
	vector<float> v;
	for (int i=0; i<contour.size(); i++) {
		v.push_back( contour[i].x );
		v.push_back( contour[i].y );
	}
	pbFiles::writeFloats( v, fileName );
}

//--------------------------------------------------------------
vector<ofPoint> readContour( const string &fileName )
{
	vector<float> v = pbFiles::readFloats( fileName );
	vector<ofPoint> contour;
	contour.clear();
	for (int i=0; i<v.size(); i+=2) {
		contour.push_back( ofPoint( v[i], v[i+1] ) );
	}
	return contour;
}

//--------------------------------------------------------------
//»щет точку контура, р€дом с p
int findContourIndex( ofPoint p, const vector<ofPoint> &contour, float rad )
{
	int index = -1;
	float dist = -1;
	for ( int i=0; i<contour.size(); i++) {
		float d = ofDist( p.x, p.y, contour[i].x, contour[i].y );
		if ( (index == -1 || d < dist) && d < rad ) {
			dist = d;
			index = i;
		}
	}
	return index;
}

//--------------------------------------------------------------
//√абаритный бокс
ofRectangle contourBoundBox( const vector<ofPoint> &c )
{
	float x0 = 10000;
	float y0 = 10000;
	float x1 = -10000;
	float y1 = -10000;
	for (int i=0; i<c.size(); i++) {
		ofPoint p = c[i];
		x0 = min( x0, p.x );
		x1 = max( x1, p.x );
		y0 = min( y0, p.y );
		y1 = max( y1, p.y );
	}
	return ofRectangle( x0, y0, x1 - x0, y1 - y0 );

}

//--------------------------------------------------------------
