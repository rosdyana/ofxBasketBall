#pragma once

//Интегратор оптического потока.
//Позволяет сглаживать движение областей типа лица


#include "ofMain.h"
#include "cv.h"
using namespace cv;

class pbOpticalIntegrator
{
public:
	void setup( int historyLength );
	void update( const Mat &image, int frame, bool debugDraw = false );

	//вычисление движения прямоугольника от frame0 до frame1.
	//оптический поток усредняется по всему прямоугольнику
	//поэтому, возможно, в задачах стоит передавать чуть уменьшенный прямоугольник
	//w, h - размеры экрана, в которых живет rect.
	//дело в том, что для скорости оптический поток обычно вычисляется на меньшем изображении
	ofRectangle integrateRect( const ofRectangle &rect, float w, float h, int frame0, int frame1 );

private:
	//История оптического потока, для восстановления положения лица
	int _n;						//длина истории
	int _optPos;				//положение в истории
	vector<int>_optFrame;
	vector<Mat>_optFlow;


	Mat _prevGray, _currentGray;	//картинки, подаваемые на детектор движения
	Mat _flow;						//оптический поток на данном кадре (с предыдущим кадром)
	Mat _flowBoxed;					//сглаженный оптический поток


};
