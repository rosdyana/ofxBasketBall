#include "pbCalibrateRender.h"


//-----------------------------------------------------------------------
void pbCalibrateRender::setup( int w, int h )
{
	_w = w;
	_h = h;
	
	_inited = false;

	_buffer.allocate(w, h, false);
	_buffer.clear(0,0,0,0);
}

//-----------------------------------------------------------------------
void pbCalibrateRender::update()
{
}

//-----------------------------------------------------------------------
//4 �����, ����� ����� �� ������� �������
void pbCalibrateRender::setPoints( const vector<ofxVec2f> &p )
{
	_points = p;

	const int K = 4;
	cv::Point2f src[K];
	cv::Point2f dst[K];

	src[ 0 ] = cv::Point2f( 0, 0 );
	src[ 1 ] = cv::Point2f( _w, 0 );
	src[ 2 ] = cv::Point2f( _w, _h );
	src[ 3 ] = cv::Point2f( 0, _h );

	for (int i=0; i<K; i++) {
		dst[ i ] = cv::Point2f( p[ i ].x, p[ i ].y );
	}

	_transform = getPerspectiveTransform( src, dst );
	_inited = true;

}

//-----------------------------------------------------------------------
void pbCalibrateRender::grabBegin()	//��������� ��������� � �����
{
	//if ( _inited ) {
		//���������� �������, ��� ��� _buffer ������ ���
		glMatrixMode(GL_PROJECTION);
		ofPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		ofPushMatrix();

		_buffer.clear(0,0,0,0);
		_buffer.begin();

	//}
}

//-----------------------------------------------------------------------
void pbCalibrateRender::grabEnd()		//���������� ��������� � �����
{
	//if ( _inited ) {

		_buffer.end();

		//��������������� �������
		glMatrixMode(GL_MODELVIEW);
		ofPopMatrix();
		glMatrixMode(GL_PROJECTION);
		ofPopMatrix();
		glMatrixMode(GL_MODELVIEW);

	//}
}

//-----------------------------------------------------------------------
void pbCalibrateRender::draw( float w, float h, float alpha )	//����� �������� �� �����, � ������ ���������, ��������� enable
								//alpha - ��� ����� �������
{
	ofSetColor( 255, 255, 255, 255 * alpha );
	_buffer.draw( 0, 0, w, h );
}

//-----------------------------------------------------------------------
