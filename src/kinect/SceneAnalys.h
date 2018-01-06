#pragma once

//������ ����� - ���� ������ ����� � �����

#include "cv.h"
#include "ofMain.h"
#include "PersonType.h"

using namespace cv;

class SceneAnalys
{
public:
	SceneAnalys(void);
	~SceneAnalys(void);

	void setup( bool increaseId = false,	//����������� �� id ��� ��������� ������ ��������
				int persistentFrames = 0,	//����� ������, �� ������� �������� ��������� �������
				int recognitionFaceCount = 5,	//����� ������������ ��������� ���, ����� �������� - �������� gender
				int recognitionFrameCount = 60	////����� ������. ���� ��������� � ��� ����� ���� ������������ ���� - �������� gender
				);

	//������ ����� �� ����������. ���� <= 0, �� �� �����������
	void setFilterZ( float filterZ ) { _filterZ = filterZ; }

	void update( int w, int h, unsigned short *depth, unsigned short *labels );

	void applyFaces( const vector<cv::Rect> &faces,
		const vector<string> &genders,
		const Mat &labelsMat		
		);		//��������� � ������� ����� ����������� ������ � �����


	const Persons &persons() const { return _persOut; }

	vector<Person> &persist() { return _persist; } //������ ����� �� id �������

	const Mat labelsMat();			
	const Mat depthMat();

	//��� ������� �������
	string message() { return _persOut.message(); }
private:
	int _w, _h;
	unsigned short *_depth;	//��������� ���������
	unsigned short *_labels;	//��������� ���������
		
	int _frame;
	
	
	Persons _pers;					//��� �������
	Persons _persOut;				//��� ������

	bool _kIncreaseId;			//����������� �� id
	int _kPersistentFrames;		//������� ������ ������� ��������� �����

	void persistenceUpdate();	//���������� � ������ _kIncreaseId � _kPersistentFrames

	vector<Person> _persist;	//�������� ����� � ���������� ������
	int _totalId;				//������������ id

	float _filterZ;				//������ ����� �� ����������. ���� <= 0, �� �� �����������

};

extern SceneAnalys shared_scene;

