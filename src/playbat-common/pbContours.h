#pragma once

//Помощник по работе с контурами

#include "ofMain.h"

void drawContour( const vector<ofPoint> &c, int color, int lineWidth, bool closed );

void writeContour( const string &fileName, const vector<ofPoint> &contour );

vector<ofPoint> readContour( const string &fileName );

//Ищет точку контура, рядом с p
int findContourIndex( ofPoint p, const vector<ofPoint> &contour, float rad );

//Габаритный бокс
ofRectangle contourBoundBox( const vector<ofPoint> &c );