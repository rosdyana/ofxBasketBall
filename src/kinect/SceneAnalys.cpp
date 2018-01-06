#include "SceneAnalys.h"
#include "highgui.h"

#include "pbKinector.h"

SceneAnalys shared_scene;

const int kMaxLabels = 50;

int kSceneAnalysN = 50;			//������������ ����� ������������� �����   !!!!!!!!!!!!!!! ��������

//--------------------------------------
SceneAnalys::SceneAnalys(void)
{
}

//--------------------------------------
SceneAnalys::~SceneAnalys(void)
{
}

//--------------------------------------
void SceneAnalys::setup(bool increaseId, int persistentFrames,  int recognitionFaceCount, int recognitionFrameCount)
{
    _kIncreaseId = increaseId;
    _kPersistentFrames = persistentFrames;

    _frame = 0;
    _totalId = 0;

    Person::setup(recognitionFaceCount, recognitionFrameCount);
    cout << "SceneAnalys: recognitionFaceCount=" << recognitionFaceCount << "  recognitionFrameCount=" << recognitionFrameCount << endl;
}

//--------------------------------------
void SceneAnalys::update(int w, int h, unsigned short *depth, unsigned short *labels)
{
    _w = w;
    _h = h;

    _frame++;


    _pers.clear();


    _depth = depth;
    _labels = labels;


    ofPoint point[ kMaxLabels ];
    int n[ kMaxLabels ] = {0};	//������������� ���� ������� - ��������� �������� � 0 ����� C++.

    //�������� ����� �������������
    unsigned short *pDepth = depth;
    unsigned short *pLabel = labels;

    for (int y = 0; y < _h; y++) {
        for (int x = 0; x < _w; x++) {
            unsigned short label = *pLabel;
            unsigned short depth = *pDepth;
            if (label > 0 && depth > 0) {
                label = label % kMaxLabels;
                if (n[ label ] == 0) {	//���� ��� �����
                    point[ label ] = ofPoint(0, 0, 0);
                }

                n[ label ] ++;
                point[ label ] += ofPoint(x, y, depth);
            }
            pDepth++;
            pLabel++;
        }
    }


    //�������� ���������
    float time = ofGetElapsedTimef();

    for (int i = 0; i < kMaxLabels; i++) {
        if (n[i] > 0) {
            point[i] /= n[i];
            ofPoint world = shared_kinect.convertScreenToWorld(point[i]);

            if (_filterZ <= 0 || point[i].z <= _filterZ) {
                _pers.add(Person(i, i, point[i].x / w, point[i].y / h, point[i].z, world.x, world.y, _frame, time));
            }
        }
    }

    //TEST ��������� �� ��������� ����� � ����
    //_pers.clear();
    //for (int i=0; i<kMaxLabels; i++) {
    //	_pers.add( Person( i, i, ofRandom(0,1), ofRandom(0,1), ofRandom(0,5000), ofRandom(-5000,5000), ofRandom(-5000,5000), _frame, time ) );
    //}



    persistenceUpdate();	//��������� _persOut

}

//--------------------------------------
const Mat SceneAnalys::labelsMat()
{
    return cv::Mat(cv::Size(_w, _h), CV_16UC1, _labels);    //.clone();
}

//--------------------------------------
const Mat SceneAnalys::depthMat()
{
    return cv::Mat(cv::Size(_w, _h), CV_16UC1, _depth);    //.clone();
}

//--------------------------------------
void SceneAnalys::persistenceUpdate()
{
    if (!_kIncreaseId && _kPersistentFrames == 0) {
        //������ �� ���� ������
        _persOut = _pers;
    } else {
        //�������� ����������
        if (_persist.size() != kSceneAnalysN) {
            _persist.resize(kSceneAnalysN);
        }

        //��������� � _persist �����
        for (int i = 0; i < _pers.p.size(); i++) {
            const Person &p = _pers.p[i];
            int id = p.id;
            if (id >= 0 && id < kSceneAnalysN) {
                //���� �� �� ���?
                int idTotal = _persist[ id ].id;	//�������� id
                if (idTotal >= 0) {   //��� ���� ����� �������
                    _persist[ id ].updatePositionAndFrame(p);	//��������� ���������
                    //_persist[ id ].id = idTotal; //��������� �������� id � ����������� �����
                } else {								//������ ����� id
                    //����� �������
                    _persist[ id ] = p;
                    cout << "time " << p._timeAppear << endl;
                    if (_kIncreaseId) {			//���� ����� ����������� id
                        _totalId++;
                        _persist[ id ].id = _totalId;
                    }
                }
            } else {
                //TODO ??? �� ������
            }
        }

        //����������
        for (int id = 0; id < kSceneAnalysN; id++) {
            if (!_persist[ id ].empty()) {
                _persist[id].update();	//���������� ����������
                if (_frame > _persist[ id ].frame + _kPersistentFrames) {
                    //���� ������ - �������
                    _persist[ id ].clear();
                }
            }
        }

        //��������� out
        _persOut.clear();
        for (int id = 0; id < kSceneAnalysN; id++) {
            if (!_persist[ id ].empty()) {
                _persOut.add(_persist[ id ]);
            }
        }
    }

}

//--------------------------------------
void SceneAnalys::applyFaces(const vector<cv::Rect> &faces,
                             const vector<string> &genders,
                             const Mat &labelsMat
                            )		//��������� � ������� ����� ����������� ������ � �����
{
    //������������, ��� �� ����� �������� id �� ���������
    //����, � �������� ���� ��������


    //�������� ����� �� �����
    /*{
    	Mat image8;
    	labelsMat.convertTo( image8, CV_8UC3 );
    	Mat t;
    	threshold( image8, t, 0, 255, CV_THRESH_BINARY );

    	//������ ������������� ���
    	for (int i=0; i<faces.size(); i++) {
    		cv::rectangle( t, faces[i], cv::Scalar( 255, 0, 0 ), 2 );
    	}
    	imshow( "labels", t );
    }*/



    vector<int> count(kSceneAnalysN);
    vector<bool> faceDetected(kSceneAnalysN, false);

    for (int k = 0; k < faces.size(); k++) {
        fill(count.begin(), count.end(), 0);
        cv::Rect r = faces[k] & cv::Rect(0, 0, labelsMat.size().width, labelsMat.size().height);
        for (int y = r.y; y < r.y + r.height; y++) {
            for (int x = r.x; x < r.x + r.width; x++) {
                int id = labelsMat.at<unsigned short>(y, x);
                if (id > 0 && id < kSceneAnalysN) {
                    count[ id ]++;
                }
            }
        }
        //������� ��������
        int m = 0;
        int index = -1;
        for (int i = 0; i < kSceneAnalysN; i++) {
            if (!_persist[ i ].empty() && count[i] > m) {
                m = count[i];
                index = i;
            }
        }
        //���������
        if (index >= 0) {
            _persist[ index ].updateFace(true, genders[ k ]);
            faceDetected[ index ] = true;
            //cout << "points=" << r.width * r.height << " detected=" << count[index] << endl;
        }

    }
    //� ���, ������� ���� �� ����������, ���������� ��� ��������
    for (int i = 0; i < kSceneAnalysN; i++) {
        if (!_persist[ i ].empty() && !faceDetected[ i ]) {
            _persist[ i ].updateFace(false, "");
        }
    }




}

//--------------------------------------