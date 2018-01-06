#include "pbPinch.h"
#include "highgui.h"

#include "ofxVectorMath.h"



//-----------------------------------------------------------------------
void pbPinch::setup(const string &settingsFileName, int flowW, int flowH)
{
    const float kActivatedLength = flowW * 0.1;		//Необходимая длина для активации щипка
    //--------------------------

    _w = flowW;
    _h = flowH;
    _euler.setup(_w, _h, kActivatedLength);
    _motion.setup();



}

//-----------------------------------------------------------------------
void pbPinch::updateDiff(float dt, const Mat &image, bool debugDraw)
{
    //ПАРАМЕТР порог
    const float kDiffThresh = //10;		//низкая освещенность - шумы
        3;  		//хорошая освещенность

    const int blurRad = 0.02 * min(_w, _h);			//ПАРАМЕТР
    //----------------------


    //разность картинок
    if (_imgLast.empty()) {
        _imgLast = image.clone();
    }
    _imgDiffRGB = image - _imgLast;

    threshold(_imgDiffRGB, _imgDiffRGB, kDiffThresh, 255, CV_THRESH_BINARY);

    //минимум по каналам
    vector<Mat> planes;
    split(_imgDiffRGB, planes);
    _imgDiff = cv::min(planes[0], planes[1]);
    _imgDiff = cv::min(_imgDiff, planes[2]);


    /*if ( debugDraw ) {
    	Mat large;
    	int zoom = 3;

    	resize( _imgDiff, large, cv::Size( _w * zoom, _h * zoom ) );
    	imshow( "d1", large );

    }*/

    _imgLast = image.clone();

}


//-----------------------------------------------------------------------
/*void pbPinch::updateHistory( float dt, const Mat &image32, bool debugDraw )	//история кадров для вычисл фона
{
	const float kFilter = 0.1;
							//0.05;	//Фильтр низких частот для фильтра  //TODO сделать независимым от FPS

	//----------------------
	if ( _imgHistory.empty() ) {
		_imgHistory = image32.clone();
	}

	addWeighted( _imgHistory, 1 - kFilter, image32, kFilter, 0.0, _imgHistory );

	Mat diff = image32 - _imgHistory;
	diff = abs( diff );
	cvtColor( diff, _imgHistoryBin, CV_RGB2GRAY );
	threshold( _imgHistoryBin, _imgHistoryBin, 10.0 / 255.0 , 1.0, CV_THRESH_BINARY );


	if ( debugDraw ) {
		//imshow( "history", _imgHistory );
		imshow( "hist diff", _imgHistoryBin );
	}
}
*/

//-----------------------------------------------------------------------
void pbPinch::updateFlow(float dt, const Mat &image0,  //const Mat &flow,
                         bool debugDraw)
{
    const float kThreshOptFlowOutlier = 0.16;		//ПАРАМЕРТР выбросы оптического потока, независимые от размера картинки
    const int blurRad = //0.02 * _w;		//ПАРАМЕТР сглаживание
        0.005 * _w;


    //----------------------
    //Вычисление картинки, которую использовать
    /*Mat image;
    Mat bin, bin3;
    vector<Mat> planes(4);

    _imgHistoryBin.convertTo( bin, CV_8UC1, 1.0 );//255.0 );
    planes[0] = bin;
    planes[1] = bin;
    planes[2] = bin;
    planes[3] = bin;
    merge( planes, bin3 );

    //imshow( "bin", bin );
    multiply( image0, bin3, image );
    //image = image0.mul( bin );
    imshow("img", image );*/

    const Mat &image = image0;

    //----------------------
    //Расчет потока
    resize(image, _imgFlowColor, cv::Size(_w, _h));

    cvtColor(_imgFlowColor, _imgFlow, CV_RGB2GRAY);

    if (_imgFlowLast.empty()) {
        _imgFlowLast = _imgFlow.clone();
    }
    _motion.update(_imgFlowLast, _imgFlow);
    _imgFlowLast = _imgFlow.clone();


    //Выбрасывание выбросов
    const Mat &flow = *_motion.flow();

    vector<Mat> flow_planes;
    split(flow, flow_planes);

    {
        Mat flowX2, flowY2;
        pow(flow_planes[0], 2, flowX2);
        pow(flow_planes[1], 2, flowY2);
        Mat flowLen = flowX2 + flowY2;
        cv::sqrt(flowLen, flowLen);
        const float kThreshLenValue = _w * kThreshOptFlowOutlier;

        Mat bin;
        threshold(flowLen, bin, kThreshLenValue, 1.0, CV_THRESH_BINARY_INV);
        if (debugDraw) {
            imshow("outliers", bin);
        }
        flow_planes[0] = flow_planes[0].mul(bin);
        flow_planes[1] = flow_planes[1].mul(bin);

    }


    //Сглаживание потока

    //Сильное сглаживание
    const int blurDiam = blurRad * 2 + 1;

    Mat blurredX, blurredY;
    GaussianBlur(flow_planes[0], blurredX, cv::Size(blurDiam, blurDiam), 0);
    GaussianBlur(flow_planes[1], blurredY, cv::Size(blurDiam, blurDiam), 0);

    Mat flowX2, flowY2;

    //Нахождение длины векторов потока
    pow(blurredX, 2, flowX2);
    pow(blurredY, 2, flowY2);
    Mat flowLen = flowX2 + flowY2;
    cv::sqrt(flowLen, flowLen);

    Mat blurred = flowLen;

    if (debugDraw) {
        imshow("blurred", blurred);
    }

    _blurredX = blurredX;
    _blurredY = blurredY;
    _blurredAbs = blurred;

}

//-----------------------------------------------------------------------
void pbPinch::updateCombined(float dt, bool debugDraw)
{
    const int blurRad = 0.08 * min(_w, _h);		//ПАРАМЕТР радиус сглаживания
    const float kThresh = 0.1;					//ПАРАМЕТР порог
    //----------------------

    Mat &flow = _blurredAbs;
    Mat &thresh0 = _imgDiff;

    Mat thresh;
    resize(thresh0, thresh, cv::Size(_w, _h));

    thresh.convertTo(thresh, CV_32FC1, 1.0 / 255.0);

    //Сильное сглаживание разности кадров


    const int blurDiam = blurRad * 2 + 1;
    Mat blurredX, blurredY;
    GaussianBlur(thresh, thresh, cv::Size(blurDiam, blurDiam), 0);

    /*if ( debugDraw ) {
    	imshow( "d2", thresh );
    }*/

    //умножение потока на thresh - для фильтрации шумов
    //Mat mult = _blurredAbs.mul( thresh );

    _blurredAbs = _blurredAbs.mul(thresh);
    _blurredX = _blurredX.mul(thresh);
    _blurredY = _blurredY.mul(thresh);


    /*if ( debugDraw ) {
    	imshow( "m3", _blurredAbs );
    }*/
}

//-----------------------------------------------------------------------
void pbPinch::updateCamera(float dt,  //const Mat &flow,
                           const Mat &image, bool debugDraw)
{
    if (debugDraw) {
        imshow("img", image);
    }

    image.convertTo(_image32, CV_32FC1, 1.0 / 255.0);

    //разность картинок
    updateDiff(dt, image, debugDraw);

    //история
    //updateHistory( dt, _image32, debugDraw );

    //поток
    updateFlow(dt, image,  //flow,
               debugDraw);

    //комбинирование
    updateCombined(dt, debugDraw);

    //щипки
    _euler.updateCamera(dt, _blurredX, _blurredY, _blurredAbs, debugDraw);

    //поинтер
    //_pointer.updateCamera( dt, _blurredX, _blurredY, _blurredAbs, debugDraw );

    //траектории поинтеров
    //_traj.updateCamera( dt, _pointer.isVisible(), _pointer.pointer(), debugDraw );
}


//-----------------------------------------------------------------------
void pbPinch::update(float dt)
{
    _euler.update(dt);
}

//-----------------------------------------------------------------------
void pbPinch::draw(float x, float y, float w, float h)
{
    float sclX = w / _w;
    float sclY = h / _h;

    _euler.draw(x, y, w, h);

}

//-----------------------------------------------------------------------
