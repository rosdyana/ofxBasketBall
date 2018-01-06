#pragma once

//анализ 3д-сцены на полу/столе


#include "ofMain.h"
#include "cv.h"
#include "pbCameraCalibrate.h"
#include "ofxVectorMath.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

using namespace cv;

//ѕараметры
struct pbDepthPlanar3DParam {
	int w, h;					//размеры картинки, в которую преобразовывать

	int doBackProjection;		//делать возврат точек на пол - замедл€ет работу, но повышает точность. ѕри калибровке - выключаетс€.
	float depthCalibrLow, depthCalibrHigh;	//пороги обрезани€ грубые - при калибровке
	float depthRawLow, depthRawHigh;	//пороги обрезани€ грубые - при первом проходе при использовании BackProjection
	float depthLow, depthHigh;	//пороги обрезани€ точные
	
	int kWarpedThreshold;		//ѕорог обработки раст€нутого warp-изображени€
	int depthErode;				//число эрозий дл€ удалени€ шумов
	int depthDilate;				//число эрозий дл€ удалени€ шумов
	int blobMinArea;			//количество точек дл€ фильтрации по размеру

	float backgroundLearnTime;		//параметр времени запоминани€ фона


	void load( const string &fileName );
	void save( const string &fileName );

};

class pbDepthPlanar3D
{
public:
	pbDepthPlanar3D(void);
	~pbDepthPlanar3D(void);

	void setup( const string &paramFileName, const string &calibrFileName );
	void update( const Mat &depth16, bool calibrateMode );

	void loadDefaultParam();
	void saveDefaultParam();
	
	void lock();			//дл€ многопоточной ситуации
	void unlock();

	const Mat &mask();			//результирующа€ маска

	vector<ofPoint> getWarpPoints();		//точки калибровки
	void setWarpPoints( vector<ofPoint> &p );

	void setBackProjection( bool value ) { _param.doBackProjection = value; };		//переключени€ режима обратной проекции с компенсацией пола

	pbDepthPlanar3DParam getParams();			//параметры
	void setParams( pbDepthPlanar3DParam &param );

	void learnBackground();	//выучить фон
	bool learningBackground() { return _learnBackgroundTime > 0; }	//идет ли обучение фона
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
	pbDepthPlanar3DParam _param;
	pbCameraCalibrate _calibrDepth;

	void loadParam();
	void saveParam();



	//»зображени€
	int _cameraW, _cameraH;
	Mat _depthSmall;
	Mat _back;

	ofxVec3f _backN;
	float _backOriginAxe;


	Mat _tempDiff;
	Mat _tempMask;

	Mat _preMask;	

	vector<XnPoint3D> _pScreen, _pWorld, _pWorldFloor, _pScreenFloor;	//точки в экранных и мировых координатах
	vector<Point2f> _pFloor, _pGame;			//напольные и игровые координаты
	Mat _tempMaskBackProj;

	Mat _tempMaskFiltration;
	Mat _mask;
	
	float _startTime;
	float _time;
	bool _learnBackgroundAtStart;	//надо учить фон при старте
	bool _learnBackground;			//хотим запомнить фон
	float _learnBackgroundTime;		//сколько идет запоминание, когда будет 0 - выключить

	//перевычислить фон при смене калибровки
	void recalcBackground();

	//расчет обратной проекции дл€ более точного позиционирование ноги
	void backProjection( Mat &inputMask, Mat &inputDepth, Mat &maskOut );

	//ќбработка маски
	void maskDenoiseFloodFill( const Mat &mask, Mat &maskOut, int minCount, int maxCount );
	void maskFilter( const Mat &mask, Mat &maskOut, Mat &maskTemp );

	Mat _erodeMask, _dilateMask;

};