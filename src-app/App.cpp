#include "App.h"
#include "highgui.h"

#include "ofxIniSettings.h"

#include "ofxOpenCv.h"
#include "pbForeground.h"
#include "pbBinaryDenoiser.h"

#include <Box2D/Box2D.h>
#include "PhysicsWorld.h"

#include "pbValueAnimation.h"
//#include "product.h"
#include "pcvConverter.h"

int _score = 0;
int _w = 800;
int _h = 600;
int _frameRate = 60;

Mat _backDepth;
bool _backDepthInited = false;


float kTimeStep;

const float kGravity = -5.0f; //-10.0f;
const float kScreenSizeInMeters = 4.0; //1.0;


ofImage _ofCameraImage;


ofxCvColorImage			colorImg;
ofxCvColorImage			colorImgProc;
ofxCvGrayscaleImage		grayImage;

ofxCvGrayscaleImage		renderMaskImageProc;
ofxCvGrayscaleImage		renderMaskImage;
unsigned char			*renderColorImageData;
ofImage					renderColorImage;


bool inited = false;


const string _backgroundLearnFileName = "../LearnedBackground.dat";
const float kLearnDelay = 3.0;
const float kLearnDuration = 3.0; //10.0;

pbForeground _foregr;
pbBinaryDenoiser _denoiserPhys;
pbBinaryDenoiser _denoiserRender;

ofImage backgroundImage;

ofImage learningBackgroundMessage; //"LearningBackgroundMessage.png"
pbValueAnimation _learningBackgroundMessageAlpha;

//b2Body* body;
PhysicsWorld _physics;

ofRectangle _tubeRenderRect;
ofRectangle _tubeCheckRect;
ofImage		_tubeImageBack, _tubeImageFront;
int _tubeEdge0, _tubeEdgeN;


const int nCatalog = 4;
const int nCircleClasses = nCatalog + 1;
ofImage circleImage[ nCircleClasses ];


vector<ofPoint> testVector;
vector<ofPoint> moveVector;

//#define SHOW_LOGO

void physicsControl();
void updatePhysicsMask();
void updateRenderMask(IplImage *inputMask, ofxCvColorImage *inputImage,
                      int grabW, int grabH);



//-----------------------------------------------------------------
int App::w()
{
    return _w;
}
int App::h()
{
    return _h;
}
int App::frameRate()
{
    return _frameRate;
}
int App::countScore()
{
    return _score;
}


//--------------------------------------------------------------
void initTube()
{
    ofxIniSettings ini;
    ini.load("data/game.ini");  //"data/settings.ini");

    _tubeImageBack.loadImage("Basket/Basket-Back.png");
    _tubeImageFront.loadImage("Basket/Basket-Front.png");

    float tw = _tubeImageFront.getWidth();
    float th = _tubeImageFront.getHeight();

    float x0 = ini.get("Hat.X0", 0.f);
    float y0 = ini.get("Hat.Y0", 0.f);
    float w0 = ini.get("Hat.Width", 0.f);
    float scale = w0 / tw;
    x0 -= scale * tw / 2;
    y0 -= scale * th / 2;


    ofRectangle rect;
    rect.x		= ini.get("Hat.RectX", 0.f);
    rect.y		= ini.get("Hat.RectY", 0.f);
    rect.width	= ini.get("Hat.RectWidth", 0.f);
    rect.height = ini.get("Hat.RectHeight", 0.f);

    int n = ini.get("Hat.Points", 0);
    vector<ofPoint> p(n);
    vector<bool> use(n);
    for (int i = 0; i < n; i++) {
        p[i].x = ini.get("Hat.p" + ofToString(i + 1) + "x", 0);
        p[i].y = ini.get("Hat.p" + ofToString(i + 1) + "y", 0);
        use[i] = (p[i].x >= 0 && p[i].y >= 0);

        p[i].x = p[i].x * scale + x0;
        p[i].y = p[i].y * scale + y0;
    }

    _tubeRenderRect = ofRectangle(x0, y0, tw * scale, th * scale);
    _tubeCheckRect = ofRectangle(x0 + rect.x * scale, y0 + rect.y * scale, rect.width * scale, rect.height * scale);

    int index = 10;
    _tubeEdge0 = index;
    for (int i = 0; i < n - 1; i++) {
        if (use[i] && use[i + 1]) {
            _physics.addStaticEdge(p[i], p[i + 1], index++);
        }
    }

    _tubeEdgeN = index - _tubeEdge0;




}

//-----------------------------------------------------------------
void App::setup()
{
    kTimeStep = 1.0 / frameRate();
    score.load("vag.ttf", 26);

    _foregr.setup(procW(), procH());

    if (!_foregr.load(_backgroundLearnFileName)) {
        _foregr.startLearn(kLearnDelay, kLearnDuration, _backgroundLearnFileName);
    }

    _denoiserPhys.setup(procW(), procH());
    _denoiserRender.setup(procW(), procH());



    colorImg.allocate(grabW(), grabH());
    colorImgProc.allocate(procW(), procH());
    grayImage.allocate(grabW(), grabH());

    renderMaskImageProc.allocate(procW(), procH());
    renderMaskImage.allocate(grabW(), grabH());
    renderColorImage.allocate(grabW(), grabH(), OF_IMAGE_COLOR_ALPHA);
    renderColorImageData = new unsigned char[ grabW() * grabH() * 4 ];


    //---------------------
    backgroundImage.loadImage("Background.png");

    learningBackgroundMessage.loadImage("LearningBackgroundMessage.png");
    _learningBackgroundMessageAlpha.setup(1.0, 0.0);

    {

        int index = 0;
        circleImage[ index++ ].loadImage("Ball/ball.png");
        for (int i = 0; i < nCatalog; i++) {
            char buffer[1024];
            sprintf(buffer, "Catalog/ball_%02d.png", i + 1);
            string fileName = buffer;
            circleImage[ index++ ].loadImage(fileName);
        }
    }

    //physics-----------
    _physics.setup(_w, _h, kScreenSizeInMeters, kTimeStep, kGravity);

    //TEST
    //_physics.addTriangle( ofPoint( 0, h ), ofPoint( 100, h ), ofPoint( 0, h-100 ) );

    _physics.addStaticEdge(ofPoint(0, _h - 1), ofPoint(_w, _h - 1), 0);
    //_physics.addStaticEdge( ofPoint( 0, h - 1 ), ofPoint( w, h - 1 - 20 ), 0 );

    //_physics.addStaticEdge( ofPoint( 0, 1 ), ofPoint( w, 1 ), 1 );
    //_physics.addStaticEdge( ofPoint( 1, 0 ), ofPoint( 1, h ), 2 );
    //_physics.addStaticEdge( ofPoint( w-1, 0 ), ofPoint( w-1, h ), 3 );

    //_physics.addStaticEdge( ofPoint( 1, 0 ), ofPoint( 1, h - h*0.2 ), 4 );
    //_physics.addStaticEdge( ofPoint( w-1, 0 ), ofPoint( w-1, h - h*0.2 ), 5 );

    initTube();



    int n = 30; //15; //150;
    for (int i = 0; i < n; i++) {

        CircleData c;
        c.rad		= ofRandom(15, 20);   //ofRandom( 10, 20 ); //ofRandom( 5, 20 );
        c.p			= ofPoint(ofRandom(c.rad, _w - c.rad), ofRandom(c.rad, _h / 2));
        c.angleDeg		= 0.0;
        c.groupIndex	= 0;
        c.density		= 10.0;//1.0;
        c.friction		= 0.2; //0.5;
        c.restitution	= 0.4; //0.2;

        _physics.addCircle(c);
    }

}

//-----------------------------------------------------------------
void App::exit()
{

}

//-----------------------------------------------------------------
void App::update(float dt)
{
    if (!_foregr.learning()) {
        applyForces(_denoiserPhys.mask(), _drawMode == 1);
    }

    physicsControl();

    _physics.update();

    {
        float target = _learningBackgroundMessageAlpha.target();
        if (_foregr.learning() && target == 0.0) {
            _learningBackgroundMessageAlpha.setTarget(1.0);
        }
        if (!_foregr.learning() && target == 1.0) {
            _learningBackgroundMessageAlpha.setTarget(0.0);
        }
        _learningBackgroundMessageAlpha.update(dt);
    }
}

//-----------------------------------------------------------------

void App::updateCamera(float dt, pbVideoGrabber &grabber, bool debugDraw)
{
    const Mat &imageGrab = grabber.grabFrameCVUnique(grabW(), grabH());
    pcvConverter::matToOfImage3(imageGrab, _ofCameraImage);

    colorImg.setFromPixels(_ofCameraImage.getPixels());

    colorImgProc.scaleIntoMe(colorImg);
    _foregr.update(colorImgProc.getCvImage(), 0);
    updatePhysicsMask();
    {
        updateRenderMask(_foregr.mask(), &colorImg, grabW(), grabH());
    }

}

//--------------------------------------------------------------
void App::drawProduction()
{
    //ofSetColor( 255, 255, 255, 255 );
    //cameraImageOf().draw( 0, 0, _w, _h );


    bool normalMode = !_foregr.learning();

    if (!normalMode) {
        ofSetColor(255, 255, 255);
        _ofCameraImage.draw(0, 0, _w, _h);
    } else {
        ofSetColor(255, 255, 255);
        backgroundImage.draw(0, 0, _w, _h);
        renderColorImage.draw(0, 0, _w, _h);
    }

    {
        ofSetColor(255, 255, 255, 255);
        _tubeImageBack.draw(_tubeRenderRect.x, _tubeRenderRect.y, _tubeRenderRect.width, _tubeRenderRect.height);

    }

    ofNoFill();
    vector<CircleData> circles = _physics.circles();
    for (int i = 0; i < circles.size(); i++) {
        CircleData &c = circles[i];

        ofSetColor(255, 255, 255, 225);   //200 );
        ofPushMatrix();
        ofTranslate(c.p.x, c.p.y);
        ofRotate(c.angleDeg);
        ofImage &image = circleImage[ c.groupIndex ];
        image.draw(-c.rad, -c.rad, c.rad * 2, c.rad * 2);
        ofPopMatrix();
    }

    {
        int alpha = 255;
        ofSetColor(255, 255, 255, alpha);
        _tubeImageFront.draw(_tubeRenderRect.x, _tubeRenderRect.y, _tubeRenderRect.width, _tubeRenderRect.height);

        if (_drawMode == 1) {
            ofSetColor(255, 0, 0, alpha);
            vector<EdgeData> staticEdges = _physics.staticEdges();
            for (int i = _tubeEdge0; i < _tubeEdge0 + _tubeEdgeN; i++) {
                EdgeData &edge = staticEdges[ i ];
                ofPoint p1 = edge.p1;
                ofPoint p2 = edge.p2;
                ofLine(p1.x, p1.y, p2.x, p2.y);
            }
        }
    }


    if (_foregr.learning()) {
        float a = _foregr.learningProgress();
        ofFill();
        ofSetColor(0, 255, 0, 128);
        ofRect(0, _h - _h / 8, a * _w, _h / 8);

    }


    {
        float scale = 1.0 * learningBackgroundMessage.getWidth() / learningBackgroundMessage.getHeight();
        int w = _w / 2;
        int h = w / scale;


        float alpha  = 200.0 * _learningBackgroundMessageAlpha.value();
        if (alpha > 0) {
            ofSetColor(255, 255, 255, alpha);
            learningBackgroundMessage.draw((_w - w) / 2, (_h - h) / 2, w, h);
        }
    }
    ofSetHexColor(0x009fff);
    score.drawString("Score : " + ofToString(countScore()), 40, 40);
    if (countScore() >= 10) {
        ofSetHexColor(0xff69b4);
        score.drawString("CONGRATULATIONS\nPress R to restart.\nPress L to Re-configure.", 250, 300);
    }
}

//--------------------------------------------------------------
//
void pushView(int x, int y, float scale)
{
    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(scale, scale);
}

void popView()
{
    ofPopMatrix();
}

//--------------------------------------------------------------
void App::drawDebug()
{
    //ofSetColor( 255, 255, 255, 255 );
    //cameraImageOf().draw( 0, 0, _w, _h );

    pushView(_w / 2, 0, 0.5);
    ofSetColor(255, 255, 255);
    _ofCameraImage.draw(0, 0, _w, _h);
    popView();

    pushView(0, 0, 0.5);
    drawProduction();
    popView();

    pushView(_w / 2, _h / 2, 0.5);
    _foregr.draw(0, 0, _w / 2, _h / 2);
    ofSetColor(0, 255, 255);
    _denoiserPhys.draw(0, _h / 2, _w / 2, _h / 2);

    ofSetColor(255, 0, 255, 255);
    _denoiserRender.draw(_w / 2, 0, _w / 2, _h / 2);
    renderMaskImage.draw(_w / 2, _h / 2, _w / 2, _h / 2);

    popView();

    //renderMaskImage.draw( _w/2, 0, _w/2, _h/2 );


    pushView(0, _h / 2, 0.5);
    ofFill();
    ofSetColor(255, 255, 255);
    ofRect(0, 0, _w, _h);
    ofSetColor(0, 255, 255, 128);
    _denoiserPhys.draw(0, 0, _w, _h);

    ofSetColor(255, 0, 0);
    vector<EdgeData> staticEdges = _physics.staticEdges();
    for (int i = 0; i < staticEdges.size(); i++) {
        EdgeData &edge = staticEdges[ i ];
        if (edge.used) {
            ofPoint p1 = edge.p1;
            ofPoint p2 = edge.p2;
            ofLine(p1.x, p1.y, p2.x, p2.y);
        }
    }


    ofFill();
    ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
    ofSetColor(128, 0, 0);
    vector<TriangleData> triangles = _physics.triangles();
    for (int i = 0; i < triangles.size(); i++) {
        TriangleData &tri = triangles[ i ];
        ofBeginShape();
        for (int k = 0; k < 3; k++) {
            ofVertex(tri.p[k].x, tri.p[k].y);
        }
        ofEndShape();
    }
    ofNoFill();


    ofNoFill();
    vector<CircleData> circles = _physics.circles();
    for (int i = 0; i < circles.size(); i++) {
        CircleData &c = circles[i];
        ofSetColor(0, 255, 0);
        ofCircle(c.p.x, c.p.y, c.rad);
        float angle = c._body->GetAngle();
        float dx = cos(-angle);
        float dy = sin(-angle);
        ofLine(c.p.x + dx * c.rad * 0.5, c.p.y + dy * c.rad * 0.5, c.p.x + dx * c.rad, c.p.y + dy * c.rad);
    }

    {

        int alpha = 255;
        ofSetColor(0, 0, 0, alpha);
        ofSetLineWidth(1);
        vector<EdgeData> staticEdges = _physics.staticEdges();
        for (int i = _tubeEdge0; i < _tubeEdge0 + _tubeEdgeN; i++) {
            EdgeData &edge = staticEdges[ i ];
            ofPoint p1 = edge.p1;
            ofPoint p2 = edge.p2;
            ofLine(p1.x, p1.y, p2.x, p2.y);
        }
        ofSetLineWidth(1);


        ofFill();
        ofSetColor(0, 0, 255, 128);
        ofRect(_tubeCheckRect.x, _tubeCheckRect.y, _tubeCheckRect.width, _tubeCheckRect.height);
        ofNoFill();
    }




    if (testVector.size() == circles.size()) {
        ofSetColor(255, 255, 0);
        for (int i = 0; i < circles.size(); i++) {
            CircleData &c = circles[i];
            ofLine(c.p.x, c.p.y, c.p.x + testVector[i].x, c.p.y + testVector[i].y);
        }
    }
    popView();

}

//-----------------------------------------------------------------
void App::keyPressed(int key)
{
    switch (key) {

        case 'l': {
            _foregr.startLearn(kLearnDelay, kLearnDuration, _backgroundLearnFileName);
        }
        break;
        case 'r': {
            _score = 0;
        }
        break;
    }

}

//-----------------------------------------------------------------
void App::mousePressed(int x, int y, int button)
{

}

//-----------------------------------------------------------------

//--------------------------------------------------------------
void App::findMoveFromMask(IplImage *mask, ofPoint p, int rad, ofxVec2f &newPos, bool &moved)
{
    const float scaleX = 1.0 * procW() / _w;
    const float scaleY = 1.0 * procH() / _h;

    const float RAD_BOOST = 1.0;//1.0;
    //int rad = c.rad * RAD_BOOST;
    int x, y;
    bool inside;


    //ofPoint p = c.p;
    newPos = ofxVec2f(0, 0);
    moved = false;


    const int iterations = 3;

    for (int iter = 0; iter < iterations; iter++) {
        ofxVec2f sum(0, 0);
        float sumCount = 0.0;
        bool ok = true;
        for (int dy = -rad; dy <= rad; dy++) {
            for (int dx = -rad; dx <= rad; dx++) {
                if (dx * dx + dy * dy <= rad * rad) {
                    x = (p.x + dx) * scaleX;
                    y = (p.y + dy) * scaleY;
                    bool inside = (x >= 0 && x < procW() && y >= 0 && y < procH());
                    if (!inside || CV_IMAGE_ELEM(mask, uchar, y, x) == 0) {
                        sumCount += 1.0;
                        sum += ofxVec2f(dx, dy);
                    } else {
                        ok = false;
                    }

                }
            }
        }

        if (sumCount > 0.001) {
            p = p + sum / sumCount;
            newPos = p;
            moved = true;
        }
        if (ok) {
            break;
        }
    }

}


//--------------------------------------------------------------
void App::applyForces(IplImage *mask, bool debugDraw)
{
    vector<CircleData> &circles = _physics.circles();
    for (int i = 0; i < circles.size(); i++) {
        CircleData &c = circles[ i ];

        ofxVec2f newPos = c.p;
        bool moved;
        findMoveFromMask(mask, c.p, c.rad, newPos, moved);

        if (moved) {
            ofxVec2f bestPos = newPos;
            c._body->SetTransform(_physics.toWorld(bestPos), c._body->GetAngle());

            const float ForceBoost = 20.0;

            ofxVec2f delta = bestPos - c.p;
            b2Vec2 force = _physics.forceToWorld(delta);
            force *= c._body->GetMass() * 1.0 / kTimeStep * ForceBoost;
            c._body->ApplyForce(force, c._body->GetPosition(), false);

        }



        if (debugDraw) {
            if (testVector.size() < circles.size()) {
                testVector.resize(circles.size());
            }
            testVector[ i ] = (moved) ? (newPos - c.p) : ofPoint(0, 0);
        }

    }

}


//--------------------------------------------------------------
void physicsControl()
{
    const float letDown = 0.2;
    float fromUp = 0.0;

    const float tubeTransformRad = 1.5;
    vector<CircleData> &circles = _physics.circles();

    float letDownY = letDown * _h;
    float fromUpY = fromUp * _h;

    for (int i = 0; i < circles.size(); i++) {
        CircleData &c = circles[ i ];
        if (c.p.y - c.rad > _h + letDownY) {
            ofPoint p = ofPoint(ofRandom(c.rad, _w - c.rad), -c.rad + ofRandom(-fromUpY, 0));
            _physics.setCirclePosAndVelocity(i, p, ofPoint(0, 0), 0.0);

            if (c.groupIndex > 0) {
                _physics.setCircleRadAndTexture(i, c.rad / tubeTransformRad, 0);
            }
        }

    }

    for (int i = 0; i < circles.size(); i++) {
        CircleData &c = circles[ i ];
        ofPoint p = c.p;
        float rad = c.rad;

        ofRectangle &rect = _tubeCheckRect;
        if (c.groupIndex == 0 &&
                p.x - rad >= rect.x && p.x + rad <= rect.x + rect.width
                && p.y >= rect.y && p.y <= rect.y + rect.height) {
            int texture = 1 + int(ofRandom(0, nCatalog));
            rad *= tubeTransformRad;
            _physics.setCircleRadAndTexture(i, rad, texture);
            _score += 1;
        }

    }

}

//--------------------------------------------------------------

void updatePhysicsMask()
{
    int iter = 1;
    int minPerimeter = 150;
    int blackAreaThreshold = 100;
    int whiteAreaThreshold = 400;

    _denoiserPhys.denoiseUsingFloodFill(_foregr.mask(), 0, iter, blackAreaThreshold, whiteAreaThreshold);
}

//--------------------------------------------------------------
void updateRenderMask(IplImage *inputMask, ofxCvColorImage *inputImage, int grabW, int grabH)
{
    if (inputImage->width == 0) {
        return;
    }

    int iter = 1;
    int blackAreaThreshold = 400;
    int whiteAreaThreshold = 400;
    _denoiserRender.denoiseUsingFloodFill(inputMask, 0, iter, blackAreaThreshold, whiteAreaThreshold);

    int additIter = 1;
    cvMorphologyEx(_denoiserRender.mask(), _denoiserRender.mask(), 0, 0, CV_MOP_CLOSE, additIter);


    renderMaskImageProc = _denoiserRender.mask();
    renderMaskImage.scaleIntoMe(renderMaskImageProc);

    IplImage *mask = renderMaskImage.getCvImage();


    const int SmoothWindow = 7; //5;
    const int SmoothIter = 3; //10;
    for (int i = 0; i < SmoothIter; i++) {
        cvSmooth(mask, mask, CV_GAUSSIAN, SmoothWindow);
    }

    int w = grabW;
    int h = grabH;

    unsigned char *in = inputImage->getPixels().getData();
    unsigned char *out = renderColorImageData;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int value = CV_IMAGE_ELEM(mask, uchar, y, x);
            int inIndex = 3 * (x + w * y);
            int index = 4 * (x + w * y);
            out[ index + 0 ] = in[ inIndex + 0 ];
            out[ index + 1 ] = in[ inIndex + 1 ];
            out[ index + 2 ] = in[ inIndex + 2 ];
            out[ index + 3 ] = value;
        }
    }
    renderColorImage.setFromPixels(out, w, h, OF_IMAGE_COLOR_ALPHA);

}

//--------------------------------------------------------------
