#include "pbTextureLiquid.h"
#include "pbTextureUtil.h"
#include "ofxIniSettings.h"

//-------------------------------------------------------------------
void TextureLiquidParam::load( const string &fileName, const string &section0 )
{
	ofxIniSettings ini;
	ini.load( ofToDataPath( fileName ) );
	string section = section0 + ".";
	enabled		= ini.get( section + "enabled", 0 );
	wireframe	= ini.get( section + "wireframe", 0 );
	gridX		= ini.get( section + "gridX", 2 );
	gridY		= ini.get( section + "gridY", 2 );
	amp.x		= ini.get( section + "ampX", 30.0f );
	amp.y		= ini.get( section + "ampY", 30.0f );
	velocity.x	= ini.get( section + "velocityA", 0.3f );
	velocity.y	= ini.get( section + "velocityB", 0.1f );
}

//-------------------------------------------------------------------
void pbTextureLiquid::setup( int bufferW, int bufferH, const string &fileName, const string &section )
{
	_w = bufferW;
	_h = bufferH;
	_buffer.allocate(_w, _h, true);	//true - автоочистка на каждом шаге
	
	if ( fileName != "" && section != "" ) {
		_param.load( fileName, section );
	}

	paramUpdated();
}

//-------------------------------------------------------------------
void pbTextureLiquid::setParam( const TextureLiquidParam &param )
{
	_param = param;
	paramUpdated();
}

//-------------------------------------------------------------------
void pbTextureLiquid::paramUpdated()
{
	_W = _param.gridX;
	_H = _param.gridY;
	int n = _W * _H;
	int n1 = (_W-1) * (_H-1);
	int K = n1 * 4 * 2;
	_m.resize( n );
	_p.resize( K );
	_t.resize( K );
}

//-------------------------------------------------------------------
void pbTextureLiquid::update( float dt )
{
	float time = ofGetElapsedTimef();
	for ( int y=0; y<_H; y++ ) {
		for (int x=0; x<_W; x++) {
			if ( x==0 || x==_W-1 || y==0 || y==_H-1 ) {
				_m[ x + _W * y ] = ofPoint( 0, 0 );
			}
			else {
				ofPoint &amp = _param.amp;
				ofPoint &vel = _param.velocity;
				float dx = amp.x * ofNoise( x + time * vel.x, y + time * vel.y );
				float dy = amp.y * ofNoise( x + time * vel.y, y + time * vel.x );
				_m[ x + _W * y ] = ofPoint( dx, dy );			
			}
		}
	}
}

//-------------------------------------------------------------------
void pbTextureLiquid::draw( float w, float h )
{
	if ( w == 0 && h == 0 ) {
		w = _w; h = _h;
	}
	if ( _param.enabled ) {
		int n1 = (_W-1) * (_H-1);
		int k = 0;
		int x[4], y[4];
		for (int yi=0; yi<_H-1; yi++) {
			for (int xi=0; xi<_W-1; xi++) {			
				//индексы
				x[0] = xi;		y[0] = yi;
				x[1] = xi+1;	y[1] = yi;
				x[2] = xi+1;	y[2] = yi+1;
				x[3] = xi;		y[3] = yi+1;
				
				//рисуем
				for (int i=0; i<4; i++) {
					float tx = 1.0 * x[i] / (_W-1) * _w;
					float ty = 1.0 * y[i] / (_H-1) * _h;
					ofPoint &p = _m[ x[i] + _W * y[i] ];
					ofPoint t = ofPoint( tx, ty ); 
					_p[k]	= p.x + t.x;
					_p[k+1] = p.y + t.y;
					_t[k]	= t.x;
					_t[k+1]	= _h - 1 - t.y;
					k += 2;
				}
			}
		}
		pbTextureUtil::drawQuads( _p, _t, _buffer );


		if ( _param.wireframe ) {
			//тестовый вывод вершин
			ofFill();
			ofSetColor( 255, 0, 0 );
			for (int y=0; y<_H; y++) {
				for (int x=0; x<_W; x++) {
					ofPoint &p = _m[ x + _W * y ];
					float x1 = 1.0 * x / (_W-1) * _w + p.x;
					float y1 = 1.0 * y / (_H-1) * _h + p.y;
					ofCircle( x1, y1, 3 );
				}
			}
		}
		
		/*
		//медленно
		ofSetColor( 255, 255, 255 );
		_buffer.bind();
		glBegin( GL_QUADS );			
		int x[4], y[4];
		for (int yi=0; yi<_H-1; yi++) {
			for (int xi=0; xi<_W-1; xi++) {			
				//индексы
				x[0] = xi;		y[0] = yi;
				x[1] = xi+1;	y[1] = yi;
				x[2] = xi+1;	y[2] = yi+1;
				x[3] = xi;		y[3] = yi+1;
				
				//рисуем
				for (int i=0; i<4; i++) {
					float tx = 1.0 * x[i] / (_W-1) * _w;
					float ty = 1.0 * y[i] / (_H-1) * _h;
					ofPoint p = _m[ x[i] + _W * y[i] ];
					glTexCoord2f( tx, _h - 1 - ty ); 
					glVertex2f( tx + p.x, ty + p.y );
				}
			}
		}
		_buffer.unbind();
		glEnd();*/


	}
	else {
		_buffer.draw( 0, 0, w, h );
	}
}

//-------------------------------------------------------------------
void pbTextureLiquid::begin()
{
	_buffer.begin();
}

//-------------------------------------------------------------------
void pbTextureLiquid::end()
{
	_buffer.end();
}

//-------------------------------------------------------------------
