#pragma once

//Редактор разметки мэппинга, и самого мэппинга

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

	//Режим работы
	void setMode( int mode );
	int mode() { return _mode; }
	static const int ModeEdit = pbMapperShape::ModeTex;
	static const int ModeTuning = pbMapperShape::ModeScr;

	void update( float dt );
	void draw( ofTexture &tex );

	void mousePressed( ofPoint p, int button );
	bool keyPressed( int key );	//true - клавиша отработала

private:
	bool _enabled;
	int _mode;
	float _w, _h;
	pbMapperShape *_shape;

	int _editMode;	//режим редактора
	static const int EditQuadSelect = 0;	//выбор квада
	static const int EditVertexEdit = 1;	//редактирование вершины
	static const int EditQuadCreate = 2;	//создание квада (выбор вершин кликом)
	static const int EditQuadMove = 3;		//сдвиг квада 
	static const int EditQuadScale = 4;		//изменение размеров квада 
	static const int EditQuadGrid = 5;		//Настройка размеров грида

	void setEditMode( int editMode );
	
	int _quad;	//текущий квад
	int _v;		//текущая вершина
	int _quad1;	//выбираемый квад
	int _v1;	//выбираемая вершина

	int _gridX, _gridY;			//размеры сетки
	ofTrueTypeFont	_font;


};
