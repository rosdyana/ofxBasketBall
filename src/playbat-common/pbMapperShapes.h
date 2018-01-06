#pragma once

//������� �� ������ ��������� 4-����������

#include "ofMain.h"
#include "ofxVectorMath.h"


//����������� �����
struct pbMapperStrucVertex {
	ofPoint p;		//[0,1]x[0,1] - ��������� � ������������ ��������������
	int parent;			//������������ 4-��

	pbMapperStrucVertex() { parent = -1; }
	pbMapperStrucVertex( const ofPoint p0, int parent0 ) { p = p0; parent = parent0; }
};

struct pbMapperStrucQuad {
	pbMapperStrucQuad() { }
	vector<int> iv;	//������� ������
};


//������
class pbMapperShape {
public:
	void setup( float w, float h, string fileStruc, string fileProj );
	void load();
	void save();
	void loadStruc( string fileName = "" );	
	void saveStruc( string fileName = "" );
	void loadProj( string fileName = "" );	
	void saveProj( string fileName = "" );

	float w() { return _w;}
	float h() { return _h;}

	void update();		//��������� ����������, ���� ����������

	//���������
	static const int ModeTex = 0;	//����� �������� �����
	static const int ModeScr = 1;	//����� ������������ �����

	void draw( ofTexture &tex, int mode = ModeScr );	//������� ��������
	void drawBinded( float texW, float texH, int mode = ModeScr );	//������� ��������, ��� ������� ��� ������ bind
	
	void drawWireframe( int mode, int selQ = -1, int selV = -1, int selQ1 = -1, int selV1 = -1,
							int colorVariant = -1
							);

	//��������������
	void editClearQuad( int q );
	void editFillQuadUniform( int q, int gridX, int gridY );
	void editAddPoint( int q, ofPoint p );		//������������ globalToLocalCoord()
	void editMovePoint( int mode, int v, ofPoint shift );	//����� �������� �����, �� ����� - �������� � ����� ����������� �� ����� shift
	void editMoveQuad( int mode, int q, ofPoint shift );
	void editResizeQuad( int mode, int q, ofPoint shift );
	int findNearestPointIndex( int q, ofPoint p );	//���� ��������� ����� � ������ ����� 4-���������

	int nextQuad( int q, int q1, int dir );	//����� ���������� � q �����, ����� q1
	int quadParent( int q );				//����� �������� ����� q
	int quadVertexIndex( int q, int num );

	//������
	void tuningReset();

private:
	float _w, _h;		//������� ��������� ������
	string _fileStruc, _fileProj;

	//��������� ��������
	vector<pbMapperStrucVertex> _v;	//����������� �����, � ��������� �����������
	vector<pbMapperStrucQuad> _q;	//4-���������

	//������
	vector<ofPoint> _proj;			//�������� ������, � ��������� �����������

	//�������������� �������, � ���������� �����������
	vector<ofPoint> _tex;			//���������� ���������� ������
	vector<ofPoint> _scr;			//�������� ������
	vector<bool> _qRender;			//���� �� ��������� ����
	vector<int> _qRenderList;		//������ ������ ��� ����������
	vector<float> texGL;            //OpenGL - ���������� ����������
	vector<float> surfGL;           //OpenGL - �������������� ����������

	void recalc();					//����������� ��� ��������� ������ //TODO ����� �������� �������� edit � tuning
	bool _needRecalc;

	void recalcActual();			//����������� ������ recalc
	void recalcOpenGL();			//����������� ������� OpenGL

	//��������� ���������� �����
	ofPoint getPointLocal( int mode, int i );
	//���������� ���������� �����
	ofPoint getPointGlobal( int mode, int i );

	//���������� ���������� �����-���������� �����
	vector<ofPoint> quadVerticesGlobal( int mode, int q ); 

	//������� ������ ��������� � �����
	ofPoint localToGlobalCoord( int mode, int q, const ofPoint &local );
	//����� �������� ������� ��������� ���������
	ofPoint globalToLocalCoord( int mode, ofPoint pLocal, ofPoint startPoint = ofPoint(0.5, 0.5) );
};


