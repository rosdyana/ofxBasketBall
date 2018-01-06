#include "pbValueAnimation2d.h"

//--------------------------------------------------------------------------
pbValueAnimation2d::pbValueAnimation2d(void)
{
}

//--------------------------------------------------------------------------
pbValueAnimation2d::~pbValueAnimation2d(void)
{
}

//--------------------------------------------------------------------------
void pbValueAnimation2d::setup( ofxVec2f value, float maxVelocity, float maxAccel, float stopRad )
{
	setValue( value );
	_kMaxVel = maxVelocity;
	_kMaxAcc = maxAccel;
	_kStopRad = stopRad;

	_rect.width = -1;

}

//--------------------------------------------------------------------------
//���������� ������������� ������. ���� w<0 - ��������
void pbValueAnimation2d::setBoundRect( float x, float y, float w, float h )	
{
	_rect.x = x;
	_rect.y = y;
	_rect.width = w;
	_rect.height = h;
}

//--------------------------------------------------------------------------
void pbValueAnimation2d::update( float dt )
{
	
	//���������
	ofxVec2f delta = _target - _value;
	ofxVec2f acc = delta.normalized() * _kMaxAcc; //limited( _kMaxAcc );
	
	//��������
	const float kMass = 1.0;

	float maxVel = _kMaxVel;
	//���� � ������� ����������, �� �������� �������� �������
	{
		float dist = _target.distance( _value );
		if ( dist < _kStopRad ) {
			float a = dist / _kStopRad;
			maxVel *= a;//a*a; //a;
		}
	}

	ofxVec2f vel = _vel;
	vel += acc * kMass * dt;
	vel.limit( maxVel );
	_vel = vel;

	//���������
	ofxVec2f pos = _value;
	
	pos += _vel * dt;

	//�������� ������
	if ( _rect.width >= 0 ) {
		pos.x = ofClamp( pos.x, _rect.x, _rect.x + _rect.width );
		pos.y = ofClamp( pos.y, _rect.y, _rect.y + _rect.height );;
	}
	_value = pos;


}

//--------------------------------------------------------------------------
void pbValueAnimation2d::setValue( ofxVec2f value )							//������� ������������
{
	_value = value;
	_vel = ofxVec2f( 0, 0 );	
	setTarget( value );
}

//--------------------------------------------------------------------------
void pbValueAnimation2d::setTarget( ofxVec2f target )							//������ ������������
{
	_target = target;
	//�������� ������
	if ( _rect.width >= 0 ) {
		_target.x = ofClamp( _target.x, _rect.x, _rect.x + _rect.width );
		_target.y = ofClamp( _target.y, _rect.y, _rect.y + _rect.height );
	}
}

//--------------------------------------------------------------------------

