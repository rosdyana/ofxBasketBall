#pragma once

#include "ofMain.h"
#include <string>
#include <vector>

using namespace std;

//--------------------------------------------------------------
extern int kRecognitionFaceCount;		//число срабатываний детектора лиц, когда превысит - ставится gender
extern int kRecognitionFrameCount;	//число кадров. Если превысили и ест ьхоть одно срабатывание лица - ставится gender

//описание одного человека
struct Person {
public:
	//Настройка параметров распознавания пола по нескольким кадрам
	static void setup( int recognitionFaceCount, int recognitionFrameCount )
	{
		kRecognitionFaceCount = recognitionFaceCount;	//5
		kRecognitionFrameCount = recognitionFrameCount; //60
	}


public:
	int id;			//>0 - постоянно увеличивается
	int idLabel;	//метка на карте labels
	float x, y;		//[0,1]x[0,1]
	float xm, ym;	//метры
	float distance;	//метры
	string gender;	//m, f, или пустая строка
	int age;
	bool faceDetected;
	//ofRectangle faceRect;
	int frame;					//на каком кадре было обнаружено, для persistence

	//bool isNear;				//ставится, что человек близко ли  //ВАЖНО эта переменная - hack!

	int _countGenderValue;		//+1 - муж, -1 - жен - голосование
	int _countGenderFaceN;			//число распознанных лиц
	int _countGenderFrameN;			//число кадров

	float _timeAppear;			//время появления человека
	//ofPoint _fixPosition;		//специальное положение
	float _fixDistance;			//максимальное расстояние до выхода
	bool _fixPositionValid;		//специальное положение
	bool _extWorked;				//уже отработал

	bool empty() const {
		return ( id == -1 || frame == -1 );
	}
	void clear() {
		id = -1;	
		idLabel = -1;
		frame = -1;
		_timeAppear = -1;
		_fixPositionValid = false;
		_extWorked = false;
	}

	Person() {
		clear();
	}

	Person( int aid, int aidLabel, float ax, float ay, float adistance, float axm, float aym, int aframe, float atime ) {
		id = aid; 
		idLabel = aidLabel;
		x = ax; y = ay; distance = adistance; 
		xm = axm;
		ym = aym;
		gender = ""; 
		age = 0;
		faceDetected = false;
		
		frame = aframe;
		_countGenderValue = 0;
		_countGenderFaceN = 0;
		_countGenderFrameN = 0;

		//isNear = false;
		_timeAppear = atime;
		_fixPositionValid = false;
		_extWorked = false;
	}

	//обновить только положение человека
	void updatePositionAndFrame( const Person &p )
	{
		x = p.x;
		y = p.y;
		distance = p.distance;
		xm = p.xm;
		ym = p.ym;
		frame = p.frame;
	}

	//обновить лицо и пол
	void updateFace( bool faceDetected0, const string &gender0 )
	{
		faceDetected = faceDetected0;
		if ( faceDetected ) {
			if ( gender == "" ) {
				_countGenderFaceN++;
				if ( gender0 == "m" ) _countGenderValue++;
				if ( gender0 == "f" ) _countGenderValue--;
			}
		}
	}

	//регулярное обновление
	void update()	
	{
		if ( gender == "" ) {
			//проверяем, как дела с распознаванием пола
			if ( _countGenderFaceN > 0 ) {	//если уже видели лицо, то начать счетчик кадров
				_countGenderFrameN++;
			}
			if ( _countGenderFaceN >= kRecognitionFaceCount
				|| _countGenderFrameN >= kRecognitionFrameCount ) {
				//готовы зафиксировать пол
				if ( _countGenderValue > 0 ) gender = "m";
				if ( _countGenderValue < 0 ) gender = "f";
			}

		}
		//cout << "id " << id << "   frames " << _countGenderFrameN 
		//	<< "  faces " << _countGenderFaceN 
		//	<< "   value " << _countGenderValue
		//	<< "   gender " << gender << endl;
		
	}

	string str() const;
};

//несколько человек
class Persons {
public:
	vector<Person> p;
	string str() const {
		string s = "<people>";
		for (int i=0; i<p.size(); i++) {
			s += p[i].str();
		}
		s += "</people>";
		return s;
	}
	string message() const { 
		return str();
	}

	void clear() { p.clear(); }
	void add( const Person &ap ) { p.push_back( ap ); }
};


