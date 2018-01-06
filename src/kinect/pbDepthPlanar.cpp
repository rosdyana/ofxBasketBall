#include "highgui.h"
#include "ofxIniSettings.h"

#include "pbDepthPlanar.h"
#include "pbFiles.h"

int kDepthErode = 3;			//число шагов эрозии
int kDepthFilterMinCount = 50;	//минимальное число пикселов блоба, чтоб его не отфильтровали

//------------------------------------------------------------
pbDepthPlanar::pbDepthPlanar(void)
{
#ifdef TARGET_WIN32
    InitializeCriticalSection(&critSec);
#else
    pthread_mutex_init(&myMutex, NULL);
#endif
}

pbDepthPlanar::~pbDepthPlanar(void)
{
}

//------------------------------------------------------------
void pbDepthPlanar::lock()
{

#ifdef TARGET_WIN32
    EnterCriticalSection(&critSec);
#else
    pthread_mutex_lock(&myMutex);
#endif
}

//------------------------------------------------------------
void pbDepthPlanar::unlock()
{

#ifdef TARGET_WIN32
    LeaveCriticalSection(&critSec);
#else
    pthread_mutex_unlock(&myMutex);
#endif
}

//------------------------------------------------------------
void pbDepthPlanar::setup(const string &paramFileName, const string &calibrFileName)
{
    lock();
    //параметры
    _param.w = 320;
    _param.h = 240;
    _param.depthErode = 3;
    _param.filterMinCount = 50;
    _param.depthLow = 100;
    _param.depthHigh = 300;

    ofxIniSettings ini;
    ini.load(paramFileName);
    _param.w		= ini.get("Floor.processW", _param.w);
    _param.h		= ini.get("Floor.processH", _param.h);
    _param.depthErode		= ini.get("Floor.kDepthErode", _param.depthErode);
    _param.filterMinCount	= ini.get("Floor.kDepthFilterMinCount", _param.filterMinCount);

    //считывание порогов, настроенных в реальном режиме времени
    loadParam();

    //калибровка
    _calibrDepth.setup(_param.w, _param.h, calibrFileName);

    _startTime = -1;			//еще не стартовали
    _learnBackground = false;

    unlock();
}

//------------------------------------------------------------
void pbDepthPlanar::update(const Mat &depth16)
{
    lock();
    float time = ofGetElapsedTimef();
    if (_startTime < 0) {
        _startTime = time;
    }

    //уменьшаем размер для скорости
    resize(depth16, _depthSmall16, cv::Size(_param.w, _param.h), 0.0, 0.0, CV_INTER_NN);

    const float maxDist = 5000.0;
    //_depthSmall16.convertTo( _depthSmallF, CV_32FC1, -1.0 /  5000.0, 1.0 );
    _depthSmall16.convertTo(_depthSmallF, CV_32FC1, -1.0, maxDist);
    //_depthSmallF = _depthSmall16;


    _calibrDepth.transformImage(_depthSmallF, _depthWarped);
    //imshow( "depth", (1.0 / maxDist) * _depthSmallF );

    //запоминание фона
    if (_back.empty() || _learnBackground) {
        float kBackAutoSetSec = 3.0;			//сколько ждать сек после старта кинекта, чтоб установить фон
        if (time <= _startTime + kBackAutoSetSec || _learnBackground) {
            _learnBackground = false;
            _back = _depthSmallF.clone();
            recalcBackground();
        }
    }
    //imshow( "back", (1.0 / maxDist) * _back );

    //построение маски
    _tempDiff = _depthWarped - _backWarped;

    //imshow( "diff", (1.0 / maxDist) * _tempDiff );

    makeMask(_tempDiff, _tempMask, _mask, _param, false);

    unlock();
}

//------------------------------------------------------------
//перевычислить фон при смене калибровки
void pbDepthPlanar::recalcBackground()
{
    if (!_back.empty()) {
        _calibrDepth.transformImage(_back, _backWarped);
    }
}

//------------------------------------------------------------
const Mat &pbDepthPlanar::mask()						//результирующая маска
{
    //Тут не надо lock, так как только на считывание
    return _mask;
}

//------------------------------------------------------------
vector<ofPoint> pbDepthPlanar::getWarpPoints()		//точки калибровки
{
    lock();
    vector<ofPoint> p = _calibrDepth.points();
    unlock();
    return p;
}

//------------------------------------------------------------
void pbDepthPlanar::setWarpPoints(vector<ofPoint> &p)
{
    lock();
    _calibrDepth.setPoints(p);
    recalcBackground();		//перевычислить фон
    unlock();
}

//------------------------------------------------------------
void pbDepthPlanar::learnBackground()	//выучить фон
{
    lock();
    _learnBackground = true;
    unlock();
}

//------------------------------------------------------------
void pbDepthPlanar::resetCorners()	//сбросить углы
{
    lock();
    _calibrDepth.resetPoints();
    recalcBackground();		//перевычислить фон
    unlock();
}

//------------------------------------------------------------
//Удаление шумов с помощью floodFill
//TODO вынести в общие процедуры
//если minCount = -1, то его не использовать. Аналогично с maxCount.
void pbDepthPlanar::maskDenoiseFloodFill(const Mat &mask, Mat &maskOut, int minCount, int maxCount)
{

    maskOut = mask.clone();
    Mat &img = maskOut;

    //перебор пикселов изображения
    int w = img.size().width;
    int h = img.size().height;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int value = img.at<uchar>(y, x);
            if (value == 255) {          //если значение - 255, то заливаем
                //значением 200
                cv::Rect rect;        //сюда запишется габаритный прямоугольник
                int count = cv::floodFill(img, cv::Point(x, y), Scalar(200), &rect);
                bool ok = true;
                if (minCount >= 0 && count < minCount) {
                    ok = false;
                }
                if (maxCount >= 0 && count > maxCount) {
                    ok = false;
                }
                if (!ok) {	//если плохой - затираем
                    floodFill(img, cv::Point(x, y), Scalar(0), &rect);
                }
            }
        }
    }
    threshold(img, img, 1, 255, CV_THRESH_BINARY);	//Все пикселы с яркостью 200 переводим обратно в 255

}

//-----------------------------------------------------------------
void pbDepthPlanar::makeMask(const Mat &diff, Mat &tempMask, Mat &maskOut,
                             const pbDepthPlanarParam &param,
                             bool debugDraw)
{
    tempMask = Mat(diff.size(), CV_8U);
    //cout << "diff " << cv::mean( diff )[0] << endl;

    cv::inRange(diff, cv::Scalar(param.depthLow), cv::Scalar(param.depthHigh), tempMask);
    //imshow( "mask0", tempMask );

    //морфология
    //for (int i=0; i < param.depthErode; i++) {
    //	erode( mask, mask, cv::Mat() );		//!! надо в другую маску поместить те,
    //которые обрезались, и их вычесть, с меньшим значением
    //- чтоб floodfill с них просто не стартовал
    //}
    if (debugDraw) {
        imshow("mask", tempMask);
    }

    //устраняем "старые" фрагменты - которые были на предыдущих кадрах
    /*Mat maskf;
    mask.convertTo( maskf, CV_32F, 1.0/255.0 );
    if ( _depthHist.empty() ) {
    _depthHist = maskf.clone();
    }
    else {

    }*/

    //Удаление мелких шумов
    //TODO !!! может быть причиной сильного замедления работы, если много белых областей
    maskDenoiseFloodFill(tempMask, maskOut, param.filterMinCount, -1);

}

//-----------------------------------------------------------------
pbDepthPlanarParam pbDepthPlanar::getParams()			//параметры
{
    return _param;
}

//-----------------------------------------------------------------
void pbDepthPlanar::setParams(pbDepthPlanarParam &param)
{
    _param = param;
    saveParam();
}

//-----------------------------------------------------------------
//Запись/считывание порогов
void pbDepthPlanar::loadParam()
{
    vector<float> v = pbFiles::readFloats("data/_thresh.dat");
    if (v.size() == 2) {
        _param.depthLow = v[0];
        _param.depthHigh = v[1];
    }
}

//-----------------------------------------------------------------
void pbDepthPlanar::saveParam()
{
    vector<float> v(2);
    v[0] = _param.depthLow;
    v[1] = _param.depthHigh;
    pbFiles::writeFloats(v, "data/_thresh.dat");
}

//-----------------------------------------------------------------
