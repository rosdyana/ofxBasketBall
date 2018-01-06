#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

//Фильтрация картинок

struct ImageFilteringSettings {

    bool enabled;
    float alpha;
    float amplifyKoef;

    ImageFilteringSettings()
    {
        enabled = false;
        alpha = 1.0;
        amplifyKoef = 1.0;
    }

};

struct CannySettings {
    double lowThresh;
    double highThresh;
    int aperture;
    CannySettings()
    {
        lowThresh = 100;
        highThresh = 150;
        aperture = 3;
    }
};


class ImageFiltering
{
public:
    void setup(int w, int h, const ImageFilteringSettings &settings);
    void update(const ofxCvColorImage &colorImg);
    ofxCvFloatImage *image()
    {
        return &filteredImage;
    }
    bool enabled()
    {
        return _settings.enabled;
    }
    float alpha()
    {
        return _settings.alpha;
    }

    ofxCvGrayscaleImage *canny(ofxCvGrayscaleImage &gray,
                               const CannySettings &settings,
                               bool updateTexture = true);



private:
    int _w, _h;
    ImageFilteringSettings _settings;
    bool _inited;

    ofxCvFloatImage filteredImage;
    ofxCvFloatImage filteredImage1;

    //Canny
    ofxCvGrayscaleImage gray1;

};
