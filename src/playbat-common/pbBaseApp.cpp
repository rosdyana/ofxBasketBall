#include "pbBaseApp.h"
#include "RunInfo.h"
#include "pcvConverter.h"
#include "highgui.h"

pbBaseApp *_sharedBaseApp = 0;

void webserver_callback(const string &in, string &out)
{
    cout << "Webclient command: " << in << endl;
    if (_sharedBaseApp) {
        _sharedBaseApp->webserverCommand0(in, out);
    } else {
        cout << "Internal error: no _sharedBaseApp" << endl;
    }
}


//-----------------------------------------------------------------
void pbBaseApp::setup0()
{

    _sharedBaseApp = this;

    ofSetFrameRate(frameRate());
    ofSetVerticalSync(true);
    ofSeedRandom();

    _grabW = runInfo.cameraGrabW();
    _grabH = runInfo.cameraGrabH();
    _procW = runInfo.cameraProcessW();
    _procH = runInfo.cameraProcessH();
    _flowW = runInfo.cameraOptFlowW();
    _flowH = runInfo.cameraOptFlowH();

#ifdef baseAppUseGrabber
    if (cameraGrabberEnabled()) {
        _grabber.setup();
        _grabber.startCamera(runInfo.cameraDevice(), runInfo.cameraPhysW(), runInfo.cameraPhysH(), //_grabW, _grabH,
                             runInfo.cameraFrameRate(),
                             runInfo.cameraMirror()
                            );
    }
#endif

    _flowInited = false;
    _flow.setup();


    _drawMode = 0;
    _debugModeAdditional = 0;
    _debugModeDrawCamera = 0;
    _cameraSaveMode = false;
    _cameraSaveModeDepth = false;

    runInfo.setupInSetup();

    _lastTimeGrab = ofGetElapsedTimef();
    _lastTimeUpdate = ofGetElapsedTimef();

    setup();

}

//-----------------------------------------------------------------
void pbBaseApp::exit0()
{
    exit();
#ifdef baseAppUseGrabber
    if (cameraGrabberEnabled()) {
        _grabber.stopCamera();
    }
#endif
}

//-----------------------------------------------------------------

void pbBaseApp::update0()
{
    runInfo.updateBegin();

#ifdef baseAppUseGrabber
    if (cameraGrabberEnabled()) {
        if (_grabber.isFrameNew()) {
            float now = ofGetElapsedTimef();
            double dt = now - _lastTimeGrab;
            dt = min(dt, 0.1);
            _lastTimeGrab = now;

            //_cameraImageCvGrab = _grabber.grabFrameCV();
            //_cameraImageCvProc = _grabber.grabFrameCV( _procW, _procH );
            updateCamera(dt, grabber(), _debugModeAdditional);

            if (_cameraSaveMode || _cameraSaveModeDepth) {
                cameraSaveFrame(_cameraSaveMode, _cameraSaveModeDepth);
            }
        }
    }
#endif

    float now = ofGetElapsedTimef();
    double dt = now - _lastTimeUpdate;
    dt = min(dt, 0.1);
    _lastTimeUpdate = now;

    update(dt);

    runInfo.update();
}

//-----------------------------------------------------------------
const Mat &pbBaseApp::opticalFlow(const Mat &img)
{
    resize(img, _imgFlowColor, cv::Size(_flowW, _flowH));
    cvtColor(_imgFlowColor, _imgFlow, CV_RGB2GRAY);
    if (!_flowInited) {
        _flowInited = true;
        _imgFlowLast = _imgFlow.clone();
    }
    _flow.update(_imgFlowLast, _imgFlow);
    _imgFlowLast = _imgFlow.clone();

    return *_flow.flow();
}

//--------------------------------------------------------------
#ifdef baseAppUseGrabber
void pbBaseApp::cameraSaveFrame(bool saveRGB, bool saveDepth)
{
    string name = ofToString(_camFrame);
    while (name.length() < 5) {
        name = "0" + name;
    }
    name = "recordings/camera/cam_" + name;


    if (cameraGrabberEnabled()) {

        //RGB
        if (saveRGB) {
            const Mat &image =  _grabber.grabFrameCVCopy(grabW(), grabH());
            imwrite(name + ".bmp", image);


        }


    }
    _camFrame++;
}
#endif

//-----------------------------------------------------------------

void pbBaseApp::draw0()
{
    runInfo.drawBegin(w(), h());
    ofBackground(0, 0, 0);
    ofEnableAlphaBlending();

    if (_drawMode == 0) {
        drawProduction();
    }
    if (_drawMode == 1) {
        drawDebug();
    }

    if (_cameraSaveMode) {
        ofSetColor(255, 0, 0);
        ofFill();
        ofEllipse(20, 20, 20, 20);
    }
    if (_cameraSaveModeDepth) {
        ofSetColor(0, 0, 255);
        ofFill();
        ofEllipse(40, 20, 20, 20);
    }

    ofDisableAlphaBlending();
    runInfo.drawEnd();
}

//-----------------------------------------------------------------
void pbBaseApp::keyPressed0(int key)
{
    switch (key) {
        case 'z':
            runInfo.setDurationControl(false);
            return;
            break;
        case 't':
            runInfo.setPrintFps(!runInfo.printFps());
            return;
            break;

        case 'd':
            _drawMode = 1 - _drawMode;
            return;
            break;
        case 'f':
            _debugModeAdditional = 1 - _debugModeAdditional;
            return;
            break;
        case 'g':
            _debugModeDrawCamera = 1 - _debugModeDrawCamera;
            return;
            break;

        case 'r':
            //_cameraSaveMode = !_cameraSaveMode;
            //return;
            break;
        case 'R':
            //_cameraSaveModeDepth = !_cameraSaveModeDepth;
            //return;
            break;

        //case 's': vidGrabber.videoSettings();
        //	break;

        case 'p': {
            cout << "Grab screen..." << endl;
            ofImage image;
            image.grabScreen(0, 0, runInfo.renderWidth(), runInfo.renderHeight());
            image.saveImage("_screen.png");
        }
        return;
        break;
    }
    keyPressed(key);
}

//-----------------------------------------------------------------
void pbBaseApp::mousePressed0(int x, int y, int button)
{
    mousePressed(x, y, button, runInfo.mouseShow());
}

//-----------------------------------------------------------------
void pbBaseApp::mouseMoved0(int x, int y)
{
    mouseMoved(x, y, runInfo.mouseShow());
}

//-----------------------------------------------------------------
void pbBaseApp::mouseReleased0(int x, int y, int button)
{
    mouseReleased(x, y, runInfo.mouseShow());
}

//-----------------------------------------------------------------
void pbBaseApp::setup()
{

}

//-----------------------------------------------------------------
void pbBaseApp::exit()
{
}

//-----------------------------------------------------------------
void pbBaseApp::update(float dt)
{
    cout << "WARNING: App update() is empty" << endl;
}

//-----------------------------------------------------------------
#ifdef baseAppUseGrabber
void pbBaseApp::updateCamera(float dt, pbVideoGrabber &grabber, bool debugDraw)
{
    cout << "WARNING: App updateCamera() is empty" << endl;;
}
#endif

//-----------------------------------------------------------------
void pbBaseApp::drawProduction()
{
    cout << "WARNING: App drawProduction() is empty" << endl;;
}

void pbBaseApp::drawDebug()
{
}

//-----------------------------------------------------------------
void pbBaseApp::keyPressed(int key)
{
}

//-----------------------------------------------------------------
void pbBaseApp::mousePressed(int x, int y, int button, bool mouseShow)
{
}

//-----------------------------------------------------------------
void pbBaseApp::mouseMoved(int x, int y, bool mouseShow)
{

}

//-----------------------------------------------------------------
void pbBaseApp::mouseReleased(int x, int y, bool mouseShow)
{

}

//-----------------------------------------------------------------
void pbBaseApp::webserverCommand0(const string &command, string &out)
{
    webserverCommand(command, out);
}

//-----------------------------------------------------------------
void pbBaseApp::webserverCommand(const string &command, string &out)
{
}

//-----------------------------------------------------------------
