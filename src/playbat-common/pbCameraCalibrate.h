#pragma once

// алибровка камеры
//¬ключает в себ€ основную процедуру дл€ коррекции картинки,
//а также средства дл€ подготовки калибровочной информации
//TODO - запись-считывание ini

#include "ofMain.h"
#include "cv.h"
#include "pbScreenFormat.h"
//#include "ofxVectorMath.h"

using namespace cv;
using namespace std;

class pbCameraCalibrate
{
public:
    //—татические члены - можно обойтись ими

    //ѕостроение матрицы преобразовани€, 4 точки по часовой стрелке
    template< class PointClass >
    static Mat getTransform(vector<PointClass> &curve4, int outW, int outH)
    {

        const int K = 4;
        cv::Point2f src[K];
        cv::Point2f dst[K];

        src[ 0 ] = cv::Point2f(0, 0);
        src[ 1 ] = cv::Point2f(outW, 0);
        src[ 2 ] = cv::Point2f(outW, outH);
        src[ 3 ] = cv::Point2f(0, outH);

        for (int i = 0; i < K; i++) {
            dst[ i ] = cv::Point2f(curve4[ i ].x, curve4[ i ].y);
        }

        return getPerspectiveTransform(dst, src);
    }

    //ѕреобразование изображени€
    static void warp(const Mat &in, Mat &out, const Mat &transform, int outW, int outH)
    {
        cv::warpPerspective(in, out, transform, cv::Size(outW, outH), INTER_LINEAR);
    }

    //«апись калиброванных точек


public:
    //inoutFileName - если не пуста€ строка, то оттуда считываетс€ и записываетс€ автоматически
    //outW, outH - размеры выходной картинки, если 0 - берутс€ из первого кадра
    //но если автозагрузка - они должны быть установлены сразу
    void setup(int outW, int outH, const string &inoutFileName = "");

    void resetPoints();						//сброс точек, на всю картинку
    bool load(const string &fileName);
    bool save(const string &fileName);
    void setAutosave(const string &fileName);	//включить режим автосохранени€. ≈сли пуста€ строка - то выключить

    void transformImage(const Mat &image, Mat &imageOut);
    void transformPixels(vector<Point2f> &in, vector<Point2f> &out);

    void draw(float x, float y, float w, float h /*, const ofPoint &mousePos*/);

    bool enabled()
    {
        return _enabled;    //если выключен - то просто исходную картинку раст€гивает в нужный размер
    }
    void setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    //”становка данных
    //4 точки, обход точек по часовой стрелке
    void setPoints(const vector<ofPoint> &p);
    const vector<ofPoint> &points()
    {
        return _points;
    }

    int width()
    {
        return _outW;
    }
    int height()
    {
        return _outH;
    }

private:
    static const int K = 4;							//число точек дл€ калибровки
    bool _enabled;
    int _outW, _outH;
    vector<ofPoint> _points;

    Mat _transform;		//матрица преобразовани€

    //автосохранение калибровки
    string _autosaveFileName;

};
