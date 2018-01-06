#pragma once

//������� ������������� �������� �� �����

#include "ofMain.h"

class pbMapper
{
public:
	pbMapper(void);
	~pbMapper(void);
	void setup( int gridX, int gridY );

	void save( const string &fileName );
	bool load( const string &fileName );
	
	int gridX() { return _W; }
	int gridY() { return _H; }

	void setUniformGrid( float w, float h );
	void setUniformGridByCorners( const vector<ofPoint> &points );		//������ ��, ��� ��� gridX=gridY=2 - �������� ��������
	vector<ofPoint> corners();
	ofPoint center();						//���������� ����� - ������� �����

	vector<ofPoint> points() { return _points; }
	void setPoints( const vector<ofPoint> &points );
	void movePoint( int index, const ofPoint &shift );
	void moveAll( const ofPoint &shift );	//�������� ��
	void scaleBy( float multX, float multY );				//��������������, �������� ����������� ���������������
	

	void draw( ofTexture &tex );	//������� ��������
	void drawBinded( float texW, float texH );	//������� ��������, ��� ������� ��� ������ bind
	void drawWireframe( int selected = -1 );

private:
	int _W, _H;						//������� �����
	vector<ofPoint> _points;		//�����
	vector<float> surfGL;           //���������� �������� �� ����
	vector<float> texGL;            //���������� ���������� ��������
	float _texW, _texH;				//������� ��������, ��� ������� ���������� ����������
	bool _needUpdate;				//! ��� �������, �������� points, ������ ������������� ���� ���� � true
	
	void updateTexParams( float texW, float texH );

};
