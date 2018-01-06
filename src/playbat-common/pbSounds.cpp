#include "pbSounds.h"
#include "pbFiles.h"
#include "MediaLoader.h"

pbSounds sharedSounds;

//----------------------------------------------------
void pbSound::load( ofxIniSettings &ini, const string &path, const string &dir )
{	
	load( dir + ini.get( path + "Sample", string() ), 
		ini.get( path + "Name", string() ),
		ini.get( path + "Volume", 1.0f ), 0, 0 );
}

//----------------------------------------------------
void pbSound::load( const string &fileName, const string &name0, float volume0,
									float fadeIn0, float fadeOut0 )
{
	name	= name0;
	volume  = volume0;	
	fadeInSec = fadeIn0;
	fadeOutSec = fadeOut0;
	fade = ( fadeInSec > 0 || fadeOutSec > 0 );

	_volume.setup( 1.0, 1.0 );
	if ( fadeInSec > 0 ) {
		_volume.setSpeed( 1.0 / fadeInSec );
	}

	if ( fileName != "" ) {
		//sample.loadSound( fileName );
		lazyLoadSound( sample, fileName );
			
		//sample.setVolume( globalVolume * volume );
		if ( fade ) {	//если фейд - то крутить по петле, и много нельзя запускать, так как не отрегулировать громкость
			sample.setLoop( true );
		}
		else {			//если не фейд - то можно много запускать
			sample.setMultiPlay( true );
		}
	}
}

//----------------------------------------------------
void pbSound::play( const pbSoundPlayParam &param )
{
	float targetVolume = volume * param.vol;
	if ( sample.isLoaded ) sample.play();
	if ( fade ) {
		if ( fadeInSec > 0 && targetVolume > 0 ) {
			_volume.setSpeed( targetVolume / fadeInSec );
			_volume.setValue( 0 );
			_volume.setTarget( targetVolume );
		}
	}
	else {
		sample.setMultiPlay( true );
		_volume.setValue( targetVolume );
	}
	sample.setVolume( _volume.value()  );
	sample.setSpeed( param.speed );
}


//----------------------------------------------------
void pbSound::updateParam( const pbSoundPlayParam &param )
{
	_volume.setTarget( volume * param.vol );
	sample.setSpeed( param.speed );
	sample.setPan( param.pan );
}

//----------------------------------------------------
void pbSound::stop()
{
	if ( fade && fadeOutSec > 0 ) {
		_volume.setSpeed( _volume.value() / fadeOutSec );
		_volume.setTarget( 0 );		
	}
	else {
		sample.stop();
	}
}

//----------------------------------------------------
void pbSound::update( float dt )
{
	if ( fade ) {
		_volume.update( dt );
		float vol = _volume.value();
		if ( sample.getIsPlaying() ) {
			sample.setVolume( vol );
			//if ( _volume.value() == 0 ) {
			//	sample.stop();
			//}
		}
		else {
			//if ( vol > 0 ) {
			//	sample.play();
			//}
		}
		
	}
}

//----------------------------------------------------
void pbSounds::append( const string &fileName )	//добавить к списку звуков
{
	ofxIniSettings ini;
	ini.load( fileName );

	string dir = ini.get( "Sounds.contentDir", string() );
	if ( dir != "" ) dir += "/";

	int N = ini.get( "Sounds.N", 0 );
	_sound.resize( N );
	for (int i=0; i<N; i++) {
		_sound[i].load( ini, "Sound" + ofToString( i + 1 ) + ".", dir );
	}
	_n = N;
}

//----------------------------------------------------
void pbSounds::setup( const string &fileName, float globalVolume )
{
	_sound.clear();
	_globalVolume = globalVolume;
	_globalVolume0 = 1.0;
	if ( fileName != "" ) {
		append( fileName );
	}	
}

//----------------------------------------------------
//заменяем все "\t" на пробелы, удаляем двойные
string filterString( string s )
{
	for (int i=0; i<s.size(); i++) {
		if ( s[i] == '\t' ) s[i] = ' ';
	}
	string res = "";
	int i=0; 
	while ( i < s.length() ) {
		if ( s[i] != ' ' || s[i] == ' ' && i > 0 && s[i-1] != ' ' ) {
			res += s[i];
		}
		i++;
	}
	return res;
}

//----------------------------------------------------	
void pbSounds::appendShortIni( const string &fileName, const string &dir )
{
	cout << endl << "Sounds loading..." << endl << "    from: " << fileName << endl;
	vector<string> list = pbFiles::readStrings( fileName );
	//считаем число непустых строк, это будет примерная оценка числа звуков
	int n = 0;
	for (int i=0; i<list.size(); i++) {
		string line = list[i];
		if ( line != "" && line[0] != ';' && line[0] != '%' && line[0] != '/' ) {
			n++;
		}
	}
	//_sound.resize( n );

	int count = 0;

	int index = 0;
	for (int i=0; i<list.size(); i++) {
		string line = filterString( list[i] );
		

		if ( line != "" && line[0] != ';' && line[0] != '%' && line[0] != '/' ) {
			//if ( index == 0 ) {
			//	_globalVolume0 = ofToFloat( line );
			//}
			//else 
			{
				vector<string> v = ofSplitString( line, " " );
				if ( v.size() >= 3 ) {
					string name = v[0];
					string fileName = v[1];
					float volume = ofToFloat( v[2] );
					float fadeIn = ( v.size() >= 4 ) ? ofToFloat( v[3] ) : 0;
					float fadeOut = ( v.size() >= 5 ) ? ofToFloat( v[4] ) : 0;

					//pbSound sound;
					//sound.load( dir + "/" + v[1], v[0], ofToFloat( v[2] ) );
					//_sound.push_back( sound );	//- так - нельзя, звук играть не будет	

					if ( _n < _sound.size() ) {
						_sound[_n++].load( dir + "/" + fileName, name, volume, fadeIn, fadeOut );
						count++;
					}
				}
				else {
					cout << endl << "ERROR Sounds file - bad string: " << line << endl;
				}
			}
			index++;
		}
	}
	//_n = count;
	cout << "    Loaded " << count << " sound(s) from " << dir << endl;	
}

//----------------------------------------------------
void pbSounds::setupShortIni( const string &fileName, const string &dir, float globalVolume )
{
	_sound.clear();
	_sound.resize(100);
	_globalVolume = globalVolume;
	_globalVolume0 = 1.0;

	_n = 0;
	if ( fileName != "" ) {
		appendShortIni( fileName, dir );
	}
}

//----------------------------------------------------
float _pbSoundsTime = 0;
void pbSounds::update()
{
	float time = ofGetElapsedTimef();
	float dt = min( time - _pbSoundsTime, 0.1 );
	_pbSoundsTime = time;

	for (int i=0; i<_n; i++) {
		_sound[i].update( dt );
	}
	ofSoundUpdate();
}

//----------------------------------------------------
void pbSounds::playRandom( const string &prefix, const pbSoundPlayParam &param )
{
	vector<int> list;
	for ( int i=0; i<_sound.size(); i++) {
		string name = _sound[i].name;
		if ( name.length() >= prefix.length() && name.substr( 0, prefix.length() ) == prefix ) {
			list.push_back(i);
		}
	}
	int n = list.size(); 
	if ( n > 0 ) {
		int k = ofRandom( 0, n );
		k %= n;
		play( _sound[list[k]].name );
	}
}

//----------------------------------------------------
void pbSounds::playWithIndex( int i, const pbSoundPlayParam &param0 )
{
	if ( i >= 0 ) {
		pbSoundPlayParam param = param0;
		param.vol *= _globalVolume * _globalVolume0;	//коррекция громкости
		_sound[ i ].play( param );
	}
}

//----------------------------------------------------
void pbSounds::play( const string &name, const pbSoundPlayParam &param )
{
	//if ( param.vol <= 0 ) {
	//	stop( name );
	//}
	//else 
	int i = find( name );
	if ( i >= 0 ) {
		playWithIndex( i, param );
	}
	else {
		cout << "WARNING: No sound with name: " + name << endl;
	}
}

//----------------------------------------------------
void pbSounds::stop( const string &name )
{
	int i = find( name );
	if ( i >= 0 ) {
		_sound[i].stop();
	}
	else {
		cout << "WARNING: No sound with name: " + name << endl;
	}
}

//----------------------------------------------------
void pbSounds::stopAll()
{
	for ( int i=0; i<_sound.size(); i++) {
		_sound[i].sample.stop();
	}

}

//----------------------------------------------------
int pbSounds::find( const string &name )
{
	for ( int i=0; i<_n; i++) {
		if ( _sound[i].name == name ) return i;
	}
	return -1;
}

//----------------------------------------------------
//Каналы. С их помощью удобно управлять фоновыми звуками
//В одном канале играет один звук, и если ставится новый - старый выключается. Если надо , с фейдом
int pbSounds::playChannel( const string &channel, const string &sound, const pbSoundPlayParam &param0,
						   bool continue0 )
{
	int i = findChannel( channel );
	
	if ( i >= 0 ) {
		if ( !continue0 || _channelSound[i] != sound ) {
			stop( _channelSound[i] );
			_channelSound[i] = sound;
			_channelSoundIndex[i] = find( sound );
			playWithIndex( _channelSoundIndex[i], param0 );
		}
		else {
			int i = find( sound );
			if ( i >= 0 ) {
				pbSoundPlayParam param = param0;
				param.vol *= _globalVolume * _globalVolume0;	//коррекция громкости
				_sound[ i ].updateParam( param );
			}			
		}
	}
	else {
		//новый канал
		int index = find( sound );
		if ( index >= 0 ) {
			_channel.push_back( channel );
			_channelSound.push_back( sound );
			_channelSoundIndex.push_back( index );
			playWithIndex( index, param0 );
		}
	}
	return i;
}

//----------------------------------------------------
void pbSounds::updateChannel( const string &channel, const pbSoundPlayParam &param0 )
{
	int k = findChannel( channel );
	if ( k >= 0 ) {
		pbSoundPlayParam param = param0;
		param.vol *= _globalVolume * _globalVolume0;	//коррекция громкости
		_sound[ _channelSoundIndex[k] ].updateParam( param );
	}
}

//----------------------------------------------------
void pbSounds::stopChannel( const string &channel )
{
	int i = findChannel( channel );
	if ( i >= 0 ) {
		stop( _channelSound[ i ] );
		_channel.erase( _channel.begin() + i );
		_channelSound.erase( _channelSound.begin() + i );
		_channelSoundIndex.erase( _channelSoundIndex.begin() + i );
	}
}

//----------------------------------------------------
int pbSounds::findChannel( const string &channel )
{
	for (int i=0; i<_channel.size(); i++) {
		if ( channel == _channel[i] ) {
			return i;
		}
	}
	return -1;
}

//----------------------------------------------------
