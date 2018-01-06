#include "pbMotionCell.h"
#include "pcvConverter.h"

//-----------------------------------------------
pbMotionCell::pbMotionCell(void)
{
	_kPercent = 0.5;
	_kDecaySec = 1.0;

	_perc = 0.0;
	_lastEnableTime = 0;
}

//-----------------------------------------------
pbMotionCell::~pbMotionCell(void)
{
}

//-----------------------------------------------
void pbMotionCell::setup( cv::Rect_<float> relRect,		//пр€моугольник, в координатах [0, 1]x[0, 1]
				float percent,			//сколько процентов пикселов должно быть включено, чтоб сработало
				float decaySec			//длительность работы после включени€
				)
{
	_rect = relRect;
	_kPercent = percent;
	_kDecaySec = decaySec;
	_value.setup( 1.0 / _kDecaySec, 0.0 );

	_perc = 0.0;
	_lastEnableTime = 0.0;

}

//-----------------------------------------------
void pbMotionCell::updateCamera( float dt, const Mat &imageBinary, bool debugDraw )
{
	cv::Size size = imageBinary.size();
	cv::Rect r = pcvConverter::relRectToAbsRect<int>( _rect, cv::Rect( cv::Point(), size ) ); 
	Scalar m = mean( imageBinary( r ) );
	_perc = m[0] / 255.0;		//процент сработавших
	if ( _perc >= _kPercent ) {
		//если до этого был выключен, то запоминаем врем€ включени€
		if ( value() == 0.0f ) {
			_lastEnableTime = ofGetElapsedTimef();
		}
		_value.setValue( 1.0 );			//включаем
		_value.setTarget( 0.0 );		//сразу ставим чтоб убывал к 0
	}
}

//-----------------------------------------------
void pbMotionCell::update( float dt )
{
	_value.update( dt );
}

//-----------------------------------------------
void pbMotionCell::draw( float x, float y, float w, float h )
{
	cv::Rect_<float> r = pcvConverter::relRectToAbsRect<float>( _rect, cv::Rect_<float>( x, y, w, h ) ); 

	//процент заполненности
	ofSetColor( 255, 255, 255, 64 );
	ofFill();
	float h1 = r.height * cv::min(_perc / _kPercent, 1.0f);
	ofRect( r.x, r.y + r.height - h1, r.width, h1 );

	//рамка
	if ( value() > 0 ) {
		ofSetColor( 255, 255, 255 );
		ofSetLineWidth( 5 );
	}
	else {
		ofSetColor( 255, 0, 0 );
		ofSetLineWidth( 1 );
	}
	ofNoFill();
	ofRect( r.x, r.y, r.width, r.height );

}

//-----------------------------------------------
void pbMotionCell::reset()			//принудительное выключение €чейки
{
	_perc = 0;
	_value.setTarget( 0.0 );
	_value.setValue( 0.0 );

}

//-----------------------------------------------
