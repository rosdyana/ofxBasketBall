#pragma once

//�������� �� ������ � ���������

#include "ofMain.h"

void drawContour( const vector<ofPoint> &c, int color, int lineWidth, bool closed );

void writeContour( const string &fileName, const vector<ofPoint> &contour );

vector<ofPoint> readContour( const string &fileName );

//���� ����� �������, ����� � p
int findContourIndex( ofPoint p, const vector<ofPoint> &contour, float rad );

//���������� ����
ofRectangle contourBoundBox( const vector<ofPoint> &c );