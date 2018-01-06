#pragma once

#include <cv.h>
#include "MotionBlob.h"

using namespace std;
using namespace cv;



class Farneback
{
public:
    Farneback(void);
    ~Farneback(void);

    void setup();
    void setEnablePostProcessing(bool enable);		//���� �� �������� ����� �� ����� update

    void update(Mat &grayImg0, Mat &grayImg1);   //IplImage* grayImg0, IplImage* grayImg1 );

    Mat *flow()
    {
        return &_Flow;
    }
    bool empty()
    {
        return (width() == 0 || height() == 0);
    }

    unsigned char *outImageRGBA()
    {
        if (_outImage.size() > 0) {
            return &_outImage[0];
        } else {
            return 0;
        }
    }

    int width()
    {
        return _Flow.cols;
    }
    int height()
    {
        return _Flow.rows;
    }

    void setColor(int r, int g, int b);	//���� ������ �� �����

    vector<MotionBlob> motionBlobs(int sw, int sh);   //�������������� � ��������� � �������� �����������

    unsigned char *processCameraImageRGBA(unsigned char *imageRGB, int w0, int h0);
    //���������� �������� � ������ � ������� ��������� �� ������������

    void flowValue(int x, int y, float &fx, float &fy);	//�������� ������
    Point2f flowValue(int x, int y);

    //�������� ������ ��� �������� ������� � �������� �����������
    //minLen - ������ ����� ����� �� ������������,
    //minCount - ������� ������ ���� ����������� �����, ���� ��������� �� 0
    Point2f flowValueCircleScreen(int x, int y, int rad,
                                  int screenW, int screenH,
                                  float minLen, int minCount);

    //�������� ������ ��� ��������������, � �������� �����������
    //minLen - ������ ����� ����� �� ������������,
    //minCount - ������� ������ ���� ����������� �����, ���� ��������� �� 0
    Point2f flowValueRectScreen(int x0, int y0, int w0, int h0,
                                int screenW, int screenH,
                                float minLen, int minCount);

    //���������� ���������� �������� ����
    void renderAbsField(IplImage *result);	//DEPRECATED

private:
    Mat _Flow;
    bool _inited;
    int _r, _g, _b;		//���� ������ �� �����


    vector<float>		  _flowLength;
    vector<unsigned char> _flowAbs;		//���������� �������� - 0, 1, 2, ������������ ��� ������� ��������
    vector<unsigned char> _flowAbsBlobs;		//���������� �������� ������
    vector<unsigned char> _outImage;		//�������� ��� ������, RGBA

    bool _enablePostProcessing;
    void postProcessing();			//������ ���� ��������, ������������ ��������, ������

    //����� �������� ��������. ������ image - ��������� ������ ����� ������.
    vector<MotionBlob> findMotionBlobs(unsigned char *image, int w, int h,
                                       int minFilter, int maxFilter);

    void renderOutImage(unsigned char *image);	//���������� ����������� � outImage

    vector<cv::Point> _tempPoints;			//��������� ������ ��� �������� ����� ��� ������ ������� ����������

    vector<MotionBlob>	_motionBlobs;		//��������� ������ ������


    vector<unsigned char>_cameraImage;		//������������ �������� � ������

};
