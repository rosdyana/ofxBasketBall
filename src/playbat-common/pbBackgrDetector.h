#pragma once

#include "ofMain.h"
#include <cv.h>
#include "cvaux.h"

using namespace std;
using namespace cv;

class pbBackgrDetector
{
public:
	pbBackgrDetector(void);
	~pbBackgrDetector(void);

	void setup();
	void close();
	void reinit();					//перезапустить алгоритм распознавания фона (сработает по приходу нового кадра
	void updateCamera( float dt, const Mat &image, bool debugDraw );

	void update( float dt );

	const Mat &foreground();	//найденные пикселы переднего плана
	const Mat &background();	//текущий фон
	
private:
	void free();

	Mat _image;	

	CvBGStatModel* bg_model;

	//картинки для внешнего использования
	Mat _background;
	Mat _foreground;

};
