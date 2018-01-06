#pragma once

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "ofMain.h"
#include "pbKinectMotor.h"

#include "cv.h"
using namespace cv;


//параметры кинекта
struct pbKinectorParam {
    bool depthImage;		//надо ли глубину
    bool rgbImage;			//надо ли rgb
    bool labelImage;		//надо ли трекинг людей
    bool threaded;			//в отдельном ли потоке

    int camera;				//номер камеры, если -1 - не использовать, как и w,h,fps
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
    //ќбертка дл€ многопотоковой версии
    static void Start(const pbKinectorParam &param);	//»ногда сбивает пути, если threaded!
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

    // онвертаци€ экранной точки с глубиной в мировую
    ofPoint convertScreenToWorld(ofPoint screenPoint);
    void convertScreenToWorld(int n, XnPoint3D *screenPoint, XnPoint3D *worldPoint);

    // онвертаци€ мировых в экранные координаты
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



    unsigned char *_depthRGB;	//картинка дл€ рисовани€ глубины

    unsigned char *_image;		//указатель
    unsigned short *_depth;		//указатель
    unsigned short *_labels;	//указатель

    pbKinectMotor _motor;		//мотор


    //запись-воспроизведение
    bool _recording;
    bool _playing;


};

extern pbKinector shared_kinect;
