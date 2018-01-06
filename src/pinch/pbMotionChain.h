#pragma once

//Цепочка из MotionCell,
//Позволяет детектировать направленное движение
//Для этого движение должно последовательно включиться во всех элементах цепочки

#include "ofMain.h"
#include <cv.h>
#include "pbMotionCell.h"

using namespace std;
using namespace cv;

class pbMotionChain
{
public:
    pbMotionChain(void);
    ~pbMotionChain(void);

    void setup(const vector< cv::Rect_<float> > &relRects,	//список прямоугольников-ячеек
               float cellPercent,			//сколько процентов пикселов должно быть включено, чтоб сработало
               float cellDecaySec,			//длительность работы после включения - влияет на время последнего срабатывания
               float usefulTimeSec		//сколько секунд считается рабочим время загорания
              );

    void updateCamera(float dt,
                      const Mat &imageBinary,			//8-битное, значения 0 и 255
                      bool debugDraw);


    void update(float dt);
    void draw(float x, float y, float w, float h);

    bool isOn()
    {
        return _isOn;    //цепочка включилась
    }

private:
    float _kUsefulTimeSec;	//Возможно, стоит хранить несколько последних моментов срабатывания ячеек

    vector<pbMotionCell> _cell;
    int _n;

    bool _isOn;

};
