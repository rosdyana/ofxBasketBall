#pragma once

//Обработка плоской поверхности, получение маски.
//! Внимание, сейчас этот модуль устарел, лучше пользоваться pbDepthPlanar3D

#include "ofMain.h"
#include "cv.h"
#include "pbCameraCalibrate.h"
using namespace cv;

//Параметры
struct pbDepthPlanarParam {
	int w, h;					//размеры картинки, в которую преобразовывать
	float depthLow, depthHigh;	//пороги обрезания
	int depthErode;				//число эрозий для удаления шумов
	int filterMinCount;			//количество точек для фильтрации по размеру

};

class pbDepthPlanar
{
public:
	pbDepthPlanar(void);
	~pbDepthPlanar(void);

	void setup( const string &paramFileName, const string &calibrFileName );
	void update( const Mat &depth16 );

	
	void lock();			//для многопоточной ситуации
	void unlock();

	const Mat &mask();			//результирующая маска

	vector<ofPoint> getWarpPoints();		//точки калибровки
	void setWarpPoints( vector<ofPoint> &p );

	pbDepthPlanarParam getParams();			//параметры
	void setParams( pbDepthPlanarParam &param );

	void learnBackground();	//выучить фон
	void resetCorners();	//сбросить углы

	pbCameraCalibrate &calibrator() { return _calibrDepth; }
private:

	//мютекс
	#ifdef TARGET_WIN32
			CRITICAL_SECTION  critSec;  	//same as a mutex
	#else
			pthread_mutex_t  myMutex;
	#endif

	//параметры
	pbDepthPlanarParam _param;
	pbCameraCalibrate _calibrDepth;

	void loadParam();
	void saveParam();


	//Изображения
	Mat _depthSmall16;
	Mat _depthSmallF;
	Mat _depthWarped;
	Mat _back;
	Mat _backWarped;

	Mat _tempDiff;
	Mat _tempMask;
	Mat _mask;
	
	float _startTime;
	bool _learnBackground;			//хотим запомнить фон

	//удаление шумов из маски
	static void maskDenoiseFloodFill( const Mat &mask, Mat &maskOut, int minCount, int maxCount );

	//построение маски
	static void makeMask( const Mat &diff, Mat &tempMask, Mat &maskOut, 
		const pbDepthPlanarParam &param,
		bool debugDraw );

	//перевычислить фон при смене калибровки
	void recalcBackground();


};
