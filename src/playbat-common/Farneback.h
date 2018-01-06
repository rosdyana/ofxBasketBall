#pragma once

#include <cv.h>
#include "MotionBlob.h"

using namespace std;
using namespace cv;



class Farneback
{
public:
    Farneback(void);
    ~Farneback(void);

    void setup();
    void setEnablePostProcessing(bool enable);		//надо ли находить блобы во время update

    void update(Mat &grayImg0, Mat &grayImg1);   //IplImage* grayImg0, IplImage* grayImg1 );

    Mat *flow()
    {
        return &_Flow;
    }
    bool empty()
    {
        return (width() == 0 || height() == 0);
    }

    unsigned char *outImageRGBA()
    {
        if (_outImage.size() > 0) {
            return &_outImage[0];
        } else {
            return 0;
        }
    }

    int width()
    {
        return _Flow.cols;
    }
    int height()
    {
        return _Flow.rows;
    }

    void setColor(int r, int g, int b);	//цвет вывода на экран

    vector<MotionBlob> motionBlobs(int sw, int sh);   //прямоугольники с движением в экранных координатах

    unsigned char *processCameraImageRGBA(unsigned char *imageRGB, int w0, int h0);
    //обработать картинку с камеры и вернуть указатель на обработанную

    void flowValue(int x, int y, float &fx, float &fy);	//Значение потока
    Point2f flowValue(int x, int y);

    //Значение потока для круглого объекта в экранных координатах
    //minLen - короче какой длины не подсчитывать,
    //minCount - сколько должно быть сработавших точек, чтоб засчитать не 0
    Point2f flowValueCircleScreen(int x, int y, int rad,
                                  int screenW, int screenH,
                                  float minLen, int minCount);

    //Значение потока для прямоугольника, в экранных координатах
    //minLen - короче какой длины не подсчитывать,
    //minCount - сколько должно быть сработавших точек, чтоб засчитать не 0
    Point2f flowValueRectScreen(int x0, int y0, int w0, int h0,
                                int screenW, int screenH,
                                float minLen, int minCount);

    //Нарисовать абсолютное значение поля
    void renderAbsField(IplImage *result);	//DEPRECATED

private:
    Mat _Flow;
    bool _inited;
    int _r, _g, _b;		//цвет вывода на экран


    vector<float>		  _flowLength;
    vector<unsigned char> _flowAbs;		//абсолютное значение - 0, 1, 2, используется для анализа областей
    vector<unsigned char> _flowAbsBlobs;		//абсолютное значение блобов
    vector<unsigned char> _outImage;		//картинка для вывода, RGBA

    bool _enablePostProcessing;
    void postProcessing();			//анализ поля движения, формирование текстуры, жестов

    //поиск областей движения. Портит image - оставляет только точки блобов.
    vector<MotionBlob> findMotionBlobs(unsigned char *image, int w, int h,
                                       int minFilter, int maxFilter);

    void renderOutImage(unsigned char *image);	//нарисовать изображение в outImage

    vector<cv::Point> _tempPoints;			//временный массив для хранения точек для поиска связной компоненты

    vector<MotionBlob>	_motionBlobs;		//результат поиска блобов


    vector<unsigned char>_cameraImage;		//обработанная картинка с камеры

};
