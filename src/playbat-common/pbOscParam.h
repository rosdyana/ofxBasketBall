#ifndef pbOscParamH
#define pbOscParamH

//���������, ����������� � OSC

#include "ofMain.h"
#include <map>
#include "ofxOsc.h"

//���������� ���������
#define OSC_ADDPARAM( name, var ) { shared_param.addParam( name, &var ); }

//��������� �������� ���������
#define OSC_VALUE( name, defValue ) (shared_param.value( name, defValue ))

//��������� �������� ��������� [0,1]
#define OSC_VALUEAMP( name, defValue ) (shared_param.valueAmp( name, defValue ))

//��������� ��������� (������ � ����������)
#define OSC_VALUE_CHANGE( name, shift ) { shared_param.setValue( name, OSC_VALUEAMP( name, 0 ) + shift ); }

//����� ����� ���������� � ������

class ParamList
{
public:
	void clear() {
		_values.clear();
	}
	void pushPointer( const string &key, void *value );
	void *findPointer( const string &key );
	vector<string> keys();
	void deleteKey( string key )	{
		_values.erase( key );
	}
private:
	typedef map<string, void *, less<string> > StringPMap;
	StringPMap _values;
};


//��������� �������� ����������
struct pbOscValueListener {
	//oscAlpha=edgesVis						- Simple
	//oscAlpha=edgesVis; peak=0.5,0.2,0.1	- Peak
	void setup( float *var, string params );
	void setupSimple();
	void setupPeak( float center, float rad, float decay );
	void setupMap( float in0, float in1, float out0, float out1 );

	void update( float input );

	float *var;		//����������, � ������� �������� ��������
	int type;				//���
	static const int TypeSimple = 0;	//������ �������� ��������
	static const int TypePeak = 1;		//���, � �������� �������, ������� � �������� ������. �������� �� 0 �� 1.
	static const int TypeMap = 2;		//������� [in0, in1] � [out0, out1]
	float peakCenter;	
	float peakRad; 
	float peakDecay;
	float in0, in1, out0, out1;
	

};


//���� ����������

struct pbOscValue {
	pbOscValue() { setup( 0, 0, 1, 0, 1 ); } 
	void setup( float aamp,
				float ainput0, float ainput1,
				float aoutput0, float aoutput1,
				float velUp = -1, float velDown = -1, bool trigger = false, bool impulse = false
				);

	//�������� ��������� ���������
	void addListener( const pbOscValueListener &listener );

	void update( float dt );

	float input0, input1;
	float output0, output1;
	float amp;
	void setAmp( float amp0 );
	float value();

private:
	bool _velUpEnabled, _velDownEnabled;	//������������ �� ��������
	float _velUp, _velDown;	//������������ �������� ����� � ��������
	bool _trigger;			//���� ��������, �� �������� �������� �������� ���� input0 � input1
	bool _impulse;			//���� ��������, �� ��������� ���������� � �������� �� ���������, � �������� ����. ���������
	
	float _targetAmp;

	vector<pbOscValueListener> _listener;	//���������
};

//����� ����������
class pbOscParams
{
public:
	pbOscParams(void);
	~pbOscParams(void);

	//���� ������: setup0, ����� ���������� ������ ����������, ����� setup2
	void setup0();
	void addParam(const string &key, pbOscValue *value);	//����������� ������ ���������
	void setup1( string fileName, const string &section, float w, float h );

	void update( float dt );
	//OSC
	void parseOscMessage( ofxOscMessage &m );

	pbOscValue* find( const string &name );
	float value( const string &name, float defValue = 0.0f );	//��������, �������� �� ����� ������
	float valueAmp( const string &name, float defValue = 0.0f );	//[0,1]
	void setValue( const string &name, float value );

	//������ �� ������, � �� �� ����� - ��� update � ������ �� �����
	vector<pbOscValue*> &getList() { return _list; }
private:
	ParamList _paramList;
	float _w, _h;

	vector<pbOscValue*> _list;
};

extern pbOscParams shared_param;

#endif