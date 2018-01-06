#include "pbValueAnimation.h"

pbValueAnimation::pbValueAnimation()
{
    setup(1.0 / 0.5, 0.0);
}

void pbValueAnimation::setup(float maxSpeed, float value)
{
    _maxSpeed = maxSpeed;
    _value = value;
    _target = value;
    _delayedTargetEnabled = false;
}

void pbValueAnimation::update(float dt)
{
    float delta = _target - _value;
    float maxSpeed = _maxSpeed * dt;
    if (fabs(delta) > maxSpeed) {
        delta = (delta > 0) ? maxSpeed : -maxSpeed;
    }
    _value += delta;

    if (_delayedTargetEnabled && ofGetElapsedTimef() >= _delayedTargetTime) {
        _delayedTargetEnabled = false;
        _target = _delayedTarget;
    }
}

void pbValueAnimation::setValue(float value)
{
    _value = value;
    setTarget(value);
}


void pbValueAnimation::setTarget(float target)
{
    _target = target;
    _delayedTargetEnabled = false;
}

float pbValueAnimation::value() const
{
    return _value;
}

float pbValueAnimation::target() const
{
    return _target;
}

bool pbValueAnimation::delayedTargetEnabled() const
{
    return _delayedTargetEnabled;
}

float pbValueAnimation::delayedTarget() const
{
    return _delayedTarget;
}

float pbValueAnimation::delayedTargetTime() const
{
    return _delayedTargetTime;
}

void pbValueAnimation::setDelayedTarget(float target, float delaySec)
{
    _delayedTargetEnabled = true;
    _delayedTarget = target;
    _delayedTargetTime = ofGetElapsedTimef() + delaySec;

}

