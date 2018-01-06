#pragma once

#include <vector>
using namespace std;

//область движения
struct MotionBlob {
    float fx, fy;		//направление
    int x, y, w, h;		//центр и размеры габаритного прямоугольника
    float n;			//число точек, либо вес
    int left()
    {
        return x - w / 2;
    }
    int top()
    {
        return y - h / 2;
    }
    //ofRectangle rect() { return ofRectangle( left(), top(), w, h ); }
};

typedef vector<MotionBlob> MotionBlobs;