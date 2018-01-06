#pragma once

//������� ������� �������������

#include "ofMain.h"

typedef vector<int> pbMeshPolygon;

//�����������
class pbMesh {
public:
	vector<ofPoint> v;			//�����
	vector< pbMeshPolygon > poly;	//��������������
	void load( string fileName = "" );
	void save( string fileName = "" );
	void clear() { 
		v.clear();
		poly.clear();
	}
	void move( ofPoint shift );
	void resize( ofPoint shift, ofPoint center );
	void moveVertex( ofPoint shift, int i );

	//������� �����-����
	void createCircle( ofPoint center, float rad, int segments );

	void draw( int selectedVertex = -1, int selectedColor = 0);	//������ wireframe, ��� ��������� ������������ �����
private:
	string _fileName;
};

//�������
void pbMeshMapping( pbMesh &meshA, pbMesh &meshB, ofTexture &tex, bool wireframe = false, int onlyOneTriangle = -1 );


//���������
/*class pbMappingMesh
{
public:
	voi setup( string defaultFileName = "" );
	void save( string fileName = "" );
	void load( string fileName = "" );

	void update( float dt );
	
	void draw( ofTexture &tex, int mode = ModeScr );	//������� ��������
	void drawBinded( float texW, float texH, int mode = ModeScr );	//������� ��������, ��� ������� ��� ������ bind
	void drawWireframe( int mode, int selQ = -1, int selV = -1, int selQ1 = -1, int selV1 = -1,
							int colorVariant = -1
							);
private:
	

};*/
