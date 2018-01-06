#include "pbMotionArray.h"
#include "pcvConverter.h"

//-----------------------------------------------
pbMotionArray::pbMotionArray(void)
{
	_w = _h = _n = 0;
}

//-----------------------------------------------
pbMotionArray::~pbMotionArray(void)
{
}

//-----------------------------------------------
void pbMotionArray::setup( cv::Rect_<float> relFrame,	//рамка дл€ €чеек
				int numW, int numH,			//число €чеек по высоте и ширине
					float cellPercent,			//сколько процентов пикселов должно быть включено, чтоб сработало
					float cellDecaySec,			//длительность работы после включени€ - вли€ет на врем€ последнего срабатывани€
					
					float usefulTimeSec,	//сколько секунд считаетс€ рабочим врем€ загорани€
					int chainLength				//длина цепочки, обычно 3 или 2	
					)
{
	_kUsefulTimeSec = usefulTimeSec;
	_kChainLength = chainLength;

	_w = numW;
	_h = numH;
	_n = _w * _h;
	_cell.resize( _n );
	for ( int y=0; y<_h; y++ ) {
		for (int x=0; x<_w; x++) {
			cv::Rect_<float> rect 
				= pcvConverter::relRectToAbsRect<float>( cv::Rect_<float>( 1.0 * x / _w, 1.0 * y / _h, 1.0 / _w, 1.0 / _h ),
							relFrame );

			_cell[ x + _w * y ].setup( rect, cellPercent, cellDecaySec );
		}
	}
}

//-----------------------------------------------
void pbMotionArray::updateCamera( float dt, 
		const Mat &imageBinary,			//8-битное, значени€ 0 и 255
		bool debugDraw )
{
	for (int i=0; i<_n; i++) {
		_cell[i].updateCamera( dt, imageBinary, debugDraw );
	}
}

//-----------------------------------------------
bool pbMotionArray::checkChain( const vector<int> &ind )	//проверить цепочку
{
	//–асчет, включена ли цепочка.
	//„тоб цепочка сработала - надо, чтоб все элементы загорелись по пор€дку,
	//причем с дальностью событий не более kUsefulTimeSec

	int n = ind.size();
	bool on = ( n > 0 );
	float time = ofGetElapsedTimef();
	for (int i=n-1; i>=0; i--) {
		float time1 = _cell[ ind[ i ] ].lastEnableTime();
		if ( time - time1 > _kUsefulTimeSec || time - time1 <= 0 ) {
			on = false;
		}
		time = time1;
	}
	return on;
}

//-----------------------------------------------
cv::Rect_<float> pbMotionArray::resultChain( int i, int j )	//генерирует результирующую цепочку
{
	cv::Point_<float> p1 = pcvConverter::rectCenter<float>( _cell[i].rect() );
	cv::Point_<float> p2 = pcvConverter::rectCenter<float>( _cell[j].rect() );
	return cv::Rect_<float>( p1.x, p1.y, p2.x - p1.x, p2.y - p1.y );
}

//-----------------------------------------------
void pbMotionArray::update( float dt )
{
	for (int i=0; i<_n; i++) {
		_cell[i].update( dt );
	}
	//ѕроверка цепочек
	int len = _kChainLength;
	if ( len > 0 ) {
		_chain.clear();
		vector<int> chain( len );
		//right
		for (int y=0; y<_h; y++) {
			for (int x=0; x<_w - len; x++) {
				for (int i=0; i<len; i++) {
					chain[ i ] = ( x + i ) + _w * y;
				}
				if ( checkChain( chain ) ) {
					_chain.push_back( resultChain( chain[0], chain[ chain.size() - 1 ] ) );
					//cout << "chain " << x << " " << y << endl;
				}
			}
		}
		//left
		for (int y=0; y<_h; y++) {
			for (int x=0; x<_w - len; x++) {
				for (int i=0; i<len; i++) {
					chain[ i ] = ( x + len - 1 - i ) + _w * y;
				}
				if ( checkChain( chain ) ) {
					_chain.push_back( resultChain( chain[0], chain[ chain.size() - 1 ] ) );
					//cout << "chain " << x << " " << y << endl;
				}
			}
		}
	}

}


//-----------------------------------------------
//дифференциалы
int pbMotionArray::differencial( int dirXMin, int dirXMax,	//вектор направлени€, в каком считать дифференци€л
								 int dirYMin, int dirYMax,
					float deltaTimeSec,	//врем€, когда два событи€ считать св€занными
					float epsTimeSec	//врем€, меньше которого два событи€ считать одновременными
					)
{
	int sum = 0;
	for (int y = cv::max(0, -dirYMin ); y < cv::min( _h, _h - dirYMax ); y++) {
		for (int x = cv::max(0, -dirXMin); x < cv::min( _w, _w - dirXMax ); x++) {
			const pbMotionCell &c0 = cell( x, y );
			if ( c0.isOn() ) {
				for (int y1 = y + dirYMin; y1 <= y + dirYMax; y1++) {
					for (int x1 = x + dirXMin; x1 <= x + dirXMax; x1++)  {
						const pbMotionCell &c1 = cell( x1, y1 );
						if ( c1.isOn() ) {
							float delta = c1.lastEnableTime() - c0.lastEnableTime();
							if ( delta <= deltaTimeSec && delta >= epsTimeSec ) {
								sum++;
							}
						}				
					}
				}
			}
		}
	}
	return sum;
}

//-----------------------------------------------
void pbMotionArray::draw( float x, float y, float w, float h )
{
	for (int i=0; i<_n; i++) {
		_cell[i].draw( x, y, w, h );
	}

	//рисуем сработавшие цепочки
	/*if ( _n > 0 ) {
		cv::Rect_<float> rect0 = pcvConverter::relRectToAbsRect<int>( _cell[0].rect(), cv::Rect( x, y, w, h ) );
		for (int i=0; i<_chain.size(); i++) {
			cv::Rect_<float> rect = _chain[i];
			//рисуем стрелку от начала к концу
			rect = pcvConverter::relRectToAbsRect<int>( rect, cv::Rect( x, y, w, h ) );

			if ( rect.width > 0 ) {
				ofSetColor( 0, 255, 0 );
			}
			else {
				ofSetColor( 255, 0, 0 );
			}
			ofSetLineWidth( 5 );
			ofLine( rect.x, rect.y, rect.x + rect.width, rect.y + rect.height );
			ofFill();
			ofCircle( rect.x, rect.y, cv::min( rect0.width, rect0.height ) / 8 );
		}
	}*/
}

//-----------------------------------------------
void pbMotionArray::reset()
{
	for (int i=0; i<_n; i++) {
		_cell[i].reset();
	}

}

//-----------------------------------------------
