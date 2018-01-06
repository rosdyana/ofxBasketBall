#pragma once

#include "ofMain.h"

//������� ����
ofColor makeColor( int r = 255, int g = 255, int b = 255, int a = 255 );

void ofSetOfColor( const ofColor &color );

//��������� �������
vector<ofColor> loadPalette( const string &imageFileName );