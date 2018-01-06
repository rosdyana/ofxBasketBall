#pragma once

//��������� ��������

#include "ofMain.h"
#include "ofxFBOTexture.h"

struct TextureLiquidParam {
    bool enabled;
    bool wireframe;
    int gridX, gridY;
    ofPoint amp, velocity;
    TextureLiquidParam()
    {
        enabled = false;
        gridX = gridY = 2;
    }
    void load(const string &fileName, const string &section);
};


class pbTextureLiquid
{
public:
    void setup(int bufferW, int bufferH, const string &fileName = "", const string &section = "");

    void setParam(const TextureLiquidParam &param);

    void update(float dt);
    void draw(float w = 0, float h = 0);

    //�������� � ���
    void begin();
    void end();

    ofxFBOTexture &buffer()
    {
        return _buffer;
    }
private:
    int _w, _h;		//������ ������
    ofxFBOTexture _buffer;

    int _W, _H;	//�����
    TextureLiquidParam _param;

    vector<ofPoint> _m;			//�������
    vector<float> _p, _t;		//����� ��� ����������

    void paramUpdated();

};
