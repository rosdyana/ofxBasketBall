#pragma once


#include "ofMain.h"

class pbValueAnimation
{
public:
    pbValueAnimation();
    void setup(float maxSpeed = 1.0, float value = 0.0);
    void update(float dt);
    void setValue(float value);
    void setTarget(float target);
    void setDelayedTarget(float target, float delaySec);

    float value() const;
    float target() const;
    bool delayedTargetEnabled() const;
    float delayedTarget() const;
    float delayedTargetTime() const;

    float speed() const
    {
        return _maxSpeed;
    }
    void setSpeed(float speed)
    {
        _maxSpeed = speed;
    }

private:
    float _value;
    float _target;
    float _maxSpeed;

    bool _delayedTargetEnabled;
    float _delayedTarget;
    float _delayedTargetTime;
};
