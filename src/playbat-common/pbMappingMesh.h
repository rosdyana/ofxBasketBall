#pragma once

//Мэппинг набором треугольников

#include "ofMain.h"

typedef vector<int> pbMeshPolygon;

//Поверхность
class pbMesh {
public:
	vector<ofPoint> v;			//точки
	vector< pbMeshPolygon > poly;	//многоугольники
	void load( string fileName = "" );
	void save( string fileName = "" );
	void clear() { 
		v.clear();
		poly.clear();
	}
	void move( ofPoint shift );
	void resize( ofPoint shift, ofPoint center );
	void moveVertex( ofPoint shift, int i );

	//сделать сетку-круг
	void createCircle( ofPoint center, float rad, int segments );

	void draw( int selectedVertex = -1, int selectedColor = 0);	//рисует wireframe, без установки собственного цвета
private:
	string _fileName;
};

//Мэппинг
void pbMeshMapping( pbMesh &meshA, pbMesh &meshB, ofTexture &tex, bool wireframe = false, int onlyOneTriangle = -1 );


//Рендеринг
/*class pbMappingMesh
{
public:
	voi setup( string defaultFileName = "" );
	void save( string fileName = "" );
	void load( string fileName = "" );

	void update( float dt );
	
	void draw( ofTexture &tex, int mode = ModeScr );	//вывести текстуру
	void drawBinded( float texW, float texH, int mode = ModeScr );	//вывести текстуру, для которой уже сделан bind
	void drawWireframe( int mode, int selQ = -1, int selV = -1, int selQ1 = -1, int selV1 = -1,
							int colorVariant = -1
							);
private:
	

};*/
