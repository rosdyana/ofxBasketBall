#pragma once

//Слой

#include "ofMain.h"
#include "ofxIniSettings.h"
#include "pbNoiseGen.h"
#include "pbMakeColor.h"



//трансформация слоя
struct Transform2D {
	ofPoint shift;
	float angle;
	float scale;
	Transform2D() {
		reset();
	}
	Transform2D( ofPoint shift0, float angle0, float scale0 ) {
		shift = shift0;
		angle = angle0;
		scale = scale0;
	}
	void reset() {
		shift = ofPoint( 0, 0 );
		angle = 0;
		scale = 1.0;
	}
};

//Габаритный эллипс или прямоугольник вокруг объекта, используется для вычисления коллизий в игре
struct pbLayerBound {
	ofPoint pos;			
	float w, h;
	pbLayerBound() { w = 1.0; h = 1.0; };
	pbLayerBound( ofPoint pos0, float w0, float h0 ) {
		pos = pos0; w = w0; h = h0;
	}
	void loadFromString( const string &line );	//x; y; w; h, где x,y - относит. сдвиг от центра картинки
	pbLayerBound transformTo( const ofRectangle &rect );

	bool collision( pbLayerBound &bound ) const;	//пересекаются ли
	ofPoint collisionCenter( pbLayerBound &bound ) const;	//в случае пересечения - дает центр

	void draw( ofColor &color );

	bool operator==(const pbLayerBound &other) const {
		return (pos.x == other.pos.x && pos.y == other.pos.y && w == other.w && h == other.h );
	}
	bool operator!=(const pbLayerBound &other) const {
		return !(*this == other);
	}


};



//Параметры для одного представителя слоя
struct pbLayerInstance {
	bool enabled;
	ofPoint pos0;		//начальное расположение
	float z;
	float w, h;		//размеры, если 0 - то не используется
	float alpha;
	float alpha2;
	float alpha3;
	pbNoiseGen noiseAlpha;
	float getAlpha() { return ( alpha + noiseAlpha.value() ) * alpha2 * alpha3; }

	float seqNoisePos;
	double seqPos;
	float seqPosOSC;		//от 0 до 1, умножается на число кадров и добавляется к seqPos
	int frameToRender;
	void resetFrame()		{ seqPos = 0; frameToRender = 0; }	//надо оба обнулять, так как из seqPos выставляется frameToRender только в update

	pbNoiseGen noisePosX, noisePosY, noiseSize, noiseAngle, noiseAngleVeloc;	//влияние шума на положение слоя

	float noisePosVel, noiseSizeVel, noiseAngleVel;

	float kSeqFPS;

	float scale;			//масштаб
	float angle;			//поворот
	float angleVel;			//скорость вращения
	float angleShift;		//сдвиг угла - для тюнинга поворота
	int flipX, flipY;		//переворот по горизонтали и вертикали


	pbLayerBound bound;		//габаритный эллипс

	string category;		//категория, выставляется сама в setup
};


//Слой
class pbLayer
{
public:
	pbLayer(void);
	~pbLayer(void);

	//layers - передавать уже загруженные слои для устранения повторной загрузки картинок, если они уже были загрыжены в других слоях
	//Внимание: поэтому, если вы указываете непустое множество layers, 
	//тогда нельзя менять их после загрузки
	void setup( ofxIniSettings &ini, string section, vector<pbLayer> &layers, int loadedLayers );

	void update( float dt );
	void draw( float w, float h );
	void setShift( float shiftX, float shiftY );
	void setVisible( bool visible ) { _visible = visible; }
	void setScale( float scale, int i = 0 ) { _ins[i].scale = scale; }
	void setAngle( float angle, int i = 0 ) { _ins[i].angle = angle; }
	void setTranform( const Transform2D &t );	//универсальная трансформация

	string name() { return _name; }
	string category() { return _category; }
	float z(int instIndex = 0) { return _ins[ instIndex ].z; }
	bool visible() { return _visible; }
	ofRectangle getDrawRect( float screenW, float screenH, int instIndex = 0 ); 
	pbLayerBound getBound( float screenW, float screenH, int instIndex = 0 );

	vector<pbLayerInstance> &instances() { return _ins; }

	string mediaFile() { return _mediaFile; }
	vector<ofImage> &image() { return *_imageRef; }		//Контент

	float screenWidth() { return _W; }
	float screenHeight() { return _H; }

private:
	string _name;
	string _category;
	vector<ofImage> _image;
	string _mediaFile;	//имя файла с картинкой или папка с секвенцией 
						//- для устранения дублирования загрузки изображений, если они используются в разных слоях			
	int _mediaRef;		//номер слоя, из которого фактически берется медиа-информация
	vector<ofImage> *_imageRef;

	ofVideoPlayer _player;
	int _frameToRender;						//кадр в последовательности для рисования

	vector<pbLayerInstance> _ins;	//инстанции слоя

	ofPoint _ancor;		//анкор
	float _W, _H;		//размеры экрана, относительно которого заданы координаты

	float kShiftAmp;		//коэффициент сдвига от движений человека
	int _looped;			//закольцовывать ли
	ofPoint _shift;			//сдвиг на панораму

	float kDriftVelocity;	//постоянный сдвиг, для закольцованных
	float kDrift0, kDrift1;		//интервал где бегает drift
	float kDriftStartPos;		//стартовое значение
	float _drift;

	bool _isEmpty;			//это пустой слой, программа сама его наполняет
	bool _isWireframe;		//это пустой слой, отображаемый рамкой
	bool _isImage;			//это картинка
	bool _isSeq;			//это последовательность
	bool _isMovie;			//это видео
	bool _pingpong;			//режим pingpong

	float kSeqNoiseAmt;		//величина влияние шума на FPS
	float kSeqNoiseVel;		//скорость шума
	int kSeqDecimate;		//сколько в секвенции пропускать кадров (1 - не пропускать)
 
	bool _visible;
	int _blendMode;			//режим наложения
	static const int BlendModeNormal = 0;
	static const int BlendModeAdd = 1;

};

