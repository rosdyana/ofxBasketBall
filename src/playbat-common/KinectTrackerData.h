#pragma once

//Данные о людях в сцене

#include "ofMain.h"
#include "ofxIniSettings.h"


//Части тела
static const int kKinectTorso = 0;
static const int kKinectBoundBox0 = 1;
static const int kKinectBoundBox1 = 2;
static const int kKinectHead = 3;
static const int kKinectNeck = 4;
static const int kKinectLShoulder = 5;
static const int kKinectRShoulder = 6;
static const int kKinectLElbow = 7;
static const int kKinectRElbow = 8;
static const int kKinectLHand = 9;
static const int kKinectRHand = 10;
static const int kKinectLHip = 11;
static const int kKinectRHip = 12;
static const int kKinectLKnee = 13;
static const int kKinectRKnee = 14;
static const int kKinectLFoot = 15;
static const int kKinectRFoot = 16;
static const int kKinectPoints = 17;

int kinectNameToInt( const string &name );
string kinectIntToName( int index );
ofColor kinectPartColor( int index );
int kinectIsBodyPart( int index );

//Состояние человека
const int KinectPersonEmpty = 0;
const int KinectPersonAppear = 1;
const int KinectPersonTracking = 2;
const int KinectPersonDissapear = 3;

//Максимальное число людей
const int KinectMaxPersons = 15;

struct KinectTrackerDataItem;
struct KinectTrackerDataPerson;

//Калибровка данных
struct KinectTrackerCalibrator {
	//xml-файл
	void setup( const string &fileName, float w, float h );
	
	//конвертировать миллиметры в экранные координаты, с учетом сдвига и поворота камеры
	void convertMmToPixel( KinectTrackerDataItem &item );
	void convertMmToPixel( KinectTrackerDataPerson &person );
	
	//конвертировать миллиметры в сдвинутые миллиметры
	void convertMmToMm( const KinectTrackerDataItem &item, KinectTrackerDataItem &itemOut );

	bool enabled() { return _enabled; }

	//рисовать сверху или сбоку
	void drawTopSideMm( const vector<KinectTrackerDataPerson> &persons, int topSide );	

	//рисовать спереди, калиброванное
	void drawFrontPixels( const vector<KinectTrackerDataPerson> &persons );	


	void shiftReset( int topSide );	//сбросить сдвиги сбоку или сверху
	void shiftMove( const ofPoint &shiftMm, const ofPoint &rearShiftMm );	//калибровка

	void skewReset();				//сброс сдвига
	void skewMove( float shiftSkewZ );	//изменение сдвига

private:
	string _fileName;
	bool _enabled;
	float _w, _h;			//экранные координаты

	ofPoint _screenSizeMm;	//размер экрана
	ofPoint _roomSizeMm;	//размер большой комнаты (просто брать с запасом
	ofPoint _shiftMm;		//сдвиг плоскости у камеры
	ofPoint _rearShiftMm;	//сдвиг дальней плоскости

	float _skewZ;			//сдвиг верх-низ по Z
	
	ofPoint _scale;			//масштаб мм -> пикселы

	void load();
	void save();
};


//Часть тела
struct KinectTrackerDataItem {
	//bool enabled;		//используются ли эти данные
	bool valid;			//правильные ли данные
	ofPoint pixelRaw;	//значение пиксельное, некалиброванное
	ofPoint pixel;		//значение пиксельное
	ofPoint mm;			//значение мм

	KinectTrackerDataItem() { /*enabled = false;*/ valid = false; }
	void update( bool valid0, const ofPoint &pixel0, const ofPoint &mm0 ){
		valid = valid0; pixelRaw = pixel0; pixel = pixel0; mm = mm0;
	}
};

//Параметры
struct KinectTrackerDataParam { 
	float receiverWaitNoTracker;	//сколько ждать сигнала с кинекта прежде чем отключить человека
	void load( ofxIniSettings &ini );
};

//Один человек
struct KinectTrackerDataPerson {
	KinectTrackerDataPerson();
	void setup( float w, float h, const KinectTrackerDataParam &param );
	void updateReceiver( float dt );							//если давно не было - исчезает
	void updateCameraReceiver( bool signal );			//просто знак что пришел сигнал с камеры

	string toString( bool pixels, bool mm, float wPix, float hPix,
		const vector<int> &enabledParts );							//для передачи по сети

	bool empty() { return state==KinectPersonEmpty; }
	void setState( int state0 );

	void draw( float w, float h );			//рисовать вид спереди, в пикселах
	void boundBoxMm( ofPoint &origin, ofPoint &size );		//возвращает габаритный прямоугольник с данных

	int id;
	int state;		//KinectPersonEmpty, KinectPersonAppear, KinectPersonTracking, KinectPersonDissapear
	vector<KinectTrackerDataItem> item;

	void drawSegmentTopSideMm( int index0, int index1, int topSide );	//не ставит цвет
	void drawSegmentFrontPixel( int index0, int index1 );		//не ставит цвет

private:
	KinectTrackerDataParam _param;
	float _w, _h;
	float _time;				//время последнего прихода данных, увеличивается 

	void drawSegment( float w, float h, int index0, int index1 );	//не ставит цвет

};


//Люди в сцене
struct KinectTrackerData {
	KinectTrackerData();
	void setup( float w, float h, const string &fileName, const string &calibrFileName,
		int autobots = 0 );
	void updateReceiver( float dt );						//нужен только приемнику данных
	void updateFromString( const string &s );
	string toString( bool pixels, bool mm, float wPix, float hPix );		//для передачи по сети
	void clear();


	void draw( float w, float h );			//рисовать вид спереди, в пикселах
	void drawFrontPixels();					//рисовать вид спереди, калиброванный, в пикселах
	void drawTopMm();	//рисовать вид сверху, мм
	void drawSideMm();	//рисовать вид сбоку, мм

	vector<KinectTrackerDataPerson> &person() { return _person; }
	vector<int> &enabledParts() { return _enabledParts; }		//какие части надо распознавать

	KinectTrackerCalibrator &calibrator() { return _calibrator; }
private:
	KinectTrackerDataParam _param;
	float _w, _h;

	vector<KinectTrackerDataPerson> _person;
	int _N;

	vector<int> _enabledParts;

	KinectTrackerCalibrator _calibrator;
};
