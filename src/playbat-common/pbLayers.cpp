#include "pbLayers.h"

pbLayers shared_layers;

//-------------------------------------------------------------------
pbLayers::pbLayers(void)
{
}

//-------------------------------------------------------------------
pbLayers::~pbLayers(void)
{
}

void pbLayers::setup( const string &fileName )
{
	ofxIniSettings ini;
	ini.load( fileName );
	setup( ini );
}

//-------------------------------------------------------------------
void pbLayers::setup( ofxIniSettings &ini )
{
	//собираем имена имеющихся слоев
	vector<string> sections;	
	for (int i=1; i<100; i++) {
		string section = "Layer" + ofToString(i);
		if ( ini.get(section + ".name",string() ) != "" ) {
			sections.push_back( section );
		}
	}
	//загружаем все слои
	int n = sections.size();
	_layer.resize( n );
	for ( int i=0; i<n; i++ ) {
		_layer[i].setup( ini, sections[i], _layer, i );	//передаем список загруженных слоев - чтобы дважды не грузить контент
	}
}

//-------------------------------------------------------------------
void pbLayers::update( float dt )
{
	for ( int i=0; i<_layer.size(); i++) {
		_layer[i].update( dt );
	}
}
//-------------------------------------------------------------------
void pbLayers::draw( float w, float h, int range )
{
	if ( range < 0 ) draw( w, h, -10000, 0 );
	else draw( w, h, 0, 10000 );
}

//-------------------------------------------------------------------
void pbLayers::draw( float w, float h, float z0, float z1 )	//рисует слои из диапазона z0 <= z <= z1 
{
	//перебор по z, от меньшего к большему
	//TODO реализация неоптимальная
	vector<int> index;
	for ( int i=0; i<_layer.size(); i++) {
		float z = _layer[i].z();
		if ( z0 <= z && z < z1 ) index.push_back( i );
	}
	for (int k=0; k<index.size(); k++) {
		//ищем минимальный
		int best = -1;
		float z = 100000;
		for (int i=0; i<index.size(); i++) {
			if ( index[i] >= 0 && _layer[index[i]].z() <= z ) {
				best = i;
				z = _layer[index[i]].z();
			}
		}
		if ( best >= 0 ) {
			_layer[index[best]].draw( w, h );
			index[ best ] = -1;
		}
	}

	//for ( int i=0; i<_layer.size(); i++) {
	//	_layer[i].draw( w, h );
	//}
}

//-------------------------------------------------------------------
void pbLayers::setShift( float shiftX, float shiftY )
{
	for ( int i=0; i<_layer.size(); i++) {
		_layer[i].setShift( shiftX, shiftY );
	}
}

//-------------------------------------------------------------------
pbLayer &pbLayers::layer( const string &name )
{
	int index = findByName( name );
	if ( index >= 0 ) return _layer[ index ];
	cout << "ERROR: layer " << name << " does not exists" << endl;
	return _layer[0];
}

//-------------------------------------------------------------------
int pbLayers::findByName( const string &name )
{
	for ( int i=0; i<_layer.size(); i++) {
		if ( name == _layer[i].name() ) {
			return i;
		}
	}
	return -1;
}

//-------------------------------------------------------------------
