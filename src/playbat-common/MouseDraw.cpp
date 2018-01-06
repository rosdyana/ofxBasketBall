#include "MouseDraw.h"


//----------------------------------------------------------
MouseDrawSettings::MouseDrawSettings()
{
	n = 10;
	colorR	= 255;
	colorG	= 0;
	colorB	= 0;
	duration		=	0.5;
	appearDuration = 0.1;
	velocityKoef	= 1.0;
	sizeKoef		= 1.0;

	useImage = false;
}

//----------------------------------------------------------
void MouseDraw::setup( const MouseDrawSettings &settings )	//размеры окна вывода	
{
	_settings = settings;
	_part.resize( _settings.n );

	if ( _settings.useImage && _settings.imageFileName != "" ) {
		_image.loadImage( _settings.imageFileName );
	}
	
	_partCounter = 0;
}
	
//----------------------------------------------------------
void MouseDraw::free()						//очистка картинок
{

}

//----------------------------------------------------------
void MouseDraw::updateCamera( float dt, MotionBlobs &blobs )	//для данных с камеры
{
	_blobs = blobs;

	int _n = _part.size();
	for (int i=0; i<blobs.size(); i++) {
		MotionBlob &blob = blobs[ i ];
		//треугольники для блобов
		ofxVec2f p( blob.x, blob.y );
		ofxVec2f v( blob.fx, blob.fy );	
		bornParticle( p, v );
	}

}

//----------------------------------------------------------
void MouseDraw::bornParticle( const ofxVec2f &p, const ofxVec2f &v )
{	
	//ищем самую старую частицу
	int i = 0;
	int _n = _part.size();
	for (int j=0; j<_n; j++){
		if ( !_part[j].enabled || _part[ j ].bornTime < _part[ i ].bornTime ) {
			i = j;
		}
	}

	MouseParticle part;
	part.enabled = true;
	part.bornTime = ofGetElapsedTimef();
	part.p = p;
	part.v = v;
	part.length = v.length();
	_part[ i ] = part;

	//int _n = _part.size();
	//_partCounter++;
	//_partCounter %= _n;
}

//----------------------------------------------------------
//const float mouseParticleDuration = 0.5;		//ПАРАМЕТР
//const float mouseParticleAppearDuration = 0.1;		//ПАРАМЕТР

//----------------------------------------------------------
void MouseDraw::update( float dt )
{
	float now = ofGetElapsedTimef();

	int _n = _part.size();
	for ( int i=0; i<_n; i++ ) {
		MouseParticle &part = _part[ i ];
		if ( part.enabled ) {
			part.p += dt * part.v * _settings.velocityKoef;;
			if ( now - part.bornTime > _settings.duration ) {
				part.enabled = false;
			}
		}
	}

/*for (int i=0; i<blobs.size(); i++) {
			MotionBlob &blob = blobs[ i ];
			//треугольники для блобов
			ofxVec2f v( blob.fx, blob.fy );
			float len = v.length();*/

}

//----------------------------------------------------------
int MouseDraw::liveParticlesCount()
{
	int sum = 0;
	int _n = _part.size();
	for ( int i=0; i<_n; i++ ) {
		if (_part[ i ].enabled ) {
			sum++;
		}
	}
	return sum;
}

//----------------------------------------------------------
//возвращает все живые частицы
void MouseDraw::getMotionBlobs( MotionBlobs &outBlobs )		
{
	int _n = _part.size();
	int count = liveParticlesCount();
	outBlobs.resize( count );
	int counter = 0;
	for ( int i=0; i<_n; i++ ) {
		MouseParticle &part = _part[ i ];
		if ( part.enabled ) {
			MotionBlob blob;
			blob.fx = part.v.x;
			blob.fy = part.v.y;
			blob.x = part.p.x;
			blob.y = part.p.y;
			blob.n = 1.0;				//TODO можно вставить ее прозрачность
			blob.w = 1.0;
			blob.h = 1.0;
			outBlobs[ counter++ ] = blob;
		}
	}
}

//----------------------------------------------------------
void MouseDraw::draw()
{
	float now = ofGetElapsedTimef();

	int _n = _part.size();

	//ofEnableAlphaBlending();
	ofSetLineWidth( 2 );
	//ofFill();
	ofNoFill();

	for (int i=0; i<_n; i++) {
		if ( _part[ i ].enabled ) {
			MouseParticle &part = _part[ i ];
			ofxVec2f c = part.p;
			ofxVec2f v = part.v;
			float len = v.length();

			v.normalize();
			ofxVec2f norm = v.getPerpendicular();
			const float len1 = len * _settings.sizeKoef; //40;	//длина треугольника
			const float len2 = len/4 * _settings.sizeKoef; //10;	//длина треугольника
			ofxVec2f v1 = c + len1 * v;
			ofxVec2f v2 = c + len2 * norm;
			ofxVec2f v3 = c - len2 * norm;
			/*ofLine( v1.x, v1.y, v2.x, v2.y );
			ofLine( v2.x, v2.y, v3.x, v3.y );
			ofLine( v3.x, v3.y, v1.x, v1.y );
			*/

			if ( _settings.duration > 0 ) {
				float alpha;
				float delta = now - part.bornTime;
				if ( _settings.appearDuration < _settings.duration ) {
					if ( delta < _settings.appearDuration ) {
						alpha = delta / _settings.appearDuration;
					}
					else alpha = 1.0 - ( delta - _settings.appearDuration ) / ( _settings.duration - _settings.appearDuration );
				}
				else alpha = 1.0 - delta / _settings.duration;

				alpha = max( alpha, 0.0f );
				alpha = min( alpha, 1.0f );

				if ( !_settings.useImage ) {	//треугольником
					ofSetColor( _settings.colorR, _settings.colorG, _settings.colorB, alpha * 255 );
					ofTriangle( v1.x, v1.y, v2.x, v2.y, v3.x, v3.y );
				}
				else {	//картинкой
					float angle = atan2f( v.y, v.x ) * 360.0 / M_TWO_PI + 90;
					float scale = 1.0 * len / _image.getHeight() * _settings.sizeKoef;
					ofPushMatrix();					
					ofTranslate( c.x, c.y );	//перенос
					ofRotate( angle );	//вращение
					
					ofScale( scale, scale ); //увеличение размера
					_image.draw( -_image.getWidth() / 2, -_image.getHeight() / 2 );
					ofPopMatrix();

					
				}

				//стрелочки для блобов		
				//ofSetColor( 255, 0, 0 );
				//ofLine( blob.x, blob.y, blob.x + blob.fx, blob.y + blob.fy );
			}
		}
	}
	ofSetLineWidth( 1 );
	//ofDisableAlphaBlending();
	

}

//----------------------------------------------------------