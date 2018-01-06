#pragma once

//������� ��� ������ � �������

#include "ofMain.h"
#include "cv.h"
using namespace cv;

class pbMaskUtils
{
public:
    //��������� ����� � 8-������ �����. ���� ����� ������ - �� (-1,-1)
    static ofPoint nearestPointToMask(const Mat &mask, const ofPoint &p,
                                      float pW = -1, float pH = -1, //������� ��������� p
                                      ofRectangle rectSearch = ofRectangle(-1, -1, 0, 0));
};
