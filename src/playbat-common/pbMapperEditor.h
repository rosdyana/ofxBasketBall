#pragma once

//�������� �������� ��������, � ������ ��������

#include "ofMain.h"
#include "pbMapperShapes.h"


class pbMapperEditor
{
public:
	pbMapperEditor(void);
	~pbMapperEditor(void);

	void setup( float w, float h, pbMapperShape *shape );

	void enable( bool yes );
	bool enabled() { return _enabled; }

	//����� ������
	void setMode( int mode );
	int mode() { return _mode; }
	static const int ModeEdit = pbMapperShape::ModeTex;
	static const int ModeTuning = pbMapperShape::ModeScr;

	void update( float dt );
	void draw( ofTexture &tex );

	void mousePressed( ofPoint p, int button );
	bool keyPressed( int key );	//true - ������� ����������

private:
	bool _enabled;
	int _mode;
	float _w, _h;
	pbMapperShape *_shape;

	int _editMode;	//����� ���������
	static const int EditQuadSelect = 0;	//����� �����
	static const int EditVertexEdit = 1;	//�������������� �������
	static const int EditQuadCreate = 2;	//�������� ����� (����� ������ ������)
	static const int EditQuadMove = 3;		//����� ����� 
	static const int EditQuadScale = 4;		//��������� �������� ����� 
	static const int EditQuadGrid = 5;		//��������� �������� �����

	void setEditMode( int editMode );
	
	int _quad;	//������� ����
	int _v;		//������� �������
	int _quad1;	//���������� ����
	int _v1;	//���������� �������

	int _gridX, _gridY;			//������� �����
	ofTrueTypeFont	_font;


};
