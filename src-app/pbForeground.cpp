#include "pbForeground.h"
#include <highgui.h>

//#define FOREGROUND_SHOW_WINDOWS

const int kLearnRad0 = 10;		//10;
const int kLearnRad1 = 10;		//10;
const int kLearnRad2 = 10;		//10;

const int kDetectRad0 = 10;		//10;
const int kDetectRad1 = 10;		//10;
const int kDetectRad2 = 10;		//10;



//------------------------------------------------------------------------------
void pbForeground::setup(int w, int h)
{
    _learning = false;
    _learningStart = 0.0;
    _learningDuration = 1.0;

    _w = w;
    _h = h;
    _ch = pbForeground_CHANNELS;

    CvSize sz = cvSize(w, h);
    _yuvImage = cvCreateImage(sz, IPL_DEPTH_8U, 3);
    _drawGrayImage = cvCreateImage(sz, IPL_DEPTH_8U, 3);
    _mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
    cvZero(_mask);

    _codeBook.resize(w * h);
    for (int i = 0; i < w * h; i++) {
        init_codebook(_codeBook[ i ]);
    }

    _drawImageReady = false;
}

//------------------------------------------------------------------------------
void pbForeground::clear()
{
    for (int i = 0; i < _w * _h; i++) {
        free_codebook(_codeBook[ i ]);
    }
}

//------------------------------------------------------------------------------
void pbForeground::startLearn(float learnDelay, float learnDuration, const string &saveToFileName)
{
    clear();

    _learningDuration = learnDuration;
    _learningStart = ofGetElapsedTimef() + learnDelay;
    _learning = true;
    _saveFileName = saveToFileName;
}

//------------------------------------------------------------------------------
void pbForeground::drawCodeBook(IplImage *resultGrayImage)
{
#ifdef FOREGROUND_SHOW_WINDOWS
    //if ( resultGrayImage ) {
    for (int y = 0; y < _h; y++) {
        for (int x = 0; x < _w; x++) {
            int col = 0;
            code_book &c = _codeBook[ x + _w * y ];
            for (int i = 0; i < 3; i++) {
                CV_IMAGE_ELEM(_drawGrayImage, uchar, y, 3 * x + i)
                    = (c.numEntries > i) ? (c.cb[i]->min[0]) : 0;     // + c.cb[0]->max[0] ) / 2;
            }
        }
    }

    namedWindow("pbForeground Background", CV_WINDOW_AUTOSIZE);
    imshow("pbForeground Background", _drawGrayImage);
    //}
#endif
}

//------------------------------------------------------------------------------
void pbForeground::update(IplImage *inputRGBImage, IplImage *resultGrayImage)
{

    cvCvtColor(inputRGBImage, _yuvImage, CV_RGB2YCrCb);  //CV_BGR2YCrCb);

    //TEST
    /*{
    	vector<Mat> planes;
    	split( _yuvImage, planes );
    	imshow( "plane0", planes[0] );
    	imshow( "plane1", planes[1] );
    	imshow( "plane2", planes[2] );
    }*/

    if (learning()) {

        float a = learningProgress(false);
        if (a >= 0 && a <= 1.0) {
            unsigned cbBounds[ pbForeground_CHANNELS ];
            cbBounds[0] = kLearnRad0;
            cbBounds[1] = kLearnRad1;
            cbBounds[2] = kLearnRad2;


            float time = ofGetElapsedTimef();
            for (int y = 0; y < _h; y++) {
                for (int x = 0; x < _w; x++) {
                    update_codebook(
                        &CV_IMAGE_ELEM(_yuvImage, uchar, y, x * _ch),
                        _codeBook[ x + _w * y ],
                        cbBounds,
                        pbForeground_CHANNELS,
                        time
                    );
                }
            }
        }
        if (a > 1.0) {
            _learning = false;
            const float staleDuration = _learningDuration * 0.1; //0.5;
            for (int y = 0; y < _h; y++) {
                for (int x = 0; x < _w; x++) {
                    clear_stale_entries(_codeBook[ x + _w * y ], staleDuration);
                }
            }
            if (_saveFileName != "") {
                save(_saveFileName);
                _saveFileName = "";
            }
        }

        drawCodeBook(resultGrayImage);
    } else {
        int cbMod[ pbForeground_CHANNELS ];
        cbMod[0] = kDetectRad0;
        cbMod[1] = kDetectRad1;
        cbMod[2] = kDetectRad2;

        for (int y = 0; y < _h; y++) {
            for (int x = 0; x < _w; x++) {
                uchar res = background_diff(
                                &CV_IMAGE_ELEM(_yuvImage, uchar, y, x * _ch),
                                _codeBook[ x + _w * y ],
                                pbForeground_CHANNELS,
                                cbMod,
                                cbMod
                            );
                CV_IMAGE_ELEM(_mask, uchar, y, x) = res;
            }
        }

#ifdef FOREGROUND_SHOW_WINDOWS
        namedWindow("pbForeground Objects", CV_WINDOW_AUTOSIZE);
        imshow("pbForeground Objects", _mask);
#endif
    }

    _drawImageReady = false;


    /*vector<Mat> planes;
    split( _yuvImage, planes);
    namedWindow("pbForeground", CV_WINDOW_AUTOSIZE);
    imshow("pbForeground", planes[0]);*/
}

//------------------------------------------------------------------------------
bool pbForeground::learning()
{
    return _learning;
}

//------------------------------------------------------------------------------
float pbForeground::learningProgress(bool truncate)
{
    float t = 0.0;
    if (learning() && _learningDuration > 0.0001) {
        t = (ofGetElapsedTimef() - _learningStart) / _learningDuration;
        if (truncate) {
            t = std::min(t, 1.0f);
            t = std::max(t, 0.0f);
        }
    }
    return t;
}

//------------------------------------------------------------------------------
void pbForeground::draw(float x, float y, float w, float h)
{
    if (learning()) {
        return;
    }
    if (!_mask || _mask->width == 0) {
        return;
    }

    if (!_drawImageReady) {
        _drawImageReady = true;
        if (!_drawImage.bAllocated) {
            _drawImage.allocate(_w, _h);
        }
        if (_drawImage.width != _w || _drawImage.height != _h) {
            _drawImage.resize(_w, _h);
        }
        _drawImage.setFromPixels((unsigned char *) _mask->imageData, _w, _h);

    }
    if (w == 0 && h == 0) {
        _drawImage.draw(x, y);
    } else {
        _drawImage.draw(x, y, w, h);
    }

}

//------------------------------------------------------------------------------
bool pbForeground::load(const string &fileName)
{
    bool result = false;
    FILE *f = fopen(fileName.c_str(), "rb");
    if (f) {
        int w, h, channels, size;
        fread(&w, sizeof(w), 1, f);
        fread(&h, sizeof(h), 1, f);
        fread(&channels, sizeof(channels), 1, f);
        fread(&size, sizeof(size), 1, f);
        if (_w == w && _h == h && channels == pbForeground_CHANNELS) {
            result = true;
            clear();
            for (int k = 0; k < size; k++) {
                codeBook &c = _codeBook[k];
                fread(&c.numEntries, 1, sizeof(c.numEntries), f);
                c.cb = new code_element*[ c.numEntries ];
                for (int i = 0; i < c.numEntries; i++) {
                    c.cb[i] = new code_element;
                    fread(c.cb[i], sizeof(*c.cb[i]), 1, f);
                }
            }
        }
        fclose(f);
    }
    return result;
}

//------------------------------------------------------------------------------
void pbForeground::save(const string &fileName)
{
    FILE *f = fopen(fileName.c_str(), "wb");
    if (f) {
        fwrite(&_w, sizeof(_w), 1, f);
        fwrite(&_h, sizeof(_h), 1, f);
        int channels = pbForeground_CHANNELS;
        fwrite(&channels, sizeof(channels), 1, f);
        int size = _codeBook.size();
        fwrite(&size, sizeof(size), 1, f);
        for (int k = 0; k < size; k++) {
            codeBook &c = _codeBook[k];
            fwrite(&c.numEntries, 1, sizeof(c.numEntries), f);
            for (int i = 0; i < c.numEntries; i++) {
                fwrite(c.cb[i], sizeof(*c.cb[i]), 1, f);
            }
        }
        //code_element **cb;
        //int numEntries;
        //float t; //count every access
        fclose(f);
    }

}

//------------------------------------------------------------------------------
void pbForeground::autoAdjust()
{

}

//------------------------------------------------------------------------------
void pbForeground::autoAdjustDisable()
{

}

//------------------------------------------------------------------------------
