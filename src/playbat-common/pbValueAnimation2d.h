#pragma once

//������� �������� 2�-�����

#include "ofMain.h"
#include "ofxVectorMath.h"

class pbValueAnimation2d
{
public:
    pbValueAnimation2d(void);
    ~pbValueAnimation2d(void);

    void setup(ofxVec2f value, float maxVelocity, float maxAccel, float stopRad);
    void setBoundRect(float x, float y, float w, float h);	//���������� ������������ ������. ���� w<0 - ��������


    void update(float dt);
    ofxVec2f value() const
    {
        return _value;
    }
    ofxVec2f target() const
    {
        return _target;
    }

    void setValue(ofxVec2f value);							//������� ������������
    void setTarget(ofxVec2f target);							//������ ������������

private:
    ofxVec2f _target, _value;
    ofxVec2f _vel;		//��������
    float _kMaxVel;
    float _kMaxAcc;
    float _kStopRad;

    ofRectangle _rect;


};
