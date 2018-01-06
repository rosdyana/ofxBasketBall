#include "pbKinectMotor.h"
#include "CLNUIDevice.h"


//-------------------------------------------------------------------
pbKinectMotor::pbKinectMotor(void)
{
    _started = false;
    _motor = 0;
    _posInited = false;
    _pos = 0.5;
}

//-------------------------------------------------------------------
pbKinectMotor::~pbKinectMotor(void)
{

}

//-------------------------------------------------------------------
void pbKinectMotor::setup()
{
    int n = GetNUIDeviceCount();
    if (n == 0) {
        cout << "ERROR: No Kinect Motor" << endl;
        _started = false;
    } else {
        PCHAR serial = GetNUIDeviceSerial(0);
        _motor = CreateNUIMotor(serial);
        _started = (_motor != 0);
    }
}

//-------------------------------------------------------------------
void pbKinectMotor::close()
{
    if (_started) {
        _started = false;
        DestroyNUIMotor(_motor);
        _motor = 0;
    }

}

//-------------------------------------------------------------------
void pbKinectMotor::setPosition(float pos)
{
    if (_started) {
        pos = min(pos, 1.0);
        pos = max(pos, 0.0);
        SHORT motorMin = -8000;
        SHORT motorMax = 8000;
        SHORT val = motorMin + pos * (motorMax - motorMin);

        SetNUIMotorPosition(_motor, val);
    } else {
        cout << "ERROR: Can not set motor position" << endl;
    }



}

//-------------------------------------------------------------------
void pbKinectMotor::setLed(int led)
{
    if (_started) {
        SetNUIMotorLED(_motor, led);
    }
}

//-------------------------------------------------------------------
void pbKinectMotor::tiltUp()
{
    changePos(0.05);
}

//-------------------------------------------------------------------
void pbKinectMotor::tiltDown()
{
    changePos(-0.05);
}

//-------------------------------------------------------------------
void pbKinectMotor::changePos(float delta)
{
    if (!_posInited) {
        _pos = 0.5;
        _posInited = true;
    }
    _pos += delta;
    _pos = min(_pos, 1.0);
    _pos = max(_pos, 0.0);
    setPosition(_pos);

}

//-------------------------------------------------------------------
