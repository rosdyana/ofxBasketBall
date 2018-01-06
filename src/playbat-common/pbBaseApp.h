#pragma once


#include "baseAppSettings.h"

#include "ofMain.h"
#include "cv.h"
#include "pbScreenFormat.h"
#include "pbVideoGrabber.h"
#include "Farneback.h"


using namespace cv;
using namespace std;

class pbBaseApp
{
public:
    void setup0();
    void exit0();
    void update0();
    void draw0();
    void keyPressed0(int key);
    void mousePressed0(int x, int y, int button);
    void mouseMoved0(int x, int y);
    void mouseReleased0(int x, int y, int button);
    void webserverCommand0(const string &command, string &out);


    const Mat &opticalFlow(const Mat &img);


#ifdef baseAppUseGrabber
    pbVideoGrabber	&grabber()
    {
        return _grabber;
    }
    void cameraSaveFrame(bool saveRGB, bool saveDepth);
#endif

    virtual int w()
    {
        return 800;
    }
    virtual int h()
    {
        return 600;
    }
    virtual int frameRate()
    {
        return 60;
    }
    virtual bool cameraGrabberEnabled()
    {
        return true;
    }

    virtual void setup();
    virtual void exit();
    virtual void update(float dt);

#ifdef baseAppUseGrabber
    virtual void updateCamera(float dt, pbVideoGrabber &grabber, bool debugDraw);
#endif
    virtual void drawProduction();
    virtual void drawDebug();
    virtual void keyPressed(int key);
    virtual void mousePressed(int x, int y, int button, bool mouseShow);
    virtual void mouseMoved(int x, int y, bool mouseShow);
    virtual void mouseReleased(int x, int y, bool mouseShow);

    virtual void webserverCommand(const string &command, string &out);



    bool debugModeDrawCamera()
    {
        return _debugModeDrawCamera == 1;
    };
protected:

    int _drawMode;
    int _debugModeAdditional;
    int _debugModeDrawCamera;

    bool _cameraSaveMode;
    bool _cameraSaveModeDepth;
    int _camFrame;

protected:
    int grabW()
    {
        return _grabW;
    }
    int grabH()
    {
        return _grabH;
    }
    int procW()
    {
        return _procW;
    }
    int procH()
    {
        return _procH;
    }
    int flowW()
    {
        return _flowW;
    }
    int flowH()
    {
        return _flowH;
    }

private:

    float _lastTimeGrab;
    float _lastTimeUpdate;


#ifdef baseAppUseGrabber
    pbVideoGrabber			_grabber;
#endif
    int _grabW, _grabH;
    int _procW, _procH;
    int _flowW, _flowH;

    Mat _imgFlowColor, _imgFlow, _imgFlowLast;
    bool _flowInited;
    Farneback _flow;


};
