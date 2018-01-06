#pragma once

//Мэппинг прямоугольных областей на экран

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
	void setUniformGridByCorners( const vector<ofPoint> &points );		//лучшее ее, так как gridX=gridY=2 - искажает картинку
	vector<ofPoint> corners();
	ofPoint center();						//возвращает центр - среднее углов

	vector<ofPoint> points() { return _points; }
	void setPoints( const vector<ofPoint> &points );
	void movePoint( int index, const ofPoint &shift );
	void moveAll( const ofPoint &shift );	//сдвинуть всё
	void scaleBy( float multX, float multY );				//масштабировать, задается коэффициент масштабирования
	

	void draw( ofTexture &tex );	//вывести текстуру
	void drawBinded( float texW, float texH );	//вывести текстуру, для которой уже сделан bind
	void drawWireframe( int selected = -1 );

private:
	int _W, _H;						//размеры сетки
	vector<ofPoint> _points;		//сетка
	vector<float> surfGL;           //координаты текстуры на поле
	vector<float> texGL;            //координаты собственно текстуры
	float _texW, _texH;				//размеры текстуры, для которой рассчитаны координаты
	bool _needUpdate;				//! Все функции, меняющие points, должны устанавливать этот флаг в true
	
	void updateTexParams( float texW, float texH );

};
