#pragma once

/*
������� �� ����� ��������
����� �������������� � ���� ������� - ���� ����� �������, � ������� ������ ��������������� ���������� ������,
���� ��������� ������������� - ���� ������� ������������ �������� ����� ��� ���������� ����������� �������� ��������

�������������:

#include "pbMotionHistory.h"
#include "pbMotionArray.h"

pbMotionHistory _history;
pbMotionArray _array;
int _dirLeft = 0;
int _dirRight = 0;


setup:
	float kTimeFilter = 0.1f;			//�������� ������� ������ �����, ��� ������ - ��� ������� �������
	float kThresh = 30.0 / 255.0;		//����� ����������� �������� ������� � �������� �����, �� 0 �� 1
	_history.setup( kTimeFilter, kThresh );


	float kPercent = 0.2;			//�������� ��� ������������
	float kDecay = 0.3;		 		//����� ��������� ������, �� ���������� ���������
	float kUsefulTimeSec = 1.0;		//������� ������� "���������" ���������� ����� ��� �������
	int kChainLength = 0;			//����� ������� ��� _array, 0 - ������, �� ������������

	int w = 21;
	int h = 21;

	_array.setup( cv::Rect_<float>(0.0, 0.0, 1.0, 1.0 ), w, h, kPercent, kDecay, kUsefulTimeSec, kChainLength );

update:
	_history.update( dt );
	_array.update( dt );

	float kDeltaTimeSec = 3.0; //1.0;		//�����, �� ����� �������� ��� ������� ������� ����������
	float kEpsTimeSec = 1.0 / 25.0; //�����, ������ �������� ��� ������� ������� ��������������
	_dirLeft = _array.differencial( -1, -1, -1, 1, kDeltaTimeSec, kEpsTimeSec );
	_dirRight = _array.differencial( 1, 1, -1, 1, kDeltaTimeSec, kEpsTimeSec );

	//���� ����� �������� ����������� ������ ���������
	//(��������, ���� ��������� �������, ���� ����� ����� ����� �� ��������) :
	...
	_array.reset();
	...

updateCamera:
	const Mat &img = grabber.grabFrameCV( procW(), procH() );
	_history.updateCamera( dt, img, debugDraw );
	_array.updateCamera( dt, _history.diffBinary(), debugDraw );

drawDebug:
	_array.draw( 0, 0, _w, _h );
	//���������� ����� _dirLeft, _dirRight

*/

#include "ofMain.h"
#include <cv.h>
#include "pbMotionCell.h"

using namespace std;
using namespace cv;

class pbMotionArray
{
public:
    pbMotionArray(void);
    ~pbMotionArray(void);


    void setup(cv::Rect_<float> relFrame,	//����� ��� �����
               int numW, int numH,			//����� ����� �� ������ � ������
               float cellPercent,			//������� ��������� �������� ������ ���� ��������, ���� ���������
               float cellDecaySec,			//������������ ������ ����� ��������� - ������ �� ����� ���������� ������������

               float usefulTimeSec,	//������� ������ ��������� ������� ����� ���������
               int chainLength				//����� �������, ������ 3 ��� 2. ���� 0 - �� ������� ��
               //������ ������� - ����� ������� ������� �������������
              );

    void updateCamera(float dt,
                      const Mat &imageBinary,			//8-������, �������� 0 � 255
                      bool debugDraw);


    void update(float dt);
    void draw(float x, float y, float w, float h);

    const vector< cv::Rect_<float> > &chains()
    {
        return _chain;    //������ ���������� �������
    }

    //�������������
    int differencial(int dirXMin, int dirXMax,	//������ �����������, � ����� ������� ������������
                     int dirYMin, int dirYMax,
                     float deltaTimeSec,	//�����, �� ����� �������� ��� ������� ������� ����������
                     float epsTimeSec	//�����, ������ �������� ��� ������� ������� ��������������
                    );

    void reset();		//����� ���� ���������� �����


    //��������� ���������� ������
    int w()
    {
        return _w;
    }
    int h()
    {
        return _h;
    }
    const pbMotionCell &cell(int x, int y)
    {
        return _cell[ x + _w * y ];
    }

    float usefulTimeSec()
    {
        return _kUsefulTimeSec;
    }


private:
    float _kUsefulTimeSec;	//��������, ����� ������� ��������� ��������� �������� ������������ �����
    int _kChainLength;

    vector<pbMotionCell> _cell;
    int _n, _w, _h;

    bool checkChain(const vector<int> &ind);	//��������� �������
    vector< cv::Rect_<float> > _chain;			//��������� �������

    cv::Rect_<float> resultChain(int i, int j);	//���������� �������������� �������
};
