#pragma once

//���������� ������
//�������� � ���� �������� ��������� ��� ��������� ��������,
//� ����� �������� ��� ���������� ������������� ����������
//TODO - ������-���������� ini

#include "ofMain.h"
#include "cv.h"
#include "pbScreenFormat.h"
//#include "ofxVectorMath.h"

using namespace cv;
using namespace std;

class pbCameraCalibrate
{
public:
    //����������� ����� - ����� �������� ���

    //���������� ������� ��������������, 4 ����� �� ������� �������
    template< class PointClass >
    static Mat getTransform(vector<PointClass> &curve4, int outW, int outH)
    {

        const int K = 4;
        cv::Point2f src[K];
        cv::Point2f dst[K];

        src[ 0 ] = cv::Point2f(0, 0);
        src[ 1 ] = cv::Point2f(outW, 0);
        src[ 2 ] = cv::Point2f(outW, outH);
        src[ 3 ] = cv::Point2f(0, outH);

        for (int i = 0; i < K; i++) {
            dst[ i ] = cv::Point2f(curve4[ i ].x, curve4[ i ].y);
        }

        return getPerspectiveTransform(dst, src);
    }

    //�������������� �����������
    static void warp(const Mat &in, Mat &out, const Mat &transform, int outW, int outH)
    {
        cv::warpPerspective(in, out, transform, cv::Size(outW, outH), INTER_LINEAR);
    }

    //������ ������������� �����


public:
    //inoutFileName - ���� �� ������ ������, �� ������ ����������� � ������������ �������������
    //outW, outH - ������� �������� ��������, ���� 0 - ������� �� ������� �����
    //�� ���� ������������ - ��� ������ ���� ����������� �����
    void setup(int outW, int outH, const string &inoutFileName = "");

    void resetPoints();						//����� �����, �� ��� ��������
    bool load(const string &fileName);
    bool save(const string &fileName);
    void setAutosave(const string &fileName);	//�������� ����� ��������������. ���� ������ ������ - �� ���������

    void transformImage(const Mat &image, Mat &imageOut);
    void transformPixels(vector<Point2f> &in, vector<Point2f> &out);

    void draw(float x, float y, float w, float h /*, const ofPoint &mousePos*/);

    bool enabled()
    {
        return _enabled;    //���� �������� - �� ������ �������� �������� ����������� � ������ ������
    }
    void setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    //��������� ������
    //4 �����, ����� ����� �� ������� �������
    void setPoints(const vector<ofPoint> &p);
    const vector<ofPoint> &points()
    {
        return _points;
    }

    int width()
    {
        return _outW;
    }
    int height()
    {
        return _outH;
    }

private:
    static const int K = 4;							//����� ����� ��� ����������
    bool _enabled;
    int _outW, _outH;
    vector<ofPoint> _points;

    Mat _transform;		//������� ��������������

    //�������������� ����������
    string _autosaveFileName;

};
