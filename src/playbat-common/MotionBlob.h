#pragma once

#include <vector>
using namespace std;

//������� ��������
struct MotionBlob {
    float fx, fy;		//�����������
    int x, y, w, h;		//����� � ������� ����������� ��������������
    float n;			//����� �����, ���� ���
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