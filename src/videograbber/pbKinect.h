#pragma once

//Работа с Kinect

//Настройка:
//1. Поставить _Kinect в папку выше openFrameworks, http://ajaxorg.posterous.com/kinect-driver-for-windows-prototype
//2. Добавить Kinect как существующий проект в решение
//3. Поставить в нем С/C++-Code Generation-Runtime library = 
//	для Release - Multithreaded Dll (так как проект openFrameworks такой)
//  для Debug - Multithreaded Debug Dll
//4. В настройках решения поставить зависимоть вашего проекта от проекта Kinect



#include "cv.h"
#include "ofMain.h"

using namespace cv;
using namespace std;

extern Mat _pbKinectNullMat;

class pbKinect

{
public:
	//Параметры устройства
	static const int KinectDepthRangeLow = 1;         //[KinectDepthLow, KinectDepthHigh - 1] - интервал правильных значений глубины
	static const int KinectDepthRangeHigh = 0x07ff;	

	static const int KinectFrameWidth	= 640;
	static const int KinectFrameHeight	= 480;


	//Вспомогательные функции
	static void KinectDepthToWorld(float &x, float &y, float &z) {};
	static void KinectWorldToRGBSpace(float &x, float &y, float z) {};
	static float KinectDepthValueToZ(unsigned short Depth) { return 0; };
	static unsigned short KinectZToDepthValue( float z ) { return 0; };
	static bool KinectIsDepthValid(unsigned short Depth) { return 0; };

	//Построить маску допустимых значений
	static void KinectDepthToValidMask( const Mat &depth, Mat &validMask ) {};

	//Конвертировать к floating-point
	static void KinectDepthToFloatDepth( const Mat &depth, Mat &floatDepth ) {};
	static const double KinectFloatRangeHigh() { return 0.84; }		//наблюдал это максимальное значение

public:

	void setup() {};
	void close() {};
	int cameraCount() { return 0; };
	bool startCamera(	int device,
						int w, int h,		
						int fps,														
						bool color,	//1- или 4-байтный						
						bool mirrorX						
						) { return 0; };		

	bool stopCamera() { return 0; };

	//Grab
	bool isFrameNew() { return 0; };
	Mat &grabColorFrameCV() { return _pbKinectNullMat; };	//Быстрая. Картинку не удалять и не менять
	Mat &grabDepthFrameCVRaw() { return _pbKinectNullMat; };	//Быстрая. Картинку не удалять и не менять
	Mat &grabDepthFrameCVFloat() { return _pbKinectNullMat; };	//Точность высокая, значения 0..1
	Mat &grabDepthFrameCV8bit() { return _pbKinectNullMat; };	//Точность низкая, для визуализации

	

private:
	Mat _frame;	
	Mat _depthFrame;
	Mat _depthFrameFloat;
	Mat _depthFrame8bit;

	void *_cam;
	bool _mirrorX;




};
