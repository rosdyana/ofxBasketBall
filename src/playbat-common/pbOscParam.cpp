#include "pbOscParam.h"
#include "ofxIniSettings.h"
#include "converter.h"
#include "parsing.h"

pbOscParams shared_param;

//------------------------------------------------------
vector<string> ParamList::keys()
{
	vector<string> _keys;
	for (StringPMap::const_iterator iter = _values.begin();
		iter != _values.end(); iter++) {
		string key = iter->first;
		_keys.push_back( key );
	}
	return _keys;
}

//------------------------------------------------------
void ParamList::pushPointer( const string &key, void *value )
{
	_values[key] = value;
}
	
//------------------------------------------------------
void *ParamList::findPointer( const string &key )
{
	StringPMap::iterator p = _values.find( key );
		if (p != _values.end()) {
			return p->second;
		}
		else {
			return 0;
		}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//oscAlpha=edgesVis						- Simple
//oscAlpha=edgesVis; peak=0.5,0.2,0.1	- Peak
//oscAlpha=edgesVis; map=0,1,20,30		- Map

void pbOscValueListener::setup( float *var0, string params )
{
	var = var0;
	setupSimple();
	vector<string> list = Parsing::splitFiltered( params, "=" );
	if ( list.size() >= 2 ) {
		string type = list[0];
		vector<float>param = Parsing::splitToFloats( list[1], "," );
		if ( type == "peak" && param.size() >= 3 ) {
			float center = param[0];
			float rad = param[1];
			float decay = param[2];
			setupPeak( center, rad, decay );
		}
		if ( type == "map" && param.size() >= 4 ) {
			setupMap( param[0], param[1], param[2], param[3] );
		}
	}
}

void pbOscValueListener::setupSimple()
{
	type = TypeSimple;
}

void pbOscValueListener::setupPeak( float center, float rad, float decay )
{
	type = TypePeak;
	peakCenter = center;
	peakRad = rad;
	peakDecay = decay;
}

void pbOscValueListener::setupMap( float in0_, float in1_, float out0_, float out1_ )
{
	type = TypeMap;
	in0 = in0_;
	in1 = in1_;
	out0 = out0_;
	out1 = out1_;
}

//-------------------------------------------------------------
void pbOscValueListener::update( float input )
{
	//cout << input << endl;
	if ( var ) {
		if (type == TypeSimple) {
			*var = input;
			return;
		}
		if ( type == TypePeak ) {
			float dist = fabs( input - peakCenter );
			if ( peakDecay > 0.0001 ) {		//мягкий спуск
				*var = ofMap( dist, peakRad, peakRad + peakDecay, 1.0, 0.0, true );
			}
			else {		//резкий
				*var = ( dist <= peakRad ) ? 1.0 : 0.0;
			}
			return;
		}
		if ( type == TypeMap ) {
			*var = ofMap( input, in0, in1, out0, out1, true );
		}
	}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
void pbOscValue::setup( float aamp,
				float ainput0, float ainput1,
				float aoutput0, float aoutput1,
				float velUp, float velDown, bool trigger, bool impulse ) 
{ 
	amp = aamp; 
	_targetAmp = aamp;

	input0 = ainput0; 
	input1 = ainput1; 
	output0 = aoutput0; 
	output1 = aoutput1;

	_velUpEnabled = ( velUp >= 0 );
	_velDownEnabled = ( velDown >= 0 );
	_velUp = velUp;
	_velDown = velDown;
	_trigger = trigger;
	_impulse = impulse;


}

//-------------------------------------------------------------
void pbOscValue::addListener( const pbOscValueListener &listener )
{
	_listener.push_back( listener );
}

//-------------------------------------------------------------
void pbOscValue::update( float dt )
{
	//обновляем значение
	if ( amp < _targetAmp && _velUpEnabled ) {
		amp = min( _targetAmp, amp + _velUp * dt );
	}
	if ( amp > _targetAmp && _velDownEnabled ) {
		amp = max( _targetAmp, amp - _velDown * dt );
	}
	//устанавливаем его слушателям
	if ( _listener.size() > 0 ) {
		float val = value();
		for (int i=0; i<_listener.size(); i++) {
			_listener[i].update( val );
		}
	}

}

//-------------------------------------------------------------
void pbOscValue::setAmp( float amp0 ) 
{
	//amp = ofClamp( amp0, input0, input1 );
	float val = ofClamp( amp0, input0, input1 );
	if ( !_impulse ) {
		_targetAmp = val;
		if ( amp < _targetAmp && !_velUpEnabled ) { amp = _targetAmp; }
		if ( amp > _targetAmp && !_velDownEnabled ) { amp = _targetAmp; }
	}
	else {
		amp = val;
	}

}

//-------------------------------------------------------------
float pbOscValue:: value() 
{
	float val = ofMap( amp, input0, input1, output0, output1, true );
	if ( _trigger ) {	val = ( val > 0 ) ? 1 : 0; }
	return val;
}
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
pbOscParams::pbOscParams(void)
{
}

//-------------------------------------------------------------
pbOscParams::~pbOscParams(void)
{
}

//-------------------------------------------------------------
pbOscValue* pbOscParams::find( const string &name )
{
	void *p = _paramList.findPointer( name );
	return (pbOscValue *)p;
}


//-------------------------------------------------------------
float pbOscParams::value( const string &name, float defValue )	//возможно, работает не очень быстро
{
	pbOscValue *v = find( name );
	return ( v ) ? v->value() : defValue;
}

//-------------------------------------------------------------
float pbOscParams::valueAmp( const string &name, float defValue )	//[0,1]
{
	pbOscValue *v = find( name );
	return ( v ) ? v->amp : defValue;
}

//-------------------------------------------------------------
void pbOscParams::setValue( const string &name, float value )
{
	pbOscValue *v = find( name );
	if ( v ) {
		v->setAmp( value );
	}
}
	
//-------------------------------------------------------------
string eraseSymbol( string str, char symb ) 
{
	str.erase (std::remove (str.begin(), str.end(), symb), str.end());
	return str;
}


//-------------------------------------------------------------
void pbOscParams::setup0()
{
	_paramList.clear();
}

//-------------------------------------------------------------
void pbOscParams::addParam(const string &key, pbOscValue *value)	//регистрация нового параметра
{
	_paramList.pushPointer( key, value );
	_list.push_back( value );
}

//-------------------------------------------------------------
//Хранилище автоматически созданных параметров
vector<pbOscValue> _oscParams;

//-------------------------------------------------------------
void pbOscParams::setup1( string fileName, const string &section, float w, float h  )
{
	//
	_w = w;
	_h = h;

	ofxIniSettings ini;
	ini.load( ofToDataPath( fileName ) );

	//Добавляем автоматически создаваемые параметры, можно до 100 строк вида autocreate1, ..., autocreate100
	{
		string Line = "";
		for (int i=0; i<100; i++) {	
			string number = (i == 0 ) ? "" : ofToString( i );
			string line = ini.get( section + "." + "autoCreate" + number, string() );
			if ( Line != "" ) {
				Line += ";";
			}
			Line += line;
		}
		vector<string> list = Parsing::splitFiltered( Line, ";" );
		int N = list.size();
		if ( N > 0 ) {
			_oscParams.resize( N );
			for (int i=0; i<N; i++) {
				if ( list[i] != "" ) {
					addParam( list[i], &_oscParams[i] );
				}
			}
		}		
	}


	//read ini	
	float input0 = ini.get( section + "." + "INPUT_MIN", 0.0f );
	float input1 = ini.get( section + "." + "INPUT_MAX", 1.0f );

	cout << "Params: " << endl;
	vector<string> keys = _paramList.keys();
	for (int i=0; i<keys.size(); i++) {
		string name = keys[i];
		pbOscValue *v = find( name );
		if ( v ) {
			string str = ini.get( section + "." + name, string("[0,1],0.0") );
			//remove " "
			//str = eraseSymbol( str, ' ' );

			//w[0.01,1],0.03;trigger;timeUp=0.2;timeDown=0.1
			vector<string> LINE = Parsing::splitFiltered( str, ";" );

			//анализируем параметры с ";"
			bool trigger = false;
			bool impulse = false;
			float velUp = -1;
			float velDown = -1;
			str = ( LINE.size() > 0 ) ? LINE[0] : "";	//строка на последующую обработку
			for (int i=1; i<LINE.size(); i++) {
				vector<string>pair = ofSplitString( LINE[i], "=" );
				if ( pair.size() >= 1 ) {
					string name = pair[0];
					trigger = trigger || (name == "trigger");
					impulse = impulse || (name == "impulse");
					if ( pair.size() >= 2 ) {
						float value = ofToFloat( pair[1] );
						float invert = ( value > 0.001 ) ? (1.0 / value) : -1;
						if ( name == "timeUp" || name == "time" ) velUp = invert;
						if ( name == "timeDown" || name == "time" ) { 
							velDown = invert;
						}
					}
				}
			}




			//w[0.01,1],0.03
			vector<string> line1 = ofSplitString( str, "]" );
			//w[0.01,1]     ,0.03
			if ( line1.size() == 2 ) {
				line1[1] = eraseSymbol( line1[1], ',' );
				float amp = ofToFloat( line1[1] );
				vector<string> line2 = ofSplitString( line1[0], "," );
				//w[0.01	1]
				if ( line2.size() == 2 ) {
					line2[1] = eraseSymbol( line2[1], ']' );
					float output1 = ofToFloat( line2[1] );
					vector<string> line3 = ofSplitString( line2[0], "[" );
					//w		0.01    or    0.01
					if ( line3.size() >= 1 ) {
						float preAmp = 1.0;
						float output0 = 0;
						if ( line3.size() == 1 ) output0 = ofToFloat( line3[0] );
						else {
							output0 = ofToFloat( line3[1] );
							string s = line3[0];
							if ( s == "w" ) preAmp = _w;
							if ( s == "h" ) preAmp = _h;
						}
						output0 *= preAmp;
						output1 *= preAmp;
						v->setup( amp, input0, input1, output0, output1, velUp, velDown, trigger, impulse );
						cout << "\t" << name 
							<< "\t" << amp << "\t(" << v->value() << ")" 
							<< "\t[" << input0 << ", " << input1 << "]"
							<< " -> [" << output0 << ", " << output1 << "]";
						if ( velUp >= 0 ) cout << " {velUp=" << velUp << "}";
						if ( velDown >= 0 ) cout << " {velDown=" << velDown << "}";
						if ( trigger ) cout << " {trigger}";
						if ( impulse ) cout << " {impulse}";
						cout << endl;  
					}
				}
			}
		}
	}
	cout << endl;

}

//-------------------------------------------------------------
void pbOscParams::update( float dt )
{
	//for (int i=0; i<_paramList.size(); i++) {
		//_paramList[i].
	//}
	vector<pbOscValue*> list = getList();
	for ( int i=0; i<list.size(); i++ ) {
		list[i]->update( dt );
	}

}


//-------------------------------------------------------------
void pbOscParams::parseOscMessage( ofxOscMessage &m )	//OSC
{
	string paramName = m.getAddress();
	paramName = Converter::filterString( paramName );		//выбрасываем пробелы
	paramName = paramName.substr( 1 );			//отрезаем первый символ

	pbOscValue *param = find( paramName );
	if ( param ) {
		float value = m.getArgAsFloat( 0 );
		param->setAmp( value );
	}
}

//-------------------------------------------------------------
