#pragma once

#include "ofMain.h"

#include "cv.h"
#include "highgui.h"
using namespace cv;

class pbScreenGrab
{
public:
    pbScreenGrab(void);
    ~pbScreenGrab(void);

    void setup(int w, int h);
    void close();
    void grab(int x, int y);
    void getMat(Mat &img);

private:
    int _w, _h;
    vector<unsigned char> _data;
    Mat _img;

};
