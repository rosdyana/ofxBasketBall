#pragma once

//Загрузчик медиа-данных отложенный, по необходимости далает это при update()

#include "ofMain.h"
#include <deque>

//считать содержимое папки
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
    void loadAll();					//принудительная загрузка всего контента
    void draw(float w, float h);
    bool loading()
    {
        return !_item.empty();    //идет процесс загрузки
    }

    void pushImage(ofImage *image, const string &fileName);
    void pushSound(ofSoundPlayer *sample, const string &fileName);
private:
    deque<MediaLoaderItem> _item;


};

extern MediaLoader lazyLoader;

//Глобальные функции
void lazyLoadImage(ofImage &image, const string &fileName);
void lazyLoadSound(ofSoundPlayer &sample, const string &fileName);

