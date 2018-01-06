#include "pbMotionChain.h"
#include "pcvConverter.h"

//-----------------------------------------------
pbMotionChain::pbMotionChain(void)
{
	_kUsefulTimeSec = 1.0;

	_n = 0;
	_isOn = false;
}

//-----------------------------------------------
pbMotionChain::~pbMotionChain(void)
{
}

//-----------------------------------------------
void pbMotionChain::setup( const vector< cv::Rect_<float> > &relRects,	//список пр€моугольников-€чеек 
					float percent,			//сколько процентов пикселов должно быть включено, чтоб сработало
					float decaySec,			//длительность работы после включени€ - вли€ет на врем€ последнего срабатывани€
					float usefulTimeSec
					)
{
	_kUsefulTimeSec = usefulTimeSec;

	_n = relRects.size();
	_cell.resize( _n );
	for (int i=0; i<_n; i++) {
		_cell[i].setup( relRects[i], percent, decaySec );
	}
	_isOn = false;

}

//-----------------------------------------------
void pbMotionChain::updateCamera( float dt, 
		const Mat &imageBinary,			//8-битное, значени€ 0 и 255
		bool debugDraw )
{
	for (int i=0; i<_n; i++) {
		_cell[i].updateCamera( dt, imageBinary, debugDraw );
	}

}

//-----------------------------------------------
void pbMotionChain::update( float dt )
{
	for (int i=0; i<_n; i++) {
		_cell[i].update( dt );
	}
	//–асчет, включена ли цепочка.
	//„тоб цепочка сработала - надо, чтоб все элементы загорелись по пор€дку,
	//причем с дальностью событий не более kUsefulTimeSec
	bool on = ( _n > 0 );
	float time = ofGetElapsedTimef();
	for (int i=_n-1; i>=0; i--) {
		float time1 = _cell[i].lastEnableTime();
		if ( time - time1 > _kUsefulTimeSec || time - time1 <= 0 ) {
			on = false;
		}
		time = time1;
	}
	_isOn = on;

	//отладочна€ печать
	/*if ( on ) {
		string s;
		s += "pbMotionChain: ";
		for ( int i = 0; i < _n; i++ ) {
			s += " " + ofToString( _cell[i].lastEnableTime() );
		}
		s + ", now: " + ofToString( ofGetElapsedTimef() );
		cout << s << endl;
	}*/
}

//-----------------------------------------------
void pbMotionChain::draw( float x, float y, float w, float h )
{
	for (int i=0; i<_n; i++) {
		_cell[i].draw( x, y, w, h );
	}
	if ( isOn() ) {
		if ( _n > 0 ) {
			//рисуем стрелку от начала к концу
			cv::Rect_<float> rect1 = pcvConverter::relRectToAbsRect<int>( _cell[0].rect(), cv::Rect( x, y, w, h ) );
			cv::Rect_<float> rect2 = pcvConverter::relRectToAbsRect<int>( _cell[_n-1].rect(), cv::Rect( x, y, w, h ) );
			cv::Point_<float> p1 = ( rect1.tl() + rect1.br() ) * 0.5f;
			cv::Point_<float> p2 = ( rect2.tl() + rect2.br() ) * 0.5f;
			ofSetColor( 0, 255, 0 );
			ofSetLineWidth( 5 );
			ofLine( p1.x, p1.y, p2.x, p2.y );
			ofFill();
			ofCircle( p2.x, p2.y, cv::min( rect2.width, rect2.height ) / 8 );
		}
	}
}

//-----------------------------------------------
