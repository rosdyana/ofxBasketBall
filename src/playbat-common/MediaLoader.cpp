#include "MediaLoader.h"
#include "ofxDirList.h"

MediaLoader lazyLoader;

//-----------------------------------------------------------------
//считать содержимое папки
vector<string> readDir( string path, bool onlyImages )
{
	ofxDirList dir;
	if (onlyImages) {
		dir.allowExt( "png" );
		dir.allowExt( "jpg" );
		dir.allowExt( "bmp" );
	}
	int n = dir.listDir( path );
	vector<string> list(n);
	for (int i=0; i<n; i++) {
		list[i] = dir.getPath(i);
	}
	return list;
}

//-------------------------------------------------------------------
MediaLoader::MediaLoader(void)
{

}

//-------------------------------------------------------------------
MediaLoader::~MediaLoader(void)
{

}

//-------------------------------------------------------------------
void MediaLoader::setup()
{

}

//-------------------------------------------------------------------
void MediaLoader::update()
{
	if ( !_item.empty() ) {
		MediaLoaderItem item = _item.front();
		_item.pop_front();
		item.execute();
	}
}

//-------------------------------------------------------------------
void MediaLoader::loadAll()
{
	while (loading()) {
		update();
	}
}

//-------------------------------------------------------------------
void MediaLoader::draw( float w, float h )
{
	if ( loading() ) {
		const float kPeriod = 2.0;
		const int kCircles = 8;
		const float kRad = 0.1;
		const float kCircleRad = 0.02;
		float t = ofGetElapsedTimef();
		t = t / kPeriod;
		t = t - int(t);		//теперь t от 0 до 1
		int shift = t * kCircles;
		float rad = kCircleRad * h;
		float Rad = kRad * h;
		
		ofSetColor( 0, 0, 0 );
		ofFill();
		ofRect( 0, 0, w, h );
		for (int i=0; i<kCircles; i++) {
			float angle = M_TWO_PI * (i + shift) / kCircles;
			float x = w/2 + Rad * cos( angle ); 
			float y = h/2 + Rad * sin( angle ); 
			float alpha = 255.0 * ( kCircles - i ) / kCircles;
			ofSetColor( alpha, alpha, alpha );
			ofCircle( x, y, rad );
		}
	}
}

//-------------------------------------------------------------------
void MediaLoader::pushImage( ofImage *image, const string &fileName )
{
	MediaLoaderItem item;
	item.fileName = fileName;
	item.pImage = image;
	_item.push_back( item );
}

//-------------------------------------------------------------------
void MediaLoader::pushSound( ofSoundPlayer *sample, const string &fileName )
{
	MediaLoaderItem item;
	item.fileName = fileName;
	item.pSample = sample;
	_item.push_back( item );
}

//-------------------------------------------------------------------
void lazyLoadImage( ofImage &image, const string &fileName )
{
	lazyLoader.pushImage( &image, fileName );
}

//-------------------------------------------------------------------
void lazyLoadSound( ofSoundPlayer &sample, const string &fileName )
{
	lazyLoader.pushSound( &sample, fileName );

}

//-------------------------------------------------------------------
