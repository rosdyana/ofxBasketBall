#include "pbDepth.h"
#include "highgui.h"

//------------------------------------------------------------------------
void pbDepth::depthToFilePNG( const Mat &depth16, const string &fileName, float minDist, float maxDist )
{
	//отображение в [0,255], value -> a * value + b;
	float a = 255.0 / ( maxDist - minDist );
	float b = -a * minDist;

	Mat depth; //grabber.grabDepthFrameCVFloat();
	depth16.convertTo( depth, CV_8UC1, a, b );
	imshow( "depth", depth );

	imwrite( fileName, depth );

}

//------------------------------------------------------------------------
void pbDepth::depthToFileArray( const Mat &depth16, const string &fileName, float minDist, float maxDist )
{
	FILE *f = fopen( fileName.c_str(), "wb" );
	if ( f ) {
		int w = depth16.size().width;
		int h = depth16.size().height;
		//неоптимально, но для будущих конвертаций удобнее
		unsigned short *data = new unsigned short[ w * h ];
		for (int y=0; y<h; y++) {
			for (int x=0; x<w; x++) {
				data[ x + w * y ] = depth16.at<unsigned short>(y, x);
			}
		}
		fwrite( data, sizeof( data[0] ), w * h, f );
		delete[] data;
		fclose(f);
	}
}

//------------------------------------------------------------------------
//запись в папку по очереди номеров кадров
int _pbDepthFrame = 0;
void pbDepth::saveDepthFrame( const Mat &depth16, const string &dirPNG, const string &dirArray, 
							 float minDist, float maxDist )
{
	string name = ofToString( _pbDepthFrame );
	while( name.length() < 5 ) name = "0" + name;
	if ( dirPNG != "" ) {
		depthToFilePNG( depth16, dirPNG + "/" + name + ".png", minDist, maxDist );
	}
	if ( dirArray != "" ) {
		depthToFileArray( depth16, dirArray + "/" + name + ".dpt", minDist, maxDist );
	}


	_pbDepthFrame++;

}

//------------------------------------------------------------------------
//запись в папку маски
int _pbMaskFrame = 0;
void pbDepth::saveMaskFrame( const Mat &mask8, const string &dirPNG )
{
	string name = ofToString( _pbMaskFrame );
	while( name.length() < 5 ) name = "0" + name;
	
	string fileName = dirPNG + "/" + name + ".png";
	imwrite( fileName, mask8 );

	_pbMaskFrame++;
}

//------------------------------------------------------------------------
