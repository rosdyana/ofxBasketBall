#include "pbTransforming.h"
#include "ofxVectorMath.h"

//----------------------------------------------------------------
void pbTransformInfo::load( ofxIniSettings &ini, string path )
{
	duration		= ini.get( path + "Duration", 1.0f );
	rectFrom.x		= ini.get( path + "RectX0", 0.f );
	rectFrom.y		= ini.get( path + "RectY0", 0.f );
	rectFrom.width	= ini.get( path + "RectWidth0", 0.f );
	rectFrom.height	= ini.get( path + "RectHeight0", 0.f );

	rectTo.x		= ini.get( path + "RectX1", 0.f );
	rectTo.y		= ini.get( path + "RectY1", 0.f );
	rectTo.width	= ini.get( path + "RectWidth1", 0.f );
	rectTo.height	= ini.get( path + "RectHeight1", 0.f );

	backgroundSize  = ini.get( path + "BackgroundSize", 1.f );

}

//----------------------------------------------------------------
pbTransforming::pbTransforming()
{
	_status = 0;
	_transTime = 0;
}


//----------------------------------------------------------------
void pbTransforming::start( const pbTransformInfo &tr )
{
	if ( status() == StatusNo ) {
		_status = StatusProcess;
		_transTime = ofGetElapsedTimef();
		_transInfo = tr;
	}
}

//----------------------------------------------------------------
void pbTransforming::update()
{
	if ( status() == StatusProcess && ofGetElapsedTimef() - _transTime >= _transInfo.duration ) {
		_status = StatusFinished;
	}

}

//----------------------------------------------------------------
int pbTransforming::status()				
{
	return _status;
}

//----------------------------------------------------------------
float pbTransforming::alpha()
{
	if ( status() == StatusNo ) return 0.0;
	float alpha = (ofGetElapsedTimef() - _transTime) / _transInfo.duration;
	alpha = max( min( alpha, 1.0f ), 0.0f );
	return alpha;	
}

//----------------------------------------------------------------
void pbTransforming::pushMatrix()
{
	ofPushMatrix();

	float a = alpha();

	//cout << "alpha " << a << endl;
	//a = 1.0; 

	ofRectangle r0 = _transInfo.rectFrom;
	ofRectangle r1 = _transInfo.rectTo;

	if ( r0.width > 0 ) {
		ofxVec2f c0( r0.x + r0.width/2, r0.y + r0.height/2 );
		ofxVec2f c1( r1.x + r1.width/2, r1.y + r1.height/2 );
		float w = (1-a) * r0.width + a * r1.width;
		float scale = w / r0.width;

		ofxVec2f c = (1-a) * c0 + a * c1;
		
		ofTranslate( c.x, c.y );
		ofScale( scale, scale );
		ofTranslate( -c0.x, -c0.y );

	
	}


}

//----------------------------------------------------------------
void pbTransforming::popMatrix()
{
	ofPopMatrix();
}

//----------------------------------------------------------------
int pbTransforming::userData()
{
	return _transInfo.userData;
}

//----------------------------------------------------------------