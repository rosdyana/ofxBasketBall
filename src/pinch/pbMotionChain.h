#pragma once

//������� �� MotionCell,
//��������� ������������� ������������ ��������
//��� ����� �������� ������ ��������������� ���������� �� ���� ��������� �������

#include "ofMain.h"
#include <cv.h>
#include "pbMotionCell.h"

using namespace std;
using namespace cv;

class pbMotionChain
{
public:
    pbMotionChain(void);
    ~pbMotionChain(void);

    void setup(const vector< cv::Rect_<float> > &relRects,	//������ ���������������-�����
               float cellPercent,			//������� ��������� �������� ������ ���� ��������, ���� ���������
               float cellDecaySec,			//������������ ������ ����� ��������� - ������ �� ����� ���������� ������������
               float usefulTimeSec		//������� ������ ��������� ������� ����� ���������
              );

    void updateCamera(float dt,
                      const Mat &imageBinary,			//8-������, �������� 0 � 255
                      bool debugDraw);


    void update(float dt);
    void draw(float x, float y, float w, float h);

    bool isOn()
    {
        return _isOn;    //������� ����������
    }

private:
    float _kUsefulTimeSec;	//��������, ����� ������� ��������� ��������� �������� ������������ �����

    vector<pbMotionCell> _cell;
    int _n;

    bool _isOn;

};
