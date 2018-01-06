#pragma once

#include "ofMain.h"
#include "ofxThread.h"
#include "pbEvent.h"
#include "cv.h"
#include "pbKinector.h"


class pbKinectThread: public ofxThread
{
public:
    //--------------------------
    pbKinectThread()
    {
        _verbose = false;

    }

    void start(const pbKinectorParam &param, bool verbose)
    {
        _param = param;
        _verbose = verbose;

        startThread(true, false);     // blocking, verbose
    }

    void stop()
    {
        stopThread();

        //ждем останова
        while (shared_kinect.started()) {
            ofSleepMillis(50);
        }
    }


    void getState()
    {

    }

    void update()
    {

    }
    //--------------------------
    void threadedFunction()
    {

        //устанавливаем приоритет потока
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

        shared_kinect.setup(_param);

        while (isThreadRunning() != 0) {
            shared_kinect.update();
            shared_kinect.updateDepthRGB(true);
            //ofSleepMillis( 20 );
        }
        shared_kinect.close();
    }

    //--------------------------
    void draw()
    {
        //string str = "Position: ";

        //pbFaceDetectorData state = getState();
        //str += ofToString( state.x ) + ", " + ofToString( state.y );
        //ofDrawBitmapString(str, 50, 56);
    }
private:
    bool _verbose;
    pbKinectorParam _param;


};


extern pbKinectThread shared_kinectThread;