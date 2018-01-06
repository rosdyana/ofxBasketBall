#pragma once

//����

#include "ofMain.h"
#include "ofxIniSettings.h"
#include "pbNoiseGen.h"
#include "pbMakeColor.h"



//������������� ����
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

//���������� ������ ��� ������������� ������ �������, ������������ ��� ���������� �������� � ����
struct pbLayerBound {
	ofPoint pos;			
	float w, h;
	pbLayerBound() { w = 1.0; h = 1.0; };
	pbLayerBound( ofPoint pos0, float w0, float h0 ) {
		pos = pos0; w = w0; h = h0;
	}
	void loadFromString( const string &line );	//x; y; w; h, ��� x,y - �������. ����� �� ������ ��������
	pbLayerBound transformTo( const ofRectangle &rect );

	bool collision( pbLayerBound &bound ) const;	//������������ ��
	ofPoint collisionCenter( pbLayerBound &bound ) const;	//� ������ ����������� - ���� �����

	void draw( ofColor &color );

	bool operator==(const pbLayerBound &other) const {
		return (pos.x == other.pos.x && pos.y == other.pos.y && w == other.w && h == other.h );
	}
	bool operator!=(const pbLayerBound &other) const {
		return !(*this == other);
	}


};



//��������� ��� ������ ������������� ����
struct pbLayerInstance {
	bool enabled;
	ofPoint pos0;		//��������� ������������
	float z;
	float w, h;		//�������, ���� 0 - �� �� ������������
	float alpha;
	float alpha2;
	float alpha3;
	pbNoiseGen noiseAlpha;
	float getAlpha() { return ( alpha + noiseAlpha.value() ) * alpha2 * alpha3; }

	float seqNoisePos;
	double seqPos;
	float seqPosOSC;		//�� 0 �� 1, ���������� �� ����� ������ � ����������� � seqPos
	int frameToRender;
	void resetFrame()		{ seqPos = 0; frameToRender = 0; }	//���� ��� ��������, ��� ��� �� seqPos ������������ frameToRender ������ � update

	pbNoiseGen noisePosX, noisePosY, noiseSize, noiseAngle, noiseAngleVeloc;	//������� ���� �� ��������� ����

	float noisePosVel, noiseSizeVel, noiseAngleVel;

	float kSeqFPS;

	float scale;			//�������
	float angle;			//�������
	float angleVel;			//�������� ��������
	float angleShift;		//����� ���� - ��� ������� ��������
	int flipX, flipY;		//��������� �� ����������� � ���������


	pbLayerBound bound;		//���������� ������

	string category;		//���������, ������������ ���� � setup
};


//����
class pbLayer
{
public:
	pbLayer(void);
	~pbLayer(void);

	//layers - ���������� ��� ����������� ���� ��� ���������� ��������� �������� ��������, ���� ��� ��� ���� ��������� � ������ �����
	//��������: �������, ���� �� ���������� �������� ��������� layers, 
	//����� ������ ������ �� ����� ��������
	void setup( ofxIniSettings &ini, string section, vector<pbLayer> &layers, int loadedLayers );

	void update( float dt );
	void draw( float w, float h );
	void setShift( float shiftX, float shiftY );
	void setVisible( bool visible ) { _visible = visible; }
	void setScale( float scale, int i = 0 ) { _ins[i].scale = scale; }
	void setAngle( float angle, int i = 0 ) { _ins[i].angle = angle; }
	void setTranform( const Transform2D &t );	//������������� �������������

	string name() { return _name; }
	string category() { return _category; }
	float z(int instIndex = 0) { return _ins[ instIndex ].z; }
	bool visible() { return _visible; }
	ofRectangle getDrawRect( float screenW, float screenH, int instIndex = 0 ); 
	pbLayerBound getBound( float screenW, float screenH, int instIndex = 0 );

	vector<pbLayerInstance> &instances() { return _ins; }

	string mediaFile() { return _mediaFile; }
	vector<ofImage> &image() { return *_imageRef; }		//�������

	float screenWidth() { return _W; }
	float screenHeight() { return _H; }

private:
	string _name;
	string _category;
	vector<ofImage> _image;
	string _mediaFile;	//��� ����� � ��������� ��� ����� � ���������� 
						//- ��� ���������� ������������ �������� �����������, ���� ��� ������������ � ������ �����			
	int _mediaRef;		//����� ����, �� �������� ���������� ������� �����-����������
	vector<ofImage> *_imageRef;

	ofVideoPlayer _player;
	int _frameToRender;						//���� � ������������������ ��� ���������

	vector<pbLayerInstance> _ins;	//��������� ����

	ofPoint _ancor;		//�����
	float _W, _H;		//������� ������, ������������ �������� ������ ����������

	float kShiftAmp;		//����������� ������ �� �������� ��������
	int _looped;			//�������������� ��
	ofPoint _shift;			//����� �� ��������

	float kDriftVelocity;	//���������� �����, ��� ��������������
	float kDrift0, kDrift1;		//�������� ��� ������ drift
	float kDriftStartPos;		//��������� ��������
	float _drift;

	bool _isEmpty;			//��� ������ ����, ��������� ���� ��� ���������
	bool _isWireframe;		//��� ������ ����, ������������ ������
	bool _isImage;			//��� ��������
	bool _isSeq;			//��� ������������������
	bool _isMovie;			//��� �����
	bool _pingpong;			//����� pingpong

	float kSeqNoiseAmt;		//�������� ������� ���� �� FPS
	float kSeqNoiseVel;		//�������� ����
	int kSeqDecimate;		//������� � ��������� ���������� ������ (1 - �� ����������)
 
	bool _visible;
	int _blendMode;			//����� ���������
	static const int BlendModeNormal = 0;
	static const int BlendModeAdd = 1;

};

