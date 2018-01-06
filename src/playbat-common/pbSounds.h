#pragma once

//«вуковой движок

#include "ofMain.h"
#include "ofxIniSettings.h"
#include "pbValueAnimation.h"

struct pbSoundPlayParam {
	float speed;
	float vol;	
	float pan;
	pbSoundPlayParam( float vol0 = 1.0, float speed0 = 1.0, float pan0 = 1.0 ) { 
		vol = vol0;
		speed = speed0;		
		pan = pan0;
	}
};

struct pbSound {
	string name;
	float volume;

	bool fade;				//включен ли фейд
	float fadeInSec;
	float fadeOutSec;
	pbValueAnimation _volume;	


	ofSoundPlayer sample;
	void load( ofxIniSettings &ini, const string &path, const string &dir );
	void load( const string &fileName, const string &name, float volume, float fadeIn, float fadeOut );

	void play( const pbSoundPlayParam &param );
	void updateParam( const pbSoundPlayParam &param );
	void stop();
	void update( float dt );


};





class pbSounds
{
public:
	//совместимость со старыми верси€ми:
	void setup( const string &fileName, float globalVolume = 1.0 );
	void append( const string &fileName );	//добавить к списку звуков

	//следует использовать:
	void setupShortIni( const string &fileName,  const string &dir, float globalVolume = 1.0 );
	void appendShortIni( const string &fileName, const string &dir );	//добавить к списку звуков

	//¬ызывать чтоб шел звук!!!
	void update();

	//играет любой звук, начинающийс€ на prefix
	void playRandom( const string &prefix, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void play( const string &name, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void playWithIndex( int index, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void stop( const string &name );	
	void stopAll();
	
	vector<pbSound> &sound() { return _sound; }

	// аналы. — их помощью удобно управл€ть фоновыми звуками
	//¬ одном канале играет один звук, и если ставитс€ новый - старый выключаетс€. ≈сли надо , с фейдом
	//continue0 - значит что продолжить текущий звук, если он уже играет, но изменить громкость
	int playChannel( const string &channel, const string &sound, const pbSoundPlayParam &param = pbSoundPlayParam(),
		bool continue0 = false );
	void updateChannel( const string &channel, const pbSoundPlayParam &param );
		
	void stopChannel( const string &channel );

	//существует ли такой звук
	bool soundExists( const string &name ) { return ( find( name ) >= 0 ); }

private:
	vector<pbSound> _sound;			//дл€ них места может быть выделено побольше, чем надо, поэтому используйте _n
	int _n;

	float _globalVolume;			//задает пользователь
	float _globalVolume0;			//считываютс€ из короткого ini-файла

	int find( const string &name );

	vector<string> _channel;			//список каналов
	vector<string> _channelSound;		//звук в канале
	vector<int>		_channelSoundIndex;	
	int findChannel( const string &channel );
};

extern pbSounds sharedSounds;