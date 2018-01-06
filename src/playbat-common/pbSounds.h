#pragma once

//�������� ������

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

	bool fade;				//������� �� ����
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
	//������������� �� ������� ��������:
	void setup( const string &fileName, float globalVolume = 1.0 );
	void append( const string &fileName );	//�������� � ������ ������

	//������� ������������:
	void setupShortIni( const string &fileName,  const string &dir, float globalVolume = 1.0 );
	void appendShortIni( const string &fileName, const string &dir );	//�������� � ������ ������

	//�������� ���� ��� ����!!!
	void update();

	//������ ����� ����, ������������ �� prefix
	void playRandom( const string &prefix, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void play( const string &name, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void playWithIndex( int index, const pbSoundPlayParam &param = pbSoundPlayParam() );
	void stop( const string &name );	
	void stopAll();
	
	vector<pbSound> &sound() { return _sound; }

	//������. � �� ������� ������ ��������� �������� �������
	//� ����� ������ ������ ���� ����, � ���� �������� ����� - ������ �����������. ���� ���� , � ������
	//continue0 - ������ ��� ���������� ������� ����, ���� �� ��� ������, �� �������� ���������
	int playChannel( const string &channel, const string &sound, const pbSoundPlayParam &param = pbSoundPlayParam(),
		bool continue0 = false );
	void updateChannel( const string &channel, const pbSoundPlayParam &param );
		
	void stopChannel( const string &channel );

	//���������� �� ����� ����
	bool soundExists( const string &name ) { return ( find( name ) >= 0 ); }

private:
	vector<pbSound> _sound;			//��� ��� ����� ����� ���� �������� ��������, ��� ����, ������� ����������� _n
	int _n;

	float _globalVolume;			//������ ������������
	float _globalVolume0;			//����������� �� ��������� ini-�����

	int find( const string &name );

	vector<string> _channel;			//������ �������
	vector<string> _channelSound;		//���� � ������
	vector<int>		_channelSoundIndex;	
	int findChannel( const string &channel );
};

extern pbSounds sharedSounds;