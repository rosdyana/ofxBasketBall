#pragma once

#include "ofMain.h"
#include "ofxIniSettings.h"

struct pbTransformInfo {
    pbTransformInfo()
    {
        duration = 1.0;
        rectFrom = ofRectangle(0, 0, 0, 0);
        rectTo = rectFrom;
        userData = -1;
    }
    void load(ofxIniSettings &ini, string path);

    float duration;
    ofRectangle rectFrom;
    ofRectangle rectTo;
    int userData;
    bool enabled()
    {
        return rectFrom.width > 0 && rectFrom.height > 0;
    }

    string background;
    float backgroundSize;
};


class pbTransforming
{
public:
    static const int StatusNo = 0;
    static const int StatusProcess = 1;
    static const int StatusFinished = 2;

    pbTransforming();


    void start(const pbTransformInfo &tr);
    void update();

    int status();
    void resetStatus()
    {
        _status = 0;
    }

    float alpha();
    void pushMatrix();
    void popMatrix();
    int userData();
    string background()
    {
        return _transInfo.background;
    }
    float backgroundSize()
    {
        return _transInfo.backgroundSize;
    }

private:
    int _status;
    float _transTime;
    pbTransformInfo _transInfo;


};
