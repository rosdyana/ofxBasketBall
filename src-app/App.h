#pragma once

//Рабочее приложение
#include "ofMain.h"
#include "pbBaseApp.h"
#include "ofxVectorMath.h"
#include "ofxOpenCv.h"

class App: public pbBaseApp
{
public:
    int w();
    int h();
    int frameRate();

    void setup();
    void exit();
    void update(float dt);
    void updateCamera(float dt, pbVideoGrabber &grabber, bool debugDraw);
    void drawProduction();
    void drawDebug();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    int countScore();

    ofTrueTypeFont 	score;

private:
    void findMoveFromMask(IplImage *mask, ofPoint p, int rad, ofxVec2f &newPos, bool &moved);
    void applyForces(IplImage *mask, bool debugDraw);

};
