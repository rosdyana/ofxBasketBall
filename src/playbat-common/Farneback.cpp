#include "Farneback.h"

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string>
#include <deque>
#include <vector>
#include <time.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------
Farneback::Farneback(void)
{
}

//----------------------------------------------------------------------------
Farneback::~Farneback(void)
{
}


//----------------------------------------------------------------------------
//Значение потока
void Farneback::flowValue(int x, int y, float &fx, float &fy)
{
    fy = _Flow.at<Point2f>(y, x).y;
    fx = _Flow.at<Point2f>(y, x).x;
}

//Значение потока
Point2f Farneback::flowValue(int x, int y)
{
    return _Flow.at<Point2f>(y, x);
}

//Значение потока для круглого объекта в экранных координатах
//minLen - короче какой длины не подсчитывать,
//minCount - сколько должно быть сработавших точек, чтоб засчитать не 0
Point2f Farneback::flowValueCircleScreen(int x0, int y0, int rad,
        int screenW, int screenH,
        float minLen, int minCount)
{
    int w = width();
    int h = height();

    x0 = x0 * w / screenW;
    y0 = y0 * h / screenH;
    rad = rad * w / screenW;


    //float useMinLen = threshAbs * sqrt( w * w + h * h );//смасштабировали порог под максимальный размер потока
    float useMinLen = minLen;

    Point2f fRes(0.0, 0.0);
    int count = 0;
    int n = 0;

    for (int y1 = -rad; y1 <= rad; y1++) {
        for (int x1 = -rad; x1 <= rad; x1++) {
            if (x1 * x1 + y1 * y1 <= rad * rad) {
                int x = x0 + x1;
                int y = y0 + y1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    n++;
                    Point2f f = flowValue(x, y);
                    if (f.x * f.x + f.y * f.y >= useMinLen * useMinLen) {
                        count++;
                        fRes += f;
                    }
                }
            }
        }
    }

    float scale = (count >= minCount) ? (1.0 / count) : 0;
    fRes *= scale;

    fRes.x *= 1.0 * screenW / w;
    fRes.y *= 1.0 * screenH / h;

    return fRes;

}

//----------------------------------------------------------------------------
//Значение потока для прямоугольника, в экранных координатах
//minLen - короче какой длины не подсчитывать,
//minCount - сколько должно быть сработавших точек, чтоб засчитать не 0
Point2f Farneback::flowValueRectScreen(int x0, int y0, int w0, int h0,
                                       int screenW, int screenH,
                                       float minLen, int minCount)
{
    int w = width();
    int h = height();

    x0 = x0 * w / screenW;
    y0 = y0 * h / screenH;
    w0 = w0 * w / screenW;
    h0 = h0 * h / screenH;


    //float useMinLen = threshAbs * sqrt( w * w + h * h );//смасштабировали порог под максимальный размер потока
    float useMinLen = minLen;

    Point2f fRes(0.0, 0.0);
    int count = 0;
    int n = 0;

    for (int y1 = y0; y1 < y0 + h0; y1++) {
        for (int x1 = x0; x1 <= x0 + h0; x1++) {
            int x = x1;
            int y = y1;
            if (x >= 0 && x < w && y >= 0 && y < h) {
                n++;
                Point2f f = flowValue(x, y);
                if (f.x * f.x + f.y * f.y >= useMinLen * useMinLen) {
                    count++;
                    fRes += f;
                }
            }
        }
    }

    float scale = (count >= minCount) ? (1.0 / count) : 0;
    fRes *= scale;

    fRes.x *= 1.0 * screenW / w;
    fRes.y *= 1.0 * screenH / h;

    return fRes;
}


//----------------------------------------------------------------------------
void Farneback::setup()	//надо ли находить блобы
{
    _inited = false;
    _r = _g = _b = 128;
    _enablePostProcessing = false;
}

//----------------------------------------------------------------------------
void Farneback::setEnablePostProcessing(bool enable)		//надо ли находить блобы во время update
{
    _enablePostProcessing = enable;
}

//----------------------------------------------------------------------------
void Farneback::update(Mat &grayImg0, Mat &grayImg1)   //IplImage* grayImg0, IplImage* grayImg1 )
{
    //Считываем изображение
    Mat im0 = grayImg0; //( grayImg0, false /*not copy data*/ );			//не выделять память каждый раз
    Mat im1 = grayImg1; //( grayImg1, false );

    //Mat im0 = imread("a.jpg");
    //Mat im1 = imread("b.jpg");


    //cv::GaussianBlur(img, img, cv::Size(7,7), 1.5, 1.5);

    /*if( im0.data == 0 || im1.data == 0) {
    	printf("can't read at least one of image files\n");
        return -2;
    }*/

    //vector<Mat> im0_planes;		//разбиение для случая трехцветной картинки
    //split(im0, im0_planes);
    //vector<Mat> im1_planes;
    //split(im1, im1_planes);


    double pyrScale		=	0.5;
    int levels			=	5; ////3;
    int winsize			=	5; //7; //19; //19;	-- c 19 получается большой ореол вокруг объекта
    int iterations		=	3;
    int polyN			=	7; //9;  //7;
    double polySigma	=	1.5; //2.0; //1.5;
    int flags = OPTFLOW_FARNEBACK_GAUSSIAN;			//OPTFLOW_USE_INITIAL_FLOW;
    //Включение использования начального потока, похоже, приводит к тому, что
    //иногда появляется "постоянная засветка" поля. Поэтому сейчас выключил.
    //if ( _inited ) flags |= OPTFLOW_USE_INITIAL_FLOW;


    clock_t start, finish;
    start = clock();
    calcOpticalFlowFarneback(im0, im1, _Flow, pyrScale, levels, winsize, iterations, polyN, polySigma, flags);
    finish = clock();
    int duration = double(finish - start) / CLOCKS_PER_SEC * 1000.0;

    //printf("duration=%d\n", duration);		//отладочная печать

    _inited = true;

    if (_enablePostProcessing) {
        postProcessing();				//анализ областей, подготовка для вывода на экран
    }


    /*vector<Mat> flow_planes;
    split(Flow, flow_planes);
    pow(flow_planes[0], 2, flow_planes[0]);
    pow(flow_planes[1], 2, flow_planes[1]);
    Mat FlowAbs=flow_planes[0]+flow_planes[1];
    sqrt(FlowAbs, FlowAbs);
    normalize(FlowAbs, FlowAbs, 0, 1, NORM_MINMAX);


    //Размеры изображения
    int h=im0.size().height;
    int w=im0.size().width;
    Mat res(h,w,CV_8UC1);
    res=255;//Устанавливаем белый цвет на всём


    const int step=10;

    for(int x=0; x<w; x++)
    for(int y=0; y<h; y++){
    	float fy=Flow.at<Point2f>(y,x).y;
    	float fx=Flow.at<Point2f>(y,x).x;
    	if (x%step==0 && y%step==0) {
    		line(res, Point(x,y), Point(x+fx, y+fy), 0);
    		circle(res, Point(x+fx, y+fy), 1, 0);
    	}
    }

    namedWindow("FlowAbs", CV_WINDOW_AUTOSIZE);
    imshow("FlowAbs", FlowAbs);
    namedWindow("res", CV_WINDOW_AUTOSIZE);
    imshow("res", res);

    cvWaitKey(0);*/


}

//----------------------------------------------------------------------------
void Farneback::setColor(int r, int g, int b)
{
    _r = r;
    _g = g;
    _b = b;
}

//----------------------------------------------------------------------------
//поиск связной sv-компоненты цвета == search; перекрашивает её в erase
void findComponent(unsigned char *image, int w, int h, int sv, Point start, int search, int erase,
                   vector<Point> &res, int &size)
{
    //deque<Point> res;
    size = 0;

    //res.push_back( start );
    res[ size++ ] = start;

    image[ start.x + w * start.y ] = erase;

    unsigned int begin = 0;
    while (begin < size) {   //res.size()){
        Point p = res[begin];
        //image[ p.x + w * p.y ] = erase;

        begin++;
        for (int y = p.y - 1; y <= p.y + 1; y++) {
            for (int x = p.x - 1; x <= p.x + 1; x++) {
                if (x >= 0 && x < w && y >= 0 && y < h
                        && (sv == 8 || x == p.x || y == p.y)
                        && image[ x + w * y ] == search) {
                    //res.push_back( Point( x, y ) );
                    res[ size++ ] = Point(x, y);
                    image[ x + w * y ] = erase;
                }
            }
        }
    }

}



//поиск областей движения. Портит image - оставляет только точки блобов.
vector<MotionBlob> Farneback::findMotionBlobs(unsigned char *image, int w, int h,
        int minFilter, int maxFilter)
{
    if (_tempPoints.size() < w * h) {
        _tempPoints.resize(w * h);
    }

    vector<MotionBlob> blobs;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (image[ x + w * y ] == 1) {
                int n;
                findComponent(image, w, h, 4, Point(x, y), 1, 0, _tempPoints, n);

                if (n >= minFilter && (maxFilter == 0 || n <= maxFilter)) {
                    //расчет параметров
                    int x0 = x;
                    int y0 = y;
                    int x1 = x;
                    int y1 = y;
                    Point2f f(0, 0);
                    for (int i = 0; i < n; i++) {
                        Point p = _tempPoints[i];
                        x0 = min(x0, p.x);
                        x1 = max(x1, p.x);
                        y0 = min(y0, p.y);
                        y1 = max(y1, p.y);
                        f += flowValue(p.x, p.y);
                    }
                    MotionBlob blob;
                    blob.n = n;
                    blob.fx = f.x / n;
                    blob.fy = f.y / n;
                    blob.w = x1 - x0 + 1;
                    blob.h = y1 - y0 + 1;
                    blob.x = x0 + blob.w / 2;
                    blob.y = y0 + blob.h / 2;

                    blobs.push_back(blob);

                    //рисуем
                    for (int i = 0; i < n; i++) {
                        image[ _tempPoints[i].x + w * _tempPoints[i].y ] = 2;		//эта метка не помешает
                    }

                }
            }
        }
    }

    return blobs;

}

//----------------------------------------------------------------------------
void Farneback::renderOutImage(unsigned char *image)	//нарисовать изображение в outImage
{
    int w = width();
    int h = height();
    if (_outImage.size() != w * h) {
        _outImage.resize(4 * w * h);
    }
    for (int i = 0; i < w * h * 4; i++) {
        _outImage[i] = 0;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int val = image[ x + w * y ];
            //вывод границы
            unsigned char find = 1;
            if (val) {
                for (int y1 = max(y - 1, 0); y1 <= min(y + 1, h - 1); y1++) {
                    for (int x1 = max(x - 1, 0); x1 <= min(x + 1, w - 1); x1++) {
                        if (!(x1 == x && y1 == y)) {
                            find = min(find, image[ x1 + w * y1 ]);
                        }
                    }
                }
            }
            if (val > 0 && find == 0) {
                val = 255; //*= 127;
                int ind = 4 * (x + w * y);
                _outImage[ ind ] = _r;
                _outImage[ ind + 1 ] = _g;
                _outImage[ ind + 2 ] = _b;
                _outImage[ ind + 3 ] = val;
            }
        }
    }
    //запись в текстуру для последующего вывода ее на экран
    //outImageTexture.loadData(outImage, w, h, GL_RGBA);

}

//----------------------------------------------------------------------------

void Farneback::postProcessing()			//анализ поля движения, формирование текстуры, жестов
{
    if (empty()) {
        return;    //надо обязательно проверять, вдруг расчеты не прошли!
    }
    //-----------------------
    const float thresh1 = 4.0;		//порог
    const float thresh2 = 6.0;		//порог
    const int minFilter = 50; //10;		//число точек
    const int maxFilter = 0; //1000;		//число точек

    //-----------------------

    Mat *Flow = flow();
    int w = width();
    int h = height();

    //----------- flowAbs -------------
    if (_flowAbs.size() != w * h) {
        _flowAbs.resize(w * h);
    }
    if (_flowLength.size() != w * h) {
        _flowLength.resize(w * h);
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float fy = Flow->at<Point2f>(y, x).y;
            float fx = Flow->at<Point2f>(y, x).x;
            float a = sqrt(fx * fx + fy * fy);
            _flowLength[ x + w * y ] = a;
            int val = 0;
            if (a >= thresh2) {
                val = 1/*2*/;
            } else {
                if (a >= thresh1) {
                    val = 1;
                }
            }
            _flowAbs[ x + w * y ] = val;
        }
    }


    //анализ блобов ---------------------
    if (_flowAbsBlobs.size() != w * h) {
        _flowAbsBlobs.resize(w * h);
    }
    for (int i = 0; i < w * h; i++) {
        _flowAbsBlobs[ i ] = _flowAbs[ i ];
    }

    _motionBlobs = findMotionBlobs(&_flowAbsBlobs[0], w, h, minFilter, maxFilter);


    /*for (int i=0; i<blobs.size(); i++) {
    	MotionBlob &blob = blobs[ i ];
    	for ( int y = blob.y - blob.h/2; y < blob.y + blob.h/2; y++ ) {	//TODO!!! из-за деления - могут быть проблемы на границе
    		for ( int x = blob.x - blob.w/2; x < blob.x + blob.w/2; x++ ) {
    			_flowAbsBlobs[ x + w * y ] = 1;
    		}
    	}
    }*/

    //outImage --------------------------
    //renderOutImage( &_flowAbs[0] );				//все блобы
    //renderOutImage( &_flowAbsBlobs[0] );		//хорошие блобы


}

//----------------------------------------------------------------------------
vector<MotionBlob> Farneback::motionBlobs(int sw, int sh)   //прямоугольники с движением в экранных координатах
{
    if (empty()) {
        return vector<MotionBlob>();    //надо обязательно проверять, вдруг расчеты не прошли!
    }

    float scaleX = 1.0 * sw / width();
    float scaleY = 1.0 * sh / height();
    int n = _motionBlobs.size();
    vector<MotionBlob> res(n);
    for (int i = 0; i < n; i++) {
        MotionBlob blob = _motionBlobs[ i ];
        blob.fx *= scaleX;
        blob.fy *= scaleY;
        blob.x *= scaleX;
        blob.y *= scaleY;
        blob.w *= scaleX;
        blob.h *= scaleY;
        res.push_back(blob);
    }
    return res;

}

//----------------------------------------------------------------------------
unsigned char *Farneback::processCameraImageRGBA(unsigned char *imageRGB, int w0, int h0)
//обработать картинку с камеры и вернуть указатель на обработанную
{
    if (empty()) {
        return 0;
    }

    int byteIn = 3;
    int byteOut = 4;
    int w = width();
    int h = height();
    if (_cameraImage.size() != byteOut * w * h) {
        _cameraImage.resize(byteOut * w * h);
    }
    unsigned char *res = &_cameraImage[0];
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float alpha;

            //alpha = _flowAbsBlobs[ x + w * y ] * 255;
            //float len = _flowLength[ x + w * y ];
            //const float maxFlow = 2.0;
            //alpha = len / maxFlow;

            alpha = 1.0;
            alpha = min(alpha, 1.0f);


            int index = byteIn * (x + w * y);
            int indexOut = byteOut * (x + w * y);


            int bright = ((int)imageRGB[ index ] + imageRGB[ index + 1 ] + imageRGB[ index + 2 ]) / 3;
            //bright *= alpha;

            res[ indexOut    ] = bright;
            res[ indexOut + 1] = bright;
            res[ indexOut + 2] = bright;
            res[ indexOut + 3] = alpha * 255;

        }
    }



    return res;
}


//----------------------------------------------------------------------------
void Farneback::renderAbsField(IplImage *result)
{
    //Mat canvas( result, false /*not copy data*/ );

    Mat *Flow = flow();
    vector<Mat> flow_planes;
    split(*Flow, flow_planes);
    pow(flow_planes[0], 2, flow_planes[0]);
    pow(flow_planes[1], 2, flow_planes[1]);

    //Mat FlowAbs=flow_planes[0]+flow_planes[1];
    Mat FlowAbs = result;
    //Mat FlowAbs( canvas.rows, canvas.cols, CV_8UC1 );

    FlowAbs = flow_planes[0] + flow_planes[1];

    //sqrt(FlowAbs, FlowAbs);
    //normalize(FlowAbs, FlowAbs, 0, 1, NORM_MINMAX);

    /*vector<Mat> m(3);
    m[0] = FlowAbs;
    m[1] = FlowAbs;
    m[2] = FlowAbs;

    merge(m, canvas);

    canvas.setTo( 0 );
    */
    //convertScaleAbs( FlowAbs, canvas, 1.0, 0.0 );
    //FlowAbs.convertTo( canvas, CV_8UC3, 1, 0 );

    //float thresh = 2.0; //3.0;					//ПАРАМЕТР порог

    //threshold( FlowAbs, FlowAbs, thresh, 255, THRESH_BINARY );




    //namedWindow("FlowAbs", CV_WINDOW_AUTOSIZE);
    //imshow("FlowAbs", FlowAbs);

}


//----------------------------------------------------------------------------
