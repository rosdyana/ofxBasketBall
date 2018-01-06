#include "pbLayer.h"
//#include "MediaLoader.h"
#include "ofxDirList.h"
#include "parsing.h"
#include "pbOscParam.h"

//-------------------------------------------------------------------
void pbLayerBound::loadFromString( const string &line )	//x; y; w; h, где x,y - пикселы относительно центра картинки
{
	vector<float> list = Parsing::splitFilteredFloat( line, ";" );
	if ( list.size() >= 4 ) {
		pos.x = list[0];
		pos.y = list[1];
		w = list[2];
		h = list[3];
	}
}

pbLayerBound pbLayerBound::transformTo( const ofRectangle &rect )
{
	float W = rect.width;
	float H = rect.height;

	return pbLayerBound( 
		ofPoint( rect.x + (0.5 + pos.x ) * W, rect.y + (0.5 + pos.y ) * H ),
		W * w, H * h );
}


//-------------------------------------------------------------------
bool pbLayerBound::collision( pbLayerBound &bound ) const //пересекаютс€ ли
{
	bool res = ( max( pos.x - w/2, bound.pos.x - bound.w/2 ) <= min( pos.x + w/2, bound.pos.x + bound.w/2 )
		&& max( pos.y - h/2, bound.pos.y - bound.h/2 ) <= min( pos.y + h/2, bound.pos.y + bound.h/2 ) 
		);
	return res;
}

//-------------------------------------------------------------------
ofPoint pbLayerBound::collisionCenter( pbLayerBound &bound ) const	//в случае пересечени€ - дает центр
{
	return ofPoint( max( pos.x - w/2, bound.pos.x - bound.w/2 ) 
		+ min( pos.x + w/2, bound.pos.x + bound.w/2 ),
		max( pos.y - h/2, bound.pos.y - bound.h/2 )
		+ min( pos.y + h/2, bound.pos.y + bound.h/2 ) 
		) * 0.5;
}

//-------------------------------------------------------------------
void pbLayerBound::draw( ofColor &color )
{
	ofSetColor( color.r, color.g, color.b );
	ofNoFill();
	//ofEllipse( pos.x, pos.y, w, h );
	ofRect( pos.x - w/2, pos.y - h/2, w, h );
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
pbLayer::pbLayer(void)
{
}

//-------------------------------------------------------------------
pbLayer::~pbLayer(void)
{
}


//-------------------------------------------------------------------
float getValue( const vector<float> &v, int i )
{
	return ( v.size() > 0 ) ? v[ i % v.size() ] : 0;
}

//-------------------------------------------------------------------
//св€зать параметр сло€ с OSC-параметром
bool joinParameter( ofxIniSettings &ini, const string &section, const string &varName, float *var )
{
	bool result = false;
	string line = ini.get( section + "." + varName, string() );
	//edgesVis		-	 проста€ св€зь
	//edgesVis; peak=0.5,0.2,0.1    числа - это радиус, ширина, спадание пика, и выдает значение от 0 до 1

	vector<string> items = Parsing::splitFiltered( line, ";" );
	if ( items.size() > 0 ) {
		string oscName = items[0];		//им€ osc-параметра
		pbOscValue* oscParam = shared_param.find( oscName );
		if ( oscParam ) {
			string listenerParams = ( items.size() >= 2 ) ? items[1] : "";	//параметры osc
			pbOscValueListener listener;
			listener.setup( var, listenerParams );
			oscParam->addListener( listener );
			result = true;
		}
		else {
			cout << "WARNING: No OSC parameter " << oscName << " !!!" << endl;
		}
	}
	return result;
}

//-------------------------------------------------------------------
void pbLayer::setup( ofxIniSettings &ini, string section, vector<pbLayer> &layers, int loadedLayers )
{
	cout << "Loading " << section << "..." << endl;

	_W = ini.get( "LayerGeneral.w", 1024 );
	_H = ini.get( "LayerGeneral.h", 768 );

	float shiftX = ini.get("LayerGeneral.shiftX", 0.0f);
	float shiftY = ini.get("LayerGeneral.shiftY", 0.0f);


	string section0 = section;
	section = section + ".";	

	_name = ini.get( section + "name", string() );
	_category = ini.get( section + "category", string() );

	//cout << _category << endl;

	string type			= ini.get( section + "type", string() );
	string imageFile	= ini.get( section + "image", string() );
	string seqDir		= ini.get( section + "seq", string() );
	string movie		= ini.get( section + "movie", string() );
	
	string loopMode		= ini.get( section + "loopMode", string() );	//если pingpong, то специально обраб.
	_pingpong = ( loopMode == "pingpong" );

	kSeqDecimate		= ini.get( section + "seqDecimate", 1 );	
	kSeqDecimate = max( kSeqDecimate, 1 );

	//ќпределение типа сло€
	_isEmpty = ( type == "empty" );
	_isWireframe = ( type == "wireframe" );
	_isSeq = (seqDir != "" ) && !_isEmpty && !_isWireframe;
	_isMovie = (movie != "" ) && !_isSeq && !_isEmpty && !_isWireframe;
	_isImage = !_isMovie && !_isSeq && !_isEmpty && !_isWireframe;

	//ѕоиск референса в уже загруженных сло€х
	_mediaFile = "";
	_mediaRef = -1;
	_imageRef = &_image;			//??????????????



	if ( _isImage ) {
		_mediaFile = imageFile;
	}
	if ( _isSeq ) {
		_mediaFile = seqDir;
	}

	if ( _mediaFile != "" ) {
		for (int i=0; i<loadedLayers; i++) {
			if ( layers[i].mediaFile() == _mediaFile ) {
				//cout << "      - using content from layer " << layers[i].name() << endl;
				_mediaRef = i;
				_imageRef = &layers[i].image();
				break;
			}
		}
	}

	//lazyLoadImage( _image[0], imageFile );
	//«агрузка изображени€
	if ( _isImage && _mediaRef == -1 ) {
		_image.resize(1);
		_image[0].loadImage( imageFile );
		//if ( !ofFileExists( ofToDataPath( imageFile ) ) ) {
		//	cout << "ERROR: No file " << imageFile << endl;
		//}
	}
	//«агрузка последовательности
	if ( _isSeq && _mediaRef == -1) {
		ofxDirList dir;
		int n = dir.listDir( seqDir );
		if ( n == 0 ) {
			cout << "ERROR: Bad animation sequence - no files in dir " << seqDir << endl;
			ofSleepMillis( 4000 );
			OF_EXIT_APP( 0 );
		}
		int load = n / kSeqDecimate;
		_image.resize( load );
		for (int i=0; i<load; i++) {
			_image[i].loadImage( dir.getPath(i * kSeqDecimate) );
		}
	}
	//«агрузка видео
	if ( _isMovie ) {
		_player.loadMovie( movie );
	}

	//********** —читывание остальных параметров

	_visible = ini.get( section + "visible", 1 );
	if ( _isEmpty ) _visible = false;

	string blendMode = ini.get( section + "blendMode", string() );
	_blendMode = BlendModeNormal;
	if ( blendMode == "add" ) _blendMode = BlendModeAdd;

	ofPoint ancorScr;		//анкор координат, заданных в экране
	ancorScr.x= ini.get( section + "ancorScrX", 0.0f );
	ancorScr.y= ini.get( section + "ancorScrY", 0.0f );

	//”становка размеров кадра
	float imW = (_isImage || _isSeq) ? image()[0].width : 1.0;
	float imH = (_isImage || _isSeq) ? image()[0].height : 1.0;
	if ( _isMovie ) { 
		imW = _player.width;
		imH = _player.height;
	}

	kShiftAmp = ini.get( section + "shiftAmp", 0.0f );
	_shift = ofPoint( 0, 0 );

	kDriftVelocity = ini.get( section + "driftVelocity", 0.0f );
	kDrift0 = ini.get( section + "drift0", 0.0f );
	kDrift1 = ini.get( section + "drift1", 1.0f );
	kDriftStartPos = ini.get( section + "driftStartPos", 0.0f );
	_drift = kDriftStartPos;

	_looped = ini.get( section + "looped", 0 );


	kSeqNoiseAmt	= ini.get( section + "seqNoiseAmt", 0.0f );
	kSeqNoiseVel	= ini.get( section + "seqNoiseVel", 1.0f );

	_ancor.x= ini.get( section + "ancorX", 0.0f );
	_ancor.y= ini.get( section + "ancorY", 0.0f );


	//*********** »нстансы 

	int inst = ini.get( section + "instances", 1 );
	_ins.resize( inst );

	vector<float> enabledI = Parsing::splitFilteredFloat( ini.get( section + "enabled", string("1") ), ";" );
	vector<float> posXI = Parsing::splitFilteredFloat( ini.get( section + "x", string("0") ), ";" );
	vector<float> posYI = Parsing::splitFilteredFloat( ini.get( section + "y", string("0") ), ";" );
	vector<float> posZI = Parsing::splitFilteredFloat( ini.get( section + "z", string("0") ), ";" );	//Ќельз€ делать несколько!
	vector<float> wI	= Parsing::splitFilteredFloat( ini.get( section + "w", string("0") ), ";" );
	vector<float> hI	= Parsing::splitFilteredFloat( ini.get( section + "h", string("0") ), ";" );
	vector<float> alphaI= Parsing::splitFilteredFloat( ini.get( section + "alpha", string("1.0") ), ";" );
	vector<float> kSeqFPSI = Parsing::splitFilteredFloat( ini.get( section + "seqFPS", string("12.0") ), ";" );	

	vector<float> angleI = Parsing::splitFilteredFloat( ini.get( section + "angle", string("0") ), ";" );
	vector<float> angleVelI = Parsing::splitFilteredFloat( ini.get( section + "angleVel", string("0") ), ";" );
	vector<float> angleShiftI = Parsing::splitFilteredFloat( ini.get( section + "angleShift", string("0") ), ";" );
	vector<float> flipXI = Parsing::splitFilteredFloat( ini.get( section + "flipX", string("0") ), ";" );
	vector<float> flipYI = Parsing::splitFilteredFloat( ini.get( section + "flipY", string("0") ), ";" );

	pbLayerBound bound0;
	bound0.loadFromString( ini.get( section + "bound", string("") ) );

	for (int i=0; i<inst; i++) {
		pbLayerInstance &ins = _ins[i];
		ins.category = _category;
		ins.enabled = getValue( enabledI, i );
		ins.pos0.x	= getValue( posXI, i ) + shiftX + _W * ancorScr.x;
		ins.pos0.y	= getValue( posYI, i ) + shiftY + _H * ancorScr.y;
		//ins.pos0.z	= getValue( posZI, i );
		ins.z		= getValue( posZI, i );

		ins.scale = 1.0;
		ins.angle = getValue( angleI, i );
		ins.angleVel = getValue( angleVelI, i );
		ins.angleShift = getValue( angleShiftI, i );
		ins.flipX = getValue( flipXI, i );
		ins.flipY = getValue( flipYI, i );


		ins.alpha	= getValue( alphaI, i );
		ins.alpha2	= 1.0;
		ins.alpha3	= 1.0;
		ins.noiseAlpha.setup( ini.get( section + "noiseAlpha", string() ) );

		ins.kSeqFPS = getValue( kSeqFPSI, i );
		ins.w		= getValue( wI, i );
		ins.h		= getValue( hI, i );
		if ( ins.w == 0 && ins.h == 0 ) {
			ins.w = imW;
			ins.h = imH;
		}
		if ( ins.w == 0 ) {
			ins.w = imW * ins.h / imH;		
		}
		if ( ins.h == 0 ) {
			ins.h = imH * ins.w / imW;		
		}
		ins.seqNoisePos	= ofRandom( 0, 1000 );
		ins.seqPos			= ofRandom( 0, image().size() );
		ins.seqPosOSC		= 0;
		//Ўумовое движени€ сло€
		ins.noisePosX.setup( ini.get( section + "noisePosX", string() ) );
		ins.noisePosY.setup( ini.get( section + "noisePosY", string() ) );
		ins.noiseSize.setup( ini.get( section + "noiseSize", string() ) );
		ins.noiseAngle.setup( ini.get( section + "noiseAngle", string() ) );
		ins.noiseAngleVeloc.setup( ini.get( section + "noiseAngleVel", string() ) );

		ins.noisePosVel = 1.0;
		ins.noiseSizeVel = 1.0;
		ins.noiseAngleVel = 1.0;

		ins.frameToRender = 0;

		ins.bound = bound0;
	}


	//*********** OSC
	for (int i=0; i<inst; i++) {
		pbLayerInstance &ins = _ins[i];
		bool join;

		//alpha
		join = joinParameter( ini, section0, "oscAlpha", &ins.alpha );
		join = joinParameter( ini, section0, "oscAlpha2", &ins.alpha2 );
		join = joinParameter( ini, section0, "oscAlpha3", &ins.alpha3 );

		//seqPos
		ins.seqPosOSC = 0;
		join = joinParameter( ini, section0, "oscSeq", &ins.seqPosOSC );
		if ( join ) { ins.seqPos = 0; } //сбрасываем, а то она случайно ставитс€

		//fps
		join = joinParameter( ini, section0, "oscSeqFPS", &ins.kSeqFPS );

		//size
		join = joinParameter( ini, section0, "oscSize", &ins.scale );

		//angle
		join = joinParameter( ini, section0, "oscAngle", &ins.angle );

		//angleVel
		join = joinParameter( ini, section0, "oscAngleVel", &ins.angleVel );

		//noise
		join = joinParameter( ini, section0, "oscNoisePosVel", &ins.noisePosVel );
		join = joinParameter( ini, section0, "oscNoiseSizeVel", &ins.noiseSizeVel );
		join = joinParameter( ini, section0, "oscNoiseAngleVel", &ins.noiseAngleVel );
	}

}

//-------------------------------------------------------------------
void pbLayer::update( float dt )
{
	if ( fabs(kDriftVelocity) > 0 ) {
		_drift += kDriftVelocity * dt;
		while (_drift < kDrift0 && kDrift0 < kDrift1 ) _drift += kDrift1 - kDrift0;
		while (_drift > kDrift1 && kDrift0 < kDrift1 ) _drift -= kDrift1 - kDrift0;
	}
	for (int i=0; i<_ins.size(); i++ ) {
		pbLayerInstance &ins = _ins[i];
		if ( ins.enabled ) {
			if ( _isSeq || _isMovie ) {
				ins.seqNoisePos += kSeqNoiseVel * dt;
				float noise = ofSignedNoise( ins.seqNoisePos );
				ins.seqPos += ( ins.kSeqFPS + noise * kSeqNoiseAmt ) * dt;
			}
			ins.noisePosX.update( dt * ins.noisePosVel );
			ins.noisePosY.update( dt * ins.noisePosVel );
			ins.noiseSize.update( dt * ins.noiseSizeVel );
			ins.noiseAngle.update( dt );
			ins.noiseAngleVeloc.update( dt );
			ins.angle += ( ins.angleVel + ins.noiseAngleVeloc.value() * ins.noiseAngleVel ) * dt;		//вращение
			ins.noiseAlpha.update( dt );

			//обновление номера кадра
			if ( _isSeq || _isMovie ) {
				int nReal = 1;
				if ( _isSeq) { nReal = image().size(); }
				if ( _isMovie ) { 
					nReal = _player.getTotalNumFrames(); 
				}
				int n = (!_pingpong) ? nReal : (2*nReal-2);	//в пинг-понге больше кадров, фиктивные добавл€ем
				int seqI = floor( ins.seqPosOSC * (n-1) + ins.seqPos + 0.5 );
				if ( seqI < 0 ) {
					seqI = n - 1 - (-seqI) % n;
				}		
				else {
					seqI %= n;
				}
				//теперь если пинг-понг, то возвращаем в нормальные кадры
				if ( _pingpong ) {
					seqI = ( seqI < nReal ) ? seqI : ( 2*nReal - seqI - 1 );
				}

				ins.frameToRender = seqI;
				if ( _isMovie && _visible && ins.getAlpha() > 0 ) {
					_player.setFrame( ins.frameToRender );		//ƒелаем только если слой включен !!!
				}
			}
		}

	}


}


//-------------------------------------------------------------------
ofRectangle pbLayer::getDrawRect( float screenW, float screenH, int instIndex ) 
{
	pbLayerInstance &ins = _ins[instIndex];
	float scaleX = screenW/_W;
	float scaleY = screenH/_H;
	float shiftX = _shift.x * kShiftAmp * _W + _drift * ins.w;
	float shiftY = _shift.y * kShiftAmp * _H;	// + _drift.y * ins.h;
	float w = ins.w * (1 + ins.noiseSize.value()) * ins.scale;
	float h = ins.h * (1 + ins.noiseSize.value()) * ins.scale;
	float x = (-_ancor.x) * w + ins.pos0.x + shiftX + ins.noisePosX.value();
	float y = (-_ancor.y) * h + ins.pos0.y + shiftY + ins.noisePosY.value();
	return ofRectangle( x * scaleX, y * scaleY, w * scaleX, h * scaleY );
}


//-------------------------------------------------------------------
pbLayerBound pbLayer::getBound( float screenW, float screenH, int instIndex ) 
{
	pbLayerInstance &ins = _ins[instIndex];
	ofRectangle rect = getDrawRect( screenW, screenH, instIndex );
	return ins.bound.transformTo( rect );
}

//-------------------------------------------------------------------
void pbLayer::draw( float screenW, float screenH )
{
	if ( !_visible ) return;

	for (int i=0; i<_ins.size(); i++) {
		pbLayerInstance &ins = _ins[i];
		if ( ins.enabled ) {
			ofRectangle rect = getDrawRect( screenW, screenH, i ); 

			if ( _isWireframe ) {
				ofSetColor( 255, 255, 255 );
				ofNoFill();
				ofRect( rect.x, rect.y, rect.width, rect.height );
			}

			if ( _isSeq || _isImage || _isMovie ) {
				ofSetColor( 255, 255, 255, 255 * ins.getAlpha() );
				//int imageIndex = 0;
				vector<ofImage> &imageList = this->image();


				ofImage *image = ( !_isMovie ) ? &imageList[ins.frameToRender] : 0;
				ofTexture *tex = 0;
				if ( image ) { tex = &image->getTextureReference(); }
				if ( _isMovie ) { tex = &_player.tex; }

				//режим наложени€
				if ( _blendMode == BlendModeAdd ) {
					//http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
					//glBlendFunc(GL_ONE, GL_ONE);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);					
				}


				//ѕоворот: пока дл€ простоты поворачиваем картинку относительно центра, а не анкора
				float angle = ins.angle + ins.angleShift + ins.noiseAngle.value();
				if ( angle != 0 ) {
					ofPushMatrix();
					ofTranslate( rect.x + rect.width / 2, rect.y + rect.height / 2 );
					ofRotate( angle );
					if ( ins.flipX ) {
						ofScale( -1, 1 );
					}
					if ( ins.flipY ) {
						ofScale( 1, -1 );
					}
					if ( tex ) {
						tex->draw( -rect.width / 2, -rect.height / 2, rect.width, rect.height );
					}
					ofPopMatrix();
				}
				else {
					if ( tex ) {
						tex->draw( rect.x, rect.y, rect.width, rect.height );
					}
				}

				if ( _looped ) {
					if ( tex ) {
						if ( _drift >= 0 ) tex->draw( rect.x - rect.width, rect.y, rect.width, rect.height );
						if ( _drift <= 0 ) tex->draw( rect.x + rect.width, rect.y, rect.width, rect.height );
					}
				}

				//¬озврат нормального режима наложени€
				if ( _blendMode != BlendModeNormal ) {
					ofEnableAlphaBlending();
				}
			}
		}
	}
}

//-------------------------------------------------------------------
void pbLayer::setShift( float shiftX, float shiftY )
{
	_shift = ofPoint( shiftX, shiftY );
}

//-------------------------------------------------------------------
void pbLayer::setTranform( const Transform2D &t )	//универсальна€ трансформаци€
{
	setShift( t.shift.x, t.shift.y );
	setAngle( t.angle );
	setScale( t.scale );
}

//-------------------------------------------------------------------
