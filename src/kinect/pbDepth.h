#pragma once

//Работа с картой глубин

#include "ofMain.h"
#include "cv.h"
using namespace cv;

class pbDepth
{
public:
    static void depthToFilePNG(const Mat &depth16, const string &fileName, float minDist = 0.0, float maxDist = 5000.0);
    static void depthToFileArray(const Mat &depth16, const string &fileName, float minDist = 0.0, float maxDist = 5000.0);

    //запись в папку по очереди номеров кадров
    static void saveDepthFrame(const Mat &depth16, const string &dirPNG, const string &dirArray = "",
                               float minDist = 0.0, float maxDist = 5000.0);

    //запись в папку маски
    static void saveMaskFrame(const Mat &mask8, const string &dirPNG);

};
