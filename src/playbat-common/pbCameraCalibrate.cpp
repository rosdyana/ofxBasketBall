#include "pbCameraCalibrate.h"
#include "pbFiles.h"

//-----------------------------------------------------------------------
//inoutFileName - если не пустая строка, то оттуда считывается и записывается автоматически
//outW, outH - размеры выходной картинки, если 0 - берутся из первого кадра
//но если автозагрузка - они должны быть установлены сразу
void pbCameraCalibrate::setup(int outW, int outH, const string &inoutFileName)
{
    _enabled = true;

    _outW = outW;
    _outH = outH;

    setAutosave(inoutFileName);
    if (!_autosaveFileName.empty()) {
        load(_autosaveFileName);
    }

}

//-----------------------------------------------------------------------
//сброс точек, на всю картинку
void pbCameraCalibrate::resetPoints()
{
    vector<ofPoint> p(4);
    p[0] = ofPoint(0, 0);
    p[1] = ofPoint(_outW, 0);
    p[2] = ofPoint(_outW, _outH);
    p[3] = ofPoint(0, _outH);
    setPoints(p);
}

//-----------------------------------------------------------------------
bool pbCameraCalibrate::load(const string &fileName)
{
    vector<float> v;

    bool ok = pbFiles::fileExists(fileName);
    if (ok) {
        v = pbFiles::readFloats(fileName);
        ok = (v.size() == 2 * K);
    }
    if (ok) {
        vector<ofPoint> p(K);
        for (int i = 0; i < K; i++) {
            p[i] = ofPoint(v[2 * i], v[2 * i + 1]);
        }
        setPoints(p);
    } else {
        resetPoints();
    }
    return ok;

}

//-----------------------------------------------------------------------
bool pbCameraCalibrate::save(const string &fileName)
{
    bool ok = (_points.size() == K);
    if (ok) {
        vector<float> v(2 * K);
        for (int i = 0; i < K; i++) {
            v[ 2 * i ] = _points[i].x;
            v[ 2 * i + 1 ] = _points[i].y;
        }
        ok = pbFiles::writeFloats(v, fileName);
    }

    return ok;
}

//-----------------------------------------------------------------------
//включить режим автосохранения. Если пустая строка - то выключить
void pbCameraCalibrate::setAutosave(const string &fileName)
{
    _autosaveFileName = fileName;
}

//-----------------------------------------------------------------------
void pbCameraCalibrate::transformImage(const Mat &image, Mat &imageOut)
{
    if (enabled() && _points.size() == K) {
        //Mat test;
        //image.convertTo(test, CV_8UC3, 1.0);

        warpPerspective(image, imageOut, _transform, cv::Size(_outW, _outH), INTER_LINEAR);
    } else {
        resize(image, imageOut, cv::Size(_outW, _outH));
    }
}

//-----------------------------------------------------------------------
void pbCameraCalibrate::transformPixels(vector<Point2f> &in, vector<Point2f> &out)
{
    if (enabled() && _points.size() == K) {
        Mat dst;
        perspectiveTransform(Mat(in), dst, _transform);
        out = Mat_<Point2f>(dst);
    } else {
        out = in;
    }

}


//-----------------------------------------------------------------------
void pbCameraCalibrate::draw(float x0, float y0, float w, float h /*, const ofPoint &mousePos*/)
{
    if (_outW > 0 && _outH > 0) {
        vector<ofPoint> &p = _points;
        for (int pass = 0; pass < 2; pass++) {
            if (pass == 0) {	//линии
                ofSetColor(0, 0, 255);
                ofSetLineWidth(3);
            }
            ofSetLineWidth(1);
            ofFill();
            /*if ( pass == 1 ) {	//круги
            	if ( calibrating() ) {
            		//рисуем зеленые кружки, для ориентира
            		ofSetColor( 0, 255, 0 );
            		int step = p.size();
            		ofPoint v( 0, 0 );
            		if ( step == 1 ) v = ofPoint( w, 0 );
            		if ( step == 2 ) v = ofPoint( w, h );
            		if ( step == 3 ) v = ofPoint( 0, h );
            		ofCircle( v.x, v.y, w / 30 );


            		ofSetColor( 255, 0, 0 );
            	}
            	else {
            		ofSetColor( 0, 0, 255 );
            	}
            }
            */

            if (pass == 1) {
                ofSetColor(255, 0, 255);
            } else {
                ofSetColor(255, 255, 0);
            }
            for (int i = 0; i < p.size(); i++) {
                int j = (i + 1) % p.size();
                float x = x0 + p[i].x * w / _outW;
                float y = y0 + p[i].y * h / _outH;
                float x1 = x0 + p[j].x * w / _outW;
                float y1 = y0 + p[j].y * h / _outH;
                if (pass == 0) {	//линии
                    ofLine(x, y, x1, y1);
                }
                if (pass == 1) {	//круги
                    ofCircle(x, y, w / 80);
                }
            }
        }
    }
    //рисуем положение мыши
    /*if ( calibrating() ) {
    	ofSetColor( 255, 0, 0 );
    	ofSetLineWidth( 3 );
    	ofLine( mousePos.x, 0, mousePos.x, h );
    	ofLine( 0, mousePos.y, w, mousePos.y );
    }*/


}

//-----------------------------------------------------------------------
//4 точки, обход точек по часовой стрелке
void pbCameraCalibrate::setPoints(const vector<ofPoint> &p)
{
    _points = p;

    //const int K = 4;
    cv::Point2f src[K];
    cv::Point2f dst[K];

    src[ 0 ] = cv::Point2f(0, 0);
    src[ 1 ] = cv::Point2f(_outW, 0);
    src[ 2 ] = cv::Point2f(_outW, _outH);
    src[ 3 ] = cv::Point2f(0, _outH);

    for (int i = 0; i < K; i++) {
        dst[ i ] = cv::Point2f(p[ i ].x, p[ i ].y);
    }

    _transform = getPerspectiveTransform(dst, src);

    //автосохранение, если включено
    if (!_autosaveFileName.empty()) {
        save(_autosaveFileName);
    }

}

//-----------------------------------------------------------------------
