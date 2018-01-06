#include "pbRecorderString.h"
#include "pbFiles.h"

//-----------------------------------------------------------------
pbRecorderString::pbRecorderString(void)
{
	clear();
}

//-----------------------------------------------------------------
pbRecorderString::~pbRecorderString(void)
{
}

//-----------------------------------------------------------------
void pbRecorderString::save( const string &fileName )
{
	vector<string> list;
	list.push_back( "fps " + ofToString( _fps ) );
	for (int i=0; i<_data.size(); i++) {
		list.push_back( _data[i] );
	}
	pbFiles::writeStrings( list, ofToDataPath( fileName ) );
}

//-----------------------------------------------------------------
void pbRecorderString::load( const string &fileName )
{
	clear();
	vector<string> list = pbFiles::readStrings( ofToDataPath( fileName ) );
	if ( list.size() >= 1 ) {
		vector<string> line = ofSplitString( list[0], " " );
		if ( line.size() >= 2 ) {
			if ( line[0] == "fps" ) {
				_fps = ofToFloat( line[1] );
			}
		}
		for (int i=1; i<list.size(); i++) {
			_data.push_back( list[i] );
		}
	}
}

//-----------------------------------------------------------------
void pbRecorderString::clear()
{
	_data.clear();
	_read = 0;
	_fps = 30;
	_readTime = -1;
}

//-----------------------------------------------------------------
void pbRecorderString::push( const string &data )
{
	_data.push_back( data );
}

//-----------------------------------------------------------------
void pbRecorderString::reset()
{
	_read = 0;
}

//-----------------------------------------------------------------
string pbRecorderString::readNext( bool useFPS )
{
	if ( _data.size() > 0 ) {
		if ( useFPS ) {
			//если не прошло достаточно времени, то не прокручиваем
			float time = ofGetElapsedTimef();
			if ( time - _readTime < 1.0 / _fps ) {
				_read--;
			}			
			else {
				_readTime = time;
			}
		}
		return _data[ (_read++) % _data.size() ];
	}
	else {
		return string();
	}
}

//-----------------------------------------------------------------
