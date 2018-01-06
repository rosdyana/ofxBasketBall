#include "pbPinchEuler.h"

#include "highgui.h"
#include "ofxVectorMath.h"

//-----------------------------------------------------------------------
void pbPinchLinePoint::pushBuffer()	//�������� ������� ���������, ���� ���������
{
    float timePrev = buffer[(bufferPos - 1 + bufferSize) % bufferSize ].time;
    float time = ofGetElapsedTimef();
    if (timePrev < 0 || time - timePrev >= _historySec / bufferSize) {
        buffer[ bufferPos ] = pbPinchLinePointBufferData(p, time);
        bufferPos++;
        bufferPos %= bufferSize;
    }
}

ofxVec2f pbPinchLinePoint::lastBufferPoint()	//����� ������ ����� � ������
{
    pbPinchLinePointBufferData &b = buffer[ bufferPos ];
    if (b.time < 0) {
        return p0;
    } else {
        return b.p;
    }
}

bool pbPinchLinePoint::checkPoint(float thresh)			//��������, ����������� �� �����
{
    pbPinchLinePointBufferData &b = buffer[ bufferPos ];
    //���� ��� ������� - ��
    if (b.time < 0) {
        return true;
    }

    //���� ����� ����������� ����� - ��
    if ((p - p0).length() < thresh) {
        return true;
    }
    return ((p - lastBufferPoint()).length() >= thresh);
}

//-----------------------------------------------------------------------
void pbPinchEuler::setup(int flowW, int flowH, float activateLength)
{
    const float historySec = 0.5;		//����� �������
    const int step = 3;					//�������� ��� ������� �����
    //----------------------

    _w = flowW;
    _h = flowH;
    _activateLength = activateLength;

    /*for (int y=y0-15; y<=y0+15; y+=step) {
    	for (int x=_x0; x<_x0 + width; x+=step ) {
    		_p.push_back( pbPinchLinePoint() );
    		_p[ _p.size() - 1 ].setup( x, y );
    	}
    }*/
    for (int y = 0; y < _h; y += step) {
        for (int x = 0; x < _w; x += step) {
            _p.push_back(pbPinchLinePoint());
            _p[ _p.size() - 1 ].setup(x, y, historySec);
        }
    }
}

//-----------------------------------------------------------------------
//��� ����� ��� ����������
void pbPinchEuler::updateCamera(float dt, const Mat &flowX, const Mat &flowY, const Mat &flowAbs, bool debugDraw)
{
    const float kMoveThresh = 2; //5; //10;	//�������� �� ������� ������ ��� ���������� �� historySec ������

    const int kLocRad = 5;
    //3;
    //1;			//�������� ������ ������ ���������

    const bool kMovePosToMax = false;	//�������� �� ����� � ����������� � ����������� ���������
    //true;
    //----------------------

    //�����
    for (int i = 0; i < _p.size(); i++) {
        pbPinchLinePoint &p = _p[i];

        //���� ��������� �� ������� ���� - �������
        if (p.activated) {
            p.reset();
        }

        int x = p.p.x;
        int y = p.p.y;
        if (x >= 0 && x < _w && y >= 0 && y < _h) {
            //ofxVec2f action( flowX.at<float>( y, x ), flowY.at<float>( y, x ) );
            //����� ���������


            cv::Rect r = cv::Rect(x - kLocRad, y - kLocRad, kLocRad * 2 + 1, kLocRad * 2 + 1) &
                         cv::Rect(0, 0, _w, _h);

            double maxVal;
            cv::Point maxLoc;

            minMaxLoc(flowAbs(r), 0, &maxVal, 0, &maxLoc);

            int xm = maxLoc.x + r.x;
            int ym = maxLoc.y + r.y;
            ofxVec2f action(flowX.at<float>(ym, xm), flowY.at<float>(ym, xm));


            //�������� ����� � ��������, ���� ��� ������������� �����
            if (kMovePosToMax) {
                //!!! ��� �������� � �����������. ��� Kaledopix - �� ����!
                //� ��� ������ ��������� - ����� �����!!

                float currentVal = flowAbs.at<float>(y, x);
                if (maxVal > currentVal) {
                    p.p = ofxVec2f(xm, ym);
                    //cout << "xm " << xm << "  ym " << ym << endl;
                }
            }

            //�������� ����� ������
            p.p += action;
            p.p.x = std::max(std::min(p.p.x, _w - 1.0f), 0.0f);
            p.p.y = std::max(std::min(p.p.y, _h - 1.0f), 0.0f);
            if (p.checkPoint(kMoveThresh)) {
                //��� ��
                p.pushBuffer();
            } else {
                //��������� ��������, �����
                p.reset();
            }


        } else {
            p.reset();
        }

    }


    //���� �����������
    //������������ ������ ������
    if (_activateLength > 0) {
        for (int i = 0; i < _p.size(); i++) {
            pbPinchLinePoint &p = _p[i];
            if (p.p.distance(p.p0) >= _activateLength) {
                p.activated = true;
            }
        }
    }


    //���������
    if (debugDraw) {
        Mat image0(_h, _w, CV_8UC3);
        image0.setTo(cv::Scalar(0, 0, 0));

        Mat image;
        const int zoom = 3;
        resize(image0, image, cv::Size(_w * zoom, _h * zoom));

        //��� �����
        for (int i = 0; i < _p.size(); i++) {
            pbPinchLinePoint &p = _p[i];
            int x0 = p.p0.x * zoom;
            int y0 = p.p0.y * zoom;
            int x1 = p.p.x * zoom;
            int y1 = p.p.y * zoom;
            cv::Scalar color = (p.activated) ? cv::Scalar(0, 0, 255) : cv::Scalar(128, 128, 128);
            int thickness = (p.activated) ? 3 : 1;
            line(image, cv::Point(x0, y0), cv::Point(x1, y1), color, thickness);
        }

        //����� �������
        /*float best = 0;
        for (int i=0; i<_p.size(); i++) {
        	pbPinchLinePoint &p = _p[i];
        	best = max( best, p.p.x - p.p0.x );
        }
        line( image, cv::Point( 0, 3 * zoom ),
        	         cv::Point( best * zoom, 3 * zoom ), cv::Scalar( 0, 0, 255 ), 3 );*/


        imshow("line", image);
    }
}

//-----------------------------------------------------------------------
void pbPinchEuler::update(float dt)
{

}

//-----------------------------------------------------------------------
void pbPinchEuler::draw(float x, float y, float w, float h)
{
    float sclX = w / _w;
    float sclY = h / _h;
    //��� �����

    for (int i = 0; i < _p.size(); i++) {
        pbPinchLinePoint &p = _p[i];
        int x0 = p.p0.x * sclX;
        int y0 = p.p0.y * sclY;
        int x1 = p.p.x * sclX;
        int y1 = p.p.y * sclY;
        if (p.activated) {
            ofSetColor(255, 0, 0);
            ofSetLineWidth(3);
            ofLine(x0, y0, x1, y1);
        } else {
            //ofSetColor( 128, 128, 128 );
            //ofSetLineWidth( 1 );
            //ofLine( x0, y0, x1, y1 );
        }
    }

    ofSetLineWidth(1);
}

//-----------------------------------------------------------------------
