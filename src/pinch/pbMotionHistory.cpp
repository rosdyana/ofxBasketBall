#include "pbMotionHistory.h"
#include "highgui.h"

//-----------------------------------------------
pbMotionHistory::pbMotionHistory(void)
{
	_kFilter = 0.5;
}

//-----------------------------------------------
pbMotionHistory::~pbMotionHistory(void)
{
}

//-----------------------------------------------
void pbMotionHistory::setup( float timeFilter, float thresh )
{
	_kFilter = timeFilter;
	_kThresh = thresh;
}

//-----------------------------------------------
void pbMotionHistory::updateCamera( float dt, const Mat &image, bool debugDraw )
{
	//Приводим к float
	image.convertTo( _image32, CV_32FC3, 1.0 / 255.0 );

	if ( _imageHist32.empty() ) {		
		_imageHist32 = _image32.clone();	
	}

	//вычисление истории
	addWeighted( _imageHist32, 1 - _kFilter, _image32, _kFilter, 0.0, _imageHist32 );


	//вычисление разности с текущим кадром
	_diff = _image32 - _imageHist32;
	_diff = abs( _diff );
	split( _diff, _diffPlanes );	
	cv::max( _diffPlanes[0], _diffPlanes[1], _diffGray );
	cv::max( _diffGray, _diffPlanes[2], _diffGray );

	//пороговая обработка
	threshold( _diffGray, _diffBin32, _kThresh, 1.0, CV_THRESH_BINARY );
	_diffBin32.convertTo( _diffBin8, CV_8UC1, 255.0 );
	
	
	if ( debugDraw ) {
		imshow( "history", _imageHist32 );
		imshow( "hist diff", _diffBin8 );
	}
}

//-----------------------------------------------
void pbMotionHistory::update( float dt )
{

}

//-----------------------------------------------
const Mat& pbMotionHistory::image8bit()
{
	_imageHist32.convertTo( _imageHist8, CV_8UC3, 255.0 );
	return _imageHist8;
}

	