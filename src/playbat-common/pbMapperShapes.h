#pragma once

//ћэппинг на основе вложенных 4-угольников

#include "ofMain.h"
#include "ofxVectorMath.h"


// онтрольна€ точка
struct pbMapperStrucVertex {
	ofPoint p;		//[0,1]x[0,1] - положение в родительском пр€моугольнике
	int parent;			//родительский 4-уг

	pbMapperStrucVertex() { parent = -1; }
	pbMapperStrucVertex( const ofPoint p0, int parent0 ) { p = p0; parent = parent0; }
};

struct pbMapperStrucQuad {
	pbMapperStrucQuad() { }
	vector<int> iv;	//индексы вершин
};


//ћэппер
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

	void update();		//выполн€ет перерасчет, если необходимо

	//–исование
	static const int ModeTex = 0;	//показ исходных точек
	static const int ModeScr = 1;	//показ отображаемых точек

	void draw( ofTexture &tex, int mode = ModeScr );	//вывести текстуру
	void drawBinded( float texW, float texH, int mode = ModeScr );	//вывести текстуру, дл€ которой уже сделан bind
	
	void drawWireframe( int mode, int selQ = -1, int selV = -1, int selQ1 = -1, int selV1 = -1,
							int colorVariant = -1
							);

	//–едактирование
	void editClearQuad( int q );
	void editFillQuadUniform( int q, int gridX, int gridY );
	void editAddPoint( int q, ofPoint p );		//использовать globalToLocalCoord()
	void editMovePoint( int mode, int v, ofPoint shift );	//можно сдвигать точно, но проще - сдвинуть и потом нормировать на длину shift
	void editMoveQuad( int mode, int q, ofPoint shift );
	void editResizeQuad( int mode, int q, ofPoint shift );
	int findNearestPointIndex( int q, ofPoint p );	//ищет ближайщую точку в первых дет€х 4-угольника

	int nextQuad( int q, int q1, int dir );	//поиск вложенного в q квада, после q1
	int quadParent( int q );				//поиск родител€ квада q
	int quadVertexIndex( int q, int num );

	//“юнинг
	void tuningReset();

private:
	float _w, _h;		//размеры итогового экрана
	string _fileStruc, _fileProj;

	//—труктура мэппинга
	vector<pbMapperStrucVertex> _v;	//контрольные точки, в локальных координатах
	vector<pbMapperStrucQuad> _q;	//4-угольники

	//“юнинг
	vector<ofPoint> _proj;			//проекци€ вершин, в локальных координатах

	//–ассчитываемые массивы, в глобальных координатах
	vector<ofPoint> _tex;			//текстурные координаты вершин
	vector<ofPoint> _scr;			//проекци€ вершин
	vector<bool> _qRender;			//надо ли рендерить квад
	vector<int> _qRenderList;		//список квадов дл€ рендеринга
	vector<float> texGL;            //OpenGL - текстурные координаты
	vector<float> surfGL;           //OpenGL - результирующие координаты

	void recalc();					//пересчитать все положени€ вершин //TODO можно разнести отдельно edit и tuning
	bool _needRecalc;

	void recalcActual();			//фактический запуск recalc
	void recalcOpenGL();			//пересчитать массивы OpenGL

	//локальные координаты точки
	ofPoint getPointLocal( int mode, int i );
	//глобальные координаты точки
	ofPoint getPointGlobal( int mode, int i );

	//глобальные координаты точек-образующих квада
	vector<ofPoint> quadVerticesGlobal( int mode, int q ); 

	//перевод систем координат в кваде
	ofPoint localToGlobalCoord( int mode, int q, const ofPoint &local );
	//поиск делением пополам локальных координат
	ofPoint globalToLocalCoord( int mode, ofPoint pLocal, ofPoint startPoint = ofPoint(0.5, 0.5) );
};


