#pragma once

#include "ofMain.h"
#include "cv.h"
#include "pbCLEye.h"

using namespace cv;
using namespace std;


class pbVideoGrabber
{
public:
    void setup();
    int cameraCount();

    void setCallback(pbVideoGrabberCallbackType callback, void *param);
    bool startCamera(int device,
                     int w, int h,
                     int fps,
                     bool mirrorX
                    );

    bool stopCamera();

    //Grab
    bool isFrameNew();
    Mat &grabFrameCV();


    Mat &grabFrameCVUnique(int procW, int procH);

    Mat grabFrameCVCopy(int procW, int procH);



    Mat &grabDepthFrameCVFloat();
    Mat &grabDepthFrameCV8bit();


    //Camera params
    bool setParamsAllAuto(bool enable);
    bool setParamGain(int value);
    bool setParamExposure(int value);

private:
    int _w, _h;
    bool _useUSB;
    bool _mirrorX;

    ofVideoGrabber 		vidGrabber;
    Mat _frame;
    Mat _resized;
    Mat _cropped;

    pbCLEye				psGrabber;

    pbVideoGrabberCallbackType _callback;
    void *_callbackParam;

};
