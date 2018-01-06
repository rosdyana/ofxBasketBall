#ifndef pbOscParamH
#define pbOscParamH

//ѕараметры, управл€емые с OSC

#include "ofMain.h"
#include <map>
#include "ofxOsc.h"

//ƒобавление параметра
#define OSC_ADDPARAM( name, var ) { shared_param.addParam( name, &var ); }

//ѕолучение значени€ параметра
#define OSC_VALUE( name, defValue ) (shared_param.value( name, defValue ))

//ѕолучение значени€ параметра [0,1]
#define OSC_VALUEAMP( name, defValue ) (shared_param.valueAmp( name, defValue ))

//»зменение параметра (обычно с клавиатуры)
#define OSC_VALUE_CHANGE( name, shift ) { shared_param.setValue( name, OSC_VALUEAMP( name, 0 ) + shift ); }

//—в€зь имени переменной и адреса

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


//—лушатель значений переменной
struct pbOscValueListener {
	//oscAlpha=edgesVis						- Simple
	//oscAlpha=edgesVis; peak=0.5,0.2,0.1	- Peak
	void setup( float *var, string params );
	void setupSimple();
	void setupPeak( float center, float rad, float decay );
	void setupMap( float in0, float in1, float out0, float out1 );

	void update( float input );

	float *var;		//переменна€, в которую записать значени€
	int type;				//тип
	static const int TypeSimple = 0;	//просто копирует значение
	static const int TypePeak = 1;		//пик, с заданным центром, шириной и линейным спадом. «начени€ от 0 до 1.
	static const int TypeMap = 2;		//мэппинг [in0, in1] в [out0, out1]
	float peakCenter;	
	float peakRad; 
	float peakDecay;
	float in0, in1, out0, out1;
	

};


//ќдна переменна€

struct pbOscValue {
	pbOscValue() { setup( 0, 0, 1, 0, 1 ); } 
	void setup( float aamp,
				float ainput0, float ainput1,
				float aoutput0, float aoutput1,
				float velUp = -1, float velDown = -1, bool trigger = false, bool impulse = false
				);

	//добавить слушател€ параметра
	void addListener( const pbOscValueListener &listener );

	void update( float dt );

	float input0, input1;
	float output0, output1;
	float amp;
	void setAmp( float amp0 );
	float value();

private:
	bool _velUpEnabled, _velDownEnabled;	//использовать ли скорость
	float _velUp, _velDown;	//максимальна€ скорость роста и спадани€
	bool _trigger;			//если включено, то выходное значение выдаетс€ лишь input0 и input1
	bool _impulse;			//если включено, то посто€нно возвращает в значение по умолчанию, с заданной макс. скоростью
	
	float _targetAmp;

	vector<pbOscValueListener> _listener;	//слушатели
};

//Ќабор переменных
class pbOscParams
{
public:
	pbOscParams(void);
	~pbOscParams(void);

	//ѕлан работы: setup0, затем добавление списка параметров, затем setup2
	void setup0();
	void addParam(const string &key, pbOscValue *value);	//регистраци€ нового параметра
	void setup1( string fileName, const string &section, float w, float h );

	void update( float dt );
	//OSC
	void parseOscMessage( ofxOscMessage &m );

	pbOscValue* find( const string &name );
	float value( const string &name, float defValue = 0.0f );	//возможно, работает не очень быстро
	float valueAmp( const string &name, float defValue = 0.0f );	//[0,1]
	void setValue( const string &name, float value );

	//доступ по номеру, а не по имени - дл€ update и вывода на экран
	vector<pbOscValue*> &getList() { return _list; }
private:
	ParamList _paramList;
	float _w, _h;

	vector<pbOscValue*> _list;
};

extern pbOscParams shared_param;

#endif