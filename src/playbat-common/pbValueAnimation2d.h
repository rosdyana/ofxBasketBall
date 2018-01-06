#pragma once

//Гладкое движение 2д-точки

#include "ofMain.h"
#include "ofxVectorMath.h"

class pbValueAnimation2d
{
public:
    pbValueAnimation2d(void);
    ~pbValueAnimation2d(void);

    void setup(ofxVec2f value, float maxVelocity, float maxAccel, float stopRad);
    void setBoundRect(float x, float y, float w, float h);	//установить прямоугльник границ. Если w<0 - выключен


    void update(float dt);
    ofxVec2f value() const
    {
        return _value;
    }
    ofxVec2f target() const
    {
        return _target;
    }

    void setValue(ofxVec2f value);							//жесткое переключение
    void setTarget(ofxVec2f target);							//мягкое переключение

private:
    ofxVec2f _target, _value;
    ofxVec2f _vel;		//скорость
    float _kMaxVel;
    float _kMaxAcc;
    float _kStopRad;

    ofRectangle _rect;


};
