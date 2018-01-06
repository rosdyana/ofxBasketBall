#pragma once

#include "ofMain.h"
#include <string>
#include <vector>

using namespace std;

//--------------------------------------------------------------
extern int kRecognitionFaceCount;		//����� ������������ ��������� ���, ����� �������� - �������� gender
extern int kRecognitionFrameCount;	//����� ������. ���� ��������� � ��� ����� ���� ������������ ���� - �������� gender

//�������� ������ ��������
struct Person {
public:
	//��������� ���������� ������������� ���� �� ���������� ������
	static void setup( int recognitionFaceCount, int recognitionFrameCount )
	{
		kRecognitionFaceCount = recognitionFaceCount;	//5
		kRecognitionFrameCount = recognitionFrameCount; //60
	}


public:
	int id;			//>0 - ��������� �������������
	int idLabel;	//����� �� ����� labels
	float x, y;		//[0,1]x[0,1]
	float xm, ym;	//�����
	float distance;	//�����
	string gender;	//m, f, ��� ������ ������
	int age;
	bool faceDetected;
	//ofRectangle faceRect;
	int frame;					//�� ����� ����� ���� ����������, ��� persistence

	//bool isNear;				//��������, ��� ������� ������ ��  //����� ��� ���������� - hack!

	int _countGenderValue;		//+1 - ���, -1 - ��� - �����������
	int _countGenderFaceN;			//����� ������������ ���
	int _countGenderFrameN;			//����� ������

	float _timeAppear;			//����� ��������� ��������
	//ofPoint _fixPosition;		//����������� ���������
	float _fixDistance;			//������������ ���������� �� ������
	bool _fixPositionValid;		//����������� ���������
	bool _extWorked;				//��� ���������

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

	//�������� ������ ��������� ��������
	void updatePositionAndFrame( const Person &p )
	{
		x = p.x;
		y = p.y;
		distance = p.distance;
		xm = p.xm;
		ym = p.ym;
		frame = p.frame;
	}

	//�������� ���� � ���
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

	//���������� ����������
	void update()	
	{
		if ( gender == "" ) {
			//���������, ��� ���� � �������������� ����
			if ( _countGenderFaceN > 0 ) {	//���� ��� ������ ����, �� ������ ������� ������
				_countGenderFrameN++;
			}
			if ( _countGenderFaceN >= kRecognitionFaceCount
				|| _countGenderFrameN >= kRecognitionFrameCount ) {
				//������ ������������� ���
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

//��������� �������
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


