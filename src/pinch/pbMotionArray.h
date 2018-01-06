#pragma once

/*
Матрица из ячеек движения
Может использоваться в двух режимах - либо через цепочки, в которых должны последовательно загореться ячейки,
либо используя дифференциалы - учет времени срабатывания соседних ячеек для вычисления направления движения объектов

Использование:

#include "pbMotionHistory.h"
#include "pbMotionArray.h"

pbMotionHistory _history;
pbMotionArray _array;
int _dirLeft = 0;
int _dirRight = 0;


setup:
	float kTimeFilter = 0.1f;			//Параметр фильтра низких чатот, чем меньше - тем длиннее история
	float kThresh = 30.0 / 255.0;		//Порог бинаризации разности истории и текущего кадра, от 0 до 1
	_history.setup( kTimeFilter, kThresh );


	float kPercent = 0.2;			//проценты для срабатывания
	float kDecay = 0.3;		 		//время загорания ячейки, до следующего включения
	float kUsefulTimeSec = 1.0;		//сколько времени "жизненная" значимость ячеек для цепочки
	int kChainLength = 0;			//длина цепочек для _array, 0 - значит, не использовать

	int w = 21;
	int h = 21;

	_array.setup( cv::Rect_<float>(0.0, 0.0, 1.0, 1.0 ), w, h, kPercent, kDecay, kUsefulTimeSec, kChainLength );

update:
	_history.update( dt );
	_array.update( dt );

	float kDeltaTimeSec = 3.0; //1.0;		//время, не более которого два события считать связанными
	float kEpsTimeSec = 1.0 / 25.0; //время, меньше которого два события считать одновременными
	_dirLeft = _array.differencial( -1, -1, -1, 1, kDeltaTimeSec, kEpsTimeSec );
	_dirRight = _array.differencial( 1, 1, -1, 1, kDeltaTimeSec, kEpsTimeSec );

	//Если нужно сбросить выключенные ячейки детектора
	//(например, пока вращается каталог, чтоб после этого сразу не закрутил) :
	...
	_array.reset();
	...

updateCamera:
	const Mat &img = grabber.grabFrameCV( procW(), procH() );
	_history.updateCamera( dt, img, debugDraw );
	_array.updateCamera( dt, _history.diffBinary(), debugDraw );

drawDebug:
	_array.draw( 0, 0, _w, _h );
	//Нарисовать также _dirLeft, _dirRight

*/

#include "ofMain.h"
#include <cv.h>
#include "pbMotionCell.h"

using namespace std;
using namespace cv;

class pbMotionArray
{
public:
    pbMotionArray(void);
    ~pbMotionArray(void);


    void setup(cv::Rect_<float> relFrame,	//рамка для ячеек
               int numW, int numH,			//число ячеек по высоте и ширине
               float cellPercent,			//сколько процентов пикселов должно быть включено, чтоб сработало
               float cellDecaySec,			//длительность работы после включения - влияет на время последнего срабатывания

               float usefulTimeSec,	//сколько секунд считается рабочим время загорания
               int chainLength				//длина цепочки, обычно 3 или 2. Если 0 - не считать их
               //вместо цепочек - также полезно считать дифференциалы
              );

    void updateCamera(float dt,
                      const Mat &imageBinary,			//8-битное, значения 0 и 255
                      bool debugDraw);


    void update(float dt);
    void draw(float x, float y, float w, float h);

    const vector< cv::Rect_<float> > &chains()
    {
        return _chain;    //список сработаших цепочек
    }

    //дифференциалы
    int differencial(int dirXMin, int dirXMax,	//вектор направления, в каком считать дифференциял
                     int dirYMin, int dirYMax,
                     float deltaTimeSec,	//время, не более которого два события считать связанными
                     float epsTimeSec	//время, меньше которого два события считать одновременными
                    );

    void reset();		//сброс всех включенных ячеек


    //Получение внутренних данных
    int w()
    {
        return _w;
    }
    int h()
    {
        return _h;
    }
    const pbMotionCell &cell(int x, int y)
    {
        return _cell[ x + _w * y ];
    }

    float usefulTimeSec()
    {
        return _kUsefulTimeSec;
    }


private:
    float _kUsefulTimeSec;	//Возможно, стоит хранить несколько последних моментов срабатывания ячеек
    int _kChainLength;

    vector<pbMotionCell> _cell;
    int _n, _w, _h;

    bool checkChain(const vector<int> &ind);	//проверить цепочку
    vector< cv::Rect_<float> > _chain;			//найденные цепочки

    cv::Rect_<float> resultChain(int i, int j);	//генерирует результирующую цепочку
};
