#include "highgui.h"
#include "ofxIniSettings.h"

#include "pbDepthPlanar.h"
#include "pbFiles.h"

int kDepthErode = 3;			//����� ����� ������
int kDepthFilterMinCount = 50;	//����������� ����� �������� �����, ���� ��� �� �������������

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
    //���������
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

    //���������� �������, ����������� � �������� ������ �������
    loadParam();

    //����������
    _calibrDepth.setup(_param.w, _param.h, calibrFileName);

    _startTime = -1;			//��� �� ����������
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

    //��������� ������ ��� ��������
    resize(depth16, _depthSmall16, cv::Size(_param.w, _param.h), 0.0, 0.0, CV_INTER_NN);

    const float maxDist = 5000.0;
    //_depthSmall16.convertTo( _depthSmallF, CV_32FC1, -1.0 /  5000.0, 1.0 );
    _depthSmall16.convertTo(_depthSmallF, CV_32FC1, -1.0, maxDist);
    //_depthSmallF = _depthSmall16;


    _calibrDepth.transformImage(_depthSmallF, _depthWarped);
    //imshow( "depth", (1.0 / maxDist) * _depthSmallF );

    //����������� ����
    if (_back.empty() || _learnBackground) {
        float kBackAutoSetSec = 3.0;			//������� ����� ��� ����� ������ �������, ���� ���������� ���
        if (time <= _startTime + kBackAutoSetSec || _learnBackground) {
            _learnBackground = false;
            _back = _depthSmallF.clone();
            recalcBackground();
        }
    }
    //imshow( "back", (1.0 / maxDist) * _back );

    //���������� �����
    _tempDiff = _depthWarped - _backWarped;

    //imshow( "diff", (1.0 / maxDist) * _tempDiff );

    makeMask(_tempDiff, _tempMask, _mask, _param, false);

    unlock();
}

//------------------------------------------------------------
//������������� ��� ��� ����� ����������
void pbDepthPlanar::recalcBackground()
{
    if (!_back.empty()) {
        _calibrDepth.transformImage(_back, _backWarped);
    }
}

//------------------------------------------------------------
const Mat &pbDepthPlanar::mask()						//�������������� �����
{
    //��� �� ���� lock, ��� ��� ������ �� ����������
    return _mask;
}

//------------------------------------------------------------
vector<ofPoint> pbDepthPlanar::getWarpPoints()		//����� ����������
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
    recalcBackground();		//������������� ���
    unlock();
}

//------------------------------------------------------------
void pbDepthPlanar::learnBackground()	//������� ���
{
    lock();
    _learnBackground = true;
    unlock();
}

//------------------------------------------------------------
void pbDepthPlanar::resetCorners()	//�������� ����
{
    lock();
    _calibrDepth.resetPoints();
    recalcBackground();		//������������� ���
    unlock();
}

//------------------------------------------------------------
//�������� ����� � ������� floodFill
//TODO ������� � ����� ���������
//���� minCount = -1, �� ��� �� ������������. ���������� � maxCount.
void pbDepthPlanar::maskDenoiseFloodFill(const Mat &mask, Mat &maskOut, int minCount, int maxCount)
{

    maskOut = mask.clone();
    Mat &img = maskOut;

    //������� �������� �����������
    int w = img.size().width;
    int h = img.size().height;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int value = img.at<uchar>(y, x);
            if (value == 255) {          //���� �������� - 255, �� ��������
                //��������� 200
                cv::Rect rect;        //���� ��������� ���������� �������������
                int count = cv::floodFill(img, cv::Point(x, y), Scalar(200), &rect);
                bool ok = true;
                if (minCount >= 0 && count < minCount) {
                    ok = false;
                }
                if (maxCount >= 0 && count > maxCount) {
                    ok = false;
                }
                if (!ok) {	//���� ������ - ��������
                    floodFill(img, cv::Point(x, y), Scalar(0), &rect);
                }
            }
        }
    }
    threshold(img, img, 1, 255, CV_THRESH_BINARY);	//��� ������� � �������� 200 ��������� ������� � 255

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

    //����������
    //for (int i=0; i < param.depthErode; i++) {
    //	erode( mask, mask, cv::Mat() );		//!! ���� � ������ ����� ��������� ��,
    //������� ����������, � �� �������, � ������� ���������
    //- ���� floodfill � ��� ������ �� ���������
    //}
    if (debugDraw) {
        imshow("mask", tempMask);
    }

    //��������� "������" ��������� - ������� ���� �� ���������� ������
    /*Mat maskf;
    mask.convertTo( maskf, CV_32F, 1.0/255.0 );
    if ( _depthHist.empty() ) {
    _depthHist = maskf.clone();
    }
    else {

    }*/

    //�������� ������ �����
    //TODO !!! ����� ���� �������� �������� ���������� ������, ���� ����� ����� ��������
    maskDenoiseFloodFill(tempMask, maskOut, param.filterMinCount, -1);

}

//-----------------------------------------------------------------
pbDepthPlanarParam pbDepthPlanar::getParams()			//���������
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
//������/���������� �������
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
