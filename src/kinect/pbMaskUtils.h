#pragma once

//Утилиты для работы с масками

#include "ofMain.h"
#include "cv.h"
using namespace cv;

class pbMaskUtils
{
public:
    //ближайшая точка к 8-битной маске. Если маска пустая - то (-1,-1)
    static ofPoint nearestPointToMask(const Mat &mask, const ofPoint &p,
                                      float pW = -1, float pH = -1, //система координат p
                                      ofRectangle rectSearch = ofRectangle(-1, -1, 0, 0));
};
