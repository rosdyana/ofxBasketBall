#pragma once

//��������� �����-������ ����������, �� ������������� ������ ��� ��� update()

#include "ofMain.h"
#include <deque>

//������� ���������� �����
vector<string> readDir(string path, bool onlyImages = false);


struct MediaLoaderItem {
    MediaLoaderItem()
    {
        pImage = 0;
        pSample = 0;
    }
    string fileName;
    ofImage *pImage;
    ofSoundPlayer *pSample;
    void execute()
    {
        if (pImage) {
            pImage->loadImage(fileName);
            pImage = 0;
        }
        if (pSample) {
            pSample->loadSound(fileName);
            pSample = 0;
        }
    }
};



class MediaLoader
{
public:
    MediaLoader(void);
    ~MediaLoader(void);
    void setup();
    void update();
    void loadAll();					//�������������� �������� ����� ��������
    void draw(float w, float h);
    bool loading()
    {
        return !_item.empty();    //���� ������� ��������
    }

    void pushImage(ofImage *image, const string &fileName);
    void pushSound(ofSoundPlayer *sample, const string &fileName);
private:
    deque<MediaLoaderItem> _item;


};

extern MediaLoader lazyLoader;

//���������� �������
void lazyLoadImage(ofImage &image, const string &fileName);
void lazyLoadSound(ofSoundPlayer &sample, const string &fileName);

