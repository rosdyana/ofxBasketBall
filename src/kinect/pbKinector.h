#pragma once

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "ofMain.h"
#include "pbKinectMotor.h"

#include "cv.h"
using namespace cv;


//��������� �������
struct pbKinectorParam {
    bool depthImage;		//���� �� �������
    bool rgbImage;			//���� �� rgb
    bool labelImage;		//���� �� ������� �����
    bool threaded;			//� ��������� �� ������

    int camera;				//����� ������, ���� -1 - �� ������������, ��� � w,h,fps
    int w;
    int h;
    int fps;

    pbKinectorParam()
    {
        depthImage = true;
        rgbImage = false;
        labelImage = false;
        threaded = false;
        camera = -1;
        w = -1;
        h = -1;
        fps = -1;
    }

};


class pbKinector
{


public:
    //������� ��� �������������� ������
    static void Start(const pbKinectorParam &param);	//������ ������� ����, ���� threaded!
    static void Close();
    static void Update();

public:
    pbKinector(void);
    ~pbKinector(void);

    void setup(const pbKinectorParam &param);
    void update();
    void close();

    const pbKinectorParam &param()
    {
        return _param;
    }

    bool started()
    {
        return _started;
    }
    bool starting()
    {
        return _starting;
    }

    void draw();

    int getWidth();
    int getHeight();

    unsigned char *getImagePixelsConst();
    unsigned char *getDepthPixelsRGBConst();
    unsigned short *getDepthPixelsConst();
    unsigned short *getScenePixelsConst();

    const Mat labelsMat16();
    const Mat depthMat16();
    const Mat colorMat8C3();


    void updateDepthRGB(bool drawLabels);

    pbKinectMotor &motor()
    {
        return _motor;
    }

    void recordStart(const string &fileName);
    void recordStop();
    bool recording()
    {
        return _recording;
    }

    void playerStart(const string &fileName);
    void playerStop();
    bool playing()
    {
        return _playing;
    }

    //����������� �������� ����� � �������� � �������
    ofPoint convertScreenToWorld(ofPoint screenPoint);
    void convertScreenToWorld(int n, XnPoint3D *screenPoint, XnPoint3D *worldPoint);

    //����������� ������� � �������� ����������
    ofPoint convertWorldToScreen(ofPoint worldPoint);
    void convertWorldToScreen(int n, XnPoint3D *worldPoint, XnPoint3D *screenPoint);

private:
    pbKinectorParam _param;

    xn::Context context;
    xn::DepthGenerator g_DepthGenerator;
    xn::ImageGenerator	image_generator;
    xn::SceneAnalyzer g_SceneAnalyzer;
    xn::Recorder *g_pRecorder;

    void setResolution(xn::MapGenerator *pGenerator, int w, int h);
    void setFPS(xn::MapGenerator *pGenerator, int fps);


    bool _started;
    bool _starting;
    int _w, _h;



    unsigned char *_depthRGB;	//�������� ��� ��������� �������

    unsigned char *_image;		//���������
    unsigned short *_depth;		//���������
    unsigned short *_labels;	//���������

    pbKinectMotor _motor;		//�����


    //������-���������������
    bool _recording;
    bool _playing;


};

extern pbKinector shared_kinect;
