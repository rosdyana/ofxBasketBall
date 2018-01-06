#include "pbMapper.h"
#include "pbFiles.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------------
pbMapper::pbMapper(void)
{
}

//--------------------------------------------------------------------
pbMapper::~pbMapper(void)
{
}

//--------------------------------------------------------------------
void pbMapper::setup( int gridX, int gridY )
{
	_W = gridX;
	_H = gridY;

	_texW = 0;
	_texH = 0;
	
	_points.resize( _W * _H );
	surfGL.resize(  4 * 2 * ( _W - 1 ) * ( _H - 1 ) );
	texGL.resize( 4 * 2 * ( _W - 1 ) * ( _H - 1 ) );
	setPoints( _points );

	_needUpdate = false;

}
	
//--------------------------------------------------------------------
void pbMapper::setUniformGrid( float w, float h )
{
	for (int y=0; y<_H; y++) {
		for (int x=0; x<_W; x++) {
			_points[x + _W * y ] = ofPoint( w * x / (_W-1), h * y / (_H-1) ); 
		}
	}
	setPoints( _points );
}

//--------------------------------------------------------------------
void pbMapper::setPoints( const vector<ofPoint> &points )
{
	_points = points;
	_needUpdate = true;
}

//--------------------------------------------------------------------
void pbMapper::movePoint( int index, const ofPoint &shift )
{
	if ( index >= 0 && index < _points.size() ) {
		_points[index] += shift;
	}
	_needUpdate = true;
}

//--------------------------------------------------------------------
void pbMapper::moveAll( const ofPoint &shift )	//сдвинуть всё
{
	for (int i=0; i<_points.size(); i++) {
		_points[i] += shift;
	}
	_needUpdate = true;
}

//--------------------------------------------------------------------
void pbMapper::scaleBy( float multX, float multY )				//масштабировать, задается коэффициент масштабирования
{
	ofPoint c = center();
	for (int i=0; i<_points.size(); i++) {
		ofPoint &p = _points[i];
		ofPoint delta = p - c;
		p = c + ofPoint( delta.x * multX, delta.y * multY );
	}
	_needUpdate = true;
}

//--------------------------------------------------------------------
vector<ofPoint> pbMapper::corners()
{
	vector<ofPoint> p(4);
	p[0] = _points[ 0 + _W * 0 ];
	p[1] = _points[ (_W-1) + _W * 0 ];
	p[2] = _points[ 0 + _W * (_H-1) ];
	p[3] = _points[ (_W-1) + _W * (_H-1) ];
	return p;
}

//--------------------------------------------------------------------
ofPoint pbMapper::center()						//возвращает центр - среднее углов
{
	vector<ofPoint> p = corners();
	ofPoint p0;
	if ( p.size() > 0 ) {
		for (int i=0; i<p.size(); i++) {
			p0 += p[i];
		}
		p0 /= p.size();
	}
	return p0;
}

//--------------------------------------------------------------------
void pbMapper::setUniformGridByCorners( const vector<ofPoint> &p )		//лучшее ее, так как gridX=gridY=2 - искажает картинку
{
	//Точки:
	//0  1
	//2  3
	for (int y=0; y<_H; y++) {
		for (int x=0; x<_W; x++) {
			float a = 1.0 * x / (_W - 1 );
			float b = 1.0 * y / (_H - 1 );
			_points[ x + _W * y ] = 
				  p[0] * (1-a) * (1-b)
				+ p[1] * a * (1-b)
				+ p[2] * (1-a) * b
				+ p[3] * a * b;
		}
	}
	_needUpdate = true;
}

//--------------------------------------------------------------------
void pbMapper::drawWireframe( int selected )
{
	ofFill();
	for ( int y = 0; y < _H; y++ ) {
		for ( int x = 0; x < _W; x++ ){ 
			int index = x + _W * y;
			int rad = 5;
			ofSetColor( 0, 255, 0 );
			if ( index == selected ) {
				rad = 10;
				ofSetColor( 255, 0, 255 );
			}
			ofPoint &p = _points[ index ];
			ofCircle( p.x, p.y, rad );
		}
	}
}


//--------------------------------------------------------------------
void pbMapper::updateTexParams( float texW, float texH )
{
	_texW = texW;
	_texH = texH;

	int _x[4], _y[4];
	int index = 0;
	for ( int y = 0; y < _H - 1; y++ ) {
		for ( int x = 0; x < _W - 1; x++ ){ 
			_x[0] = x;		_y[0] = y;
			_x[1] = x+1;	_y[1] = y;
			_x[2] = x+1;	_y[2] = y+1;
			_x[3] = x;		_y[3] = y+1;
			for ( int i = 0; i < 4; i++ ) {
				float tx = 1.0 * _x[i] / ( _W - 1 ) * texW;
				float ty = 1.0 * _y[i] / ( _H - 1 ) * texH;
				ofPoint _p = _points[ _x[i] + _W * _y[i] ];

				texGL[ index ] = tx;
				texGL[ index + 1 ] = ty;

				surfGL[ index ] = _p.x;
				surfGL[ index + 1 ] = _p.y;
				index += 2;
			}
		}
	}
	_needUpdate = false;
}


//--------------------------------------------------------------------
void pbMapper::drawBinded( float texW, float texH )	//вывести текстуру, для которой уже сделан bind
{
	//если надо, пересчитать под новые размеры текстуры или под точки
	if ( _needUpdate || !(texW == _texW && texH == _texH ) ) {
		updateTexParams( texW, texH );		
	}		

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 2, GL_FLOAT, 0, &surfGL[0] );
	glTexCoordPointer( 2, GL_FLOAT, 0, &texGL[0] );
	
	glDrawArrays( GL_QUADS, 0, ( _W - 1 ) * ( _H - 1 ) * 4 );
	
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

}

//--------------------------------------------------------------------
void pbMapper::draw( ofTexture &tex )	//вывести текстуру
{
	float texW = tex.texData.tex_w;
	float texH = tex.texData.tex_h;

	tex.bind();
	drawBinded( texW, texH );
	tex.unbind();

}


//--------------------------------------------------------------------
void pbMapper::save( const string &fileName0 )
{
	string fileName = ofToDataPath( fileName0 );
	int n = _points.size();
	vector<float> p( 2 * n );
	for (int i=0; i<n; i++) {
		p[ 2 * i ] = _points[i].x;
		p[ 2 * i + 1 ] = _points[i].y;
	}
	pbFiles::writeFloats( p, fileName );
}

//--------------------------------------------------------------------
bool pbMapper::load( const string &fileName0 )
{
	string fileName = ofToDataPath( fileName0 );
	bool result = false;
	vector<float> p = pbFiles::readFloats( fileName );
	if ( p.size() == _points.size() * 2 ) {
		for (int i=0; i<_points.size(); i++) {
			_points[i].x = p[ 2 * i ];
			_points[i].y = p[ 2 * i + 1 ];
		}
		_needUpdate = true;
		result = true;
	}	
	return result;
}

//--------------------------------------------------------------------
