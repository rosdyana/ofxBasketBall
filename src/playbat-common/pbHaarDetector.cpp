#include "pbHaarDetector.h"
#include "highgui.h"


//-----------------------------------------------------------------------
void pbHaarDetector::setup(const string &cascadeFileName,
                           const string &internalId,
                           float minSizeRel
                          )
{
    if (!_cascade.load(cascadeFileName)) {
        cout << "Haar Detector: can not load cascade " << cascadeFileName << endl;
        OF_EXIT_APP(-1);
    } else {
        cout << "Haar Detector: loaded " << cascadeFileName << endl;
    }

    _internalId = internalId;
    kMinSizeRel = minSizeRel;
}


//-----------------------------------------------------------------------
vector<cv::Rect> pbHaarDetector::find(const Mat &img, bool debugDraw)
{
    vector<cv::Rect> result;

    int w = img.rows;
    int h = img.cols;

    //const float kMinSizeRel = 0.10;
    //const float kMaxSizeRel = 0.40;
    int kMinSizeAbs = kMinSizeRel * w; //50; //30;
    //int kMaxSizeAbs = kMaxSizeRel * w;
    const cv::Size kMinSize(kMinSizeAbs, kMinSizeAbs);


    const float kRescale = 1.1;
    const int kMinNeighbours = 3;
    const int kFlags = 0
                       //|CV_HAAR_FIND_BIGGEST_OBJECT
                       //|CV_HAAR_DO_ROUGH_SEARCH
                       | CV_HAAR_SCALE_IMAGE;


    cvtColor(img, _gray, CV_BGR2GRAY);


    vector<cv::Rect> rects;
    _cascade.detectMultiScale(_gray,
                              rects,
                              kRescale,
                              kMinNeighbours,
                              kFlags,
                              kMinSize
                             );

    for (vector<cv::Rect>::const_iterator r = rects.begin(); r != rects.end(); r++) {

        cv::Rect rect = *r;

        int size = cv::max(rect.width, rect.height);

        //if ( size >= kMinSizeAbs && size <= kMaxSizeAbs ) {
        result.push_back(rect);

        //cout << "detected" << endl;
        //}
    }

    if (debugDraw) {
        _debugDrawImg = img.clone();
        for (int i = 0; i < result.size(); i++) {
            cv::rectangle(_debugDrawImg, result[i], cv::Scalar(0, 0, 255), 3);
        }
        imshow(_internalId, _debugDrawImg);


    }

    return result;
}

//-----------------------------------------------------------------------
