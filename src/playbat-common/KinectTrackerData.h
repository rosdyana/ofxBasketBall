#pragma once

//������ � ����� � �����

#include "ofMain.h"
#include "ofxIniSettings.h"


//����� ����
static const int kKinectTorso = 0;
static const int kKinectBoundBox0 = 1;
static const int kKinectBoundBox1 = 2;
static const int kKinectHead = 3;
static const int kKinectNeck = 4;
static const int kKinectLShoulder = 5;
static const int kKinectRShoulder = 6;
static const int kKinectLElbow = 7;
static const int kKinectRElbow = 8;
static const int kKinectLHand = 9;
static const int kKinectRHand = 10;
static const int kKinectLHip = 11;
static const int kKinectRHip = 12;
static const int kKinectLKnee = 13;
static const int kKinectRKnee = 14;
static const int kKinectLFoot = 15;
static const int kKinectRFoot = 16;
static const int kKinectPoints = 17;

int kinectNameToInt( const string &name );
string kinectIntToName( int index );
ofColor kinectPartColor( int index );
int kinectIsBodyPart( int index );

//��������� ��������
const int KinectPersonEmpty = 0;
const int KinectPersonAppear = 1;
const int KinectPersonTracking = 2;
const int KinectPersonDissapear = 3;

//������������ ����� �����
const int KinectMaxPersons = 15;

struct KinectTrackerDataItem;
struct KinectTrackerDataPerson;

//���������� ������
struct KinectTrackerCalibrator {
	//xml-����
	void setup( const string &fileName, float w, float h );
	
	//�������������� ���������� � �������� ����������, � ������ ������ � �������� ������
	void convertMmToPixel( KinectTrackerDataItem &item );
	void convertMmToPixel( KinectTrackerDataPerson &person );
	
	//�������������� ���������� � ��������� ����������
	void convertMmToMm( const KinectTrackerDataItem &item, KinectTrackerDataItem &itemOut );

	bool enabled() { return _enabled; }

	//�������� ������ ��� �����
	void drawTopSideMm( const vector<KinectTrackerDataPerson> &persons, int topSide );	

	//�������� �������, �������������
	void drawFrontPixels( const vector<KinectTrackerDataPerson> &persons );	


	void shiftReset( int topSide );	//�������� ������ ����� ��� ������
	void shiftMove( const ofPoint &shiftMm, const ofPoint &rearShiftMm );	//����������

	void skewReset();				//����� ������
	void skewMove( float shiftSkewZ );	//��������� ������

private:
	string _fileName;
	bool _enabled;
	float _w, _h;			//�������� ����������

	ofPoint _screenSizeMm;	//������ ������
	ofPoint _roomSizeMm;	//������ ������� ������� (������ ����� � �������
	ofPoint _shiftMm;		//����� ��������� � ������
	ofPoint _rearShiftMm;	//����� ������� ���������

	float _skewZ;			//����� ����-��� �� Z
	
	ofPoint _scale;			//������� �� -> �������

	void load();
	void save();
};


//����� ����
struct KinectTrackerDataItem {
	//bool enabled;		//������������ �� ��� ������
	bool valid;			//���������� �� ������
	ofPoint pixelRaw;	//�������� ����������, ���������������
	ofPoint pixel;		//�������� ����������
	ofPoint mm;			//�������� ��

	KinectTrackerDataItem() { /*enabled = false;*/ valid = false; }
	void update( bool valid0, const ofPoint &pixel0, const ofPoint &mm0 ){
		valid = valid0; pixelRaw = pixel0; pixel = pixel0; mm = mm0;
	}
};

//���������
struct KinectTrackerDataParam { 
	float receiverWaitNoTracker;	//������� ����� ������� � ������� ������ ��� ��������� ��������
	void load( ofxIniSettings &ini );
};

//���� �������
struct KinectTrackerDataPerson {
	KinectTrackerDataPerson();
	void setup( float w, float h, const KinectTrackerDataParam &param );
	void updateReceiver( float dt );							//���� ����� �� ���� - ��������
	void updateCameraReceiver( bool signal );			//������ ���� ��� ������ ������ � ������

	string toString( bool pixels, bool mm, float wPix, float hPix,
		const vector<int> &enabledParts );							//��� �������� �� ����

	bool empty() { return state==KinectPersonEmpty; }
	void setState( int state0 );

	void draw( float w, float h );			//�������� ��� �������, � ��������
	void boundBoxMm( ofPoint &origin, ofPoint &size );		//���������� ���������� ������������� � ������

	int id;
	int state;		//KinectPersonEmpty, KinectPersonAppear, KinectPersonTracking, KinectPersonDissapear
	vector<KinectTrackerDataItem> item;

	void drawSegmentTopSideMm( int index0, int index1, int topSide );	//�� ������ ����
	void drawSegmentFrontPixel( int index0, int index1 );		//�� ������ ����

private:
	KinectTrackerDataParam _param;
	float _w, _h;
	float _time;				//����� ���������� ������� ������, ������������� 

	void drawSegment( float w, float h, int index0, int index1 );	//�� ������ ����

};


//���� � �����
struct KinectTrackerData {
	KinectTrackerData();
	void setup( float w, float h, const string &fileName, const string &calibrFileName,
		int autobots = 0 );
	void updateReceiver( float dt );						//����� ������ ��������� ������
	void updateFromString( const string &s );
	string toString( bool pixels, bool mm, float wPix, float hPix );		//��� �������� �� ����
	void clear();


	void draw( float w, float h );			//�������� ��� �������, � ��������
	void drawFrontPixels();					//�������� ��� �������, �������������, � ��������
	void drawTopMm();	//�������� ��� ������, ��
	void drawSideMm();	//�������� ��� �����, ��

	vector<KinectTrackerDataPerson> &person() { return _person; }
	vector<int> &enabledParts() { return _enabledParts; }		//����� ����� ���� ������������

	KinectTrackerCalibrator &calibrator() { return _calibrator; }
private:
	KinectTrackerDataParam _param;
	float _w, _h;

	vector<KinectTrackerDataPerson> _person;
	int _N;

	vector<int> _enabledParts;

	KinectTrackerCalibrator _calibrator;
};
