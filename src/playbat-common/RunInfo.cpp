#include <windows.h>

#include "RunInfo.h"


#include "paths.h"
//#include "product.h"
#include "pbFiles.h"
#include <glut.h>

RunInfo runInfo;

//--------------------------------------------------------------
RunInfo::RunInfo(void)
: _args( 0, 0 )
{	
	_w = 800;
	_h = 600;
	_screenPhysW = 0;
	_screenPhysH = 0;

	_screenFormat = pbScreenFormat_3x4;
	_mouseShow = false;

	_rotate = 0;	
	_mirror = 0;
	_resizeMode = ResizeModeCrop;
	_fullScreen=0;	
	_runDurationSec = 0.0;
	_soundVolume = 1.0;
	_cameraDevice = 0;
	_cameraDevice2 = 1;
	_cameraPS3Eye = 0;
	_cameraFrameRate = 30;
	_cameraMirror = 0;
	_cameraGrabW = 320;
	_cameraGrabH = 240;
	_cameraPhysW = _cameraGrabW;
	_cameraPhysH = _cameraGrabH;
	_cameraProcessW = 320;
	_cameraProcessH = 240;
	_cameraOptFlowW = 320;
	_cameraOptFlowH = 240;


	_durationControl = true;
	_fadeDuration = 0.0;
	_introDuration = 0.0;

	_printFps		= false;	
	_updateStart	= 0.0;
	_updateEnd		= 0.0;

	_pWindow = 0;
	
}

//--------------------------------------------------------------
int RunInfo::readInt( const string &key, int defaultValue )
{
	int result = defaultValue;
	result = _defaultIni.get( key, result );
	result = _userIni.get( key, result );
	if ( _args.contains( key ) ) {
		result = _args.getInt( key );
	}

	return result;
}

float RunInfo::readFloat( const string &key, float defaultValue )
{
	float result = defaultValue;
	result = _defaultIni.get( key, result );
	result = _userIni.get( key, result );
	if ( _args.contains( key ) ) {
		result = _args.getFloat( key );
	}
	return result;
}

void RunInfo::writeInt( const string &key, int value )
{
	_userIni.set( key, value );
}

void RunInfo::writeFloat( const string &key, int value )
{
	_userIni.set( key, value );
}


//--------------------------------------------------------------
void RunInfo::setup( int argc, char *argv[], ofAppGlutWindow *window ) 
{
	sharedPaths.init( "" );
	//cout << sharedPaths.userSettingsIniFile() << endl;

	//cout << "--- You can use command line parameters : --------" << endl;
	//cout << "-SettingsFile=[filename] -Screen.scale=[0.1...1.0] -Run.durationSec=[1..10000]" << endl;

	string settingsFileName = "data/settings.ini";
	float scale = 1.0;

	_args = ofxArgs(argc, argv);
	if(_args.contains("-settingsFile")){
		settingsFileName = _args.getString("-settingsFile");
	}

	//ofxIniSettings ini;
	_defaultIni.load( settingsFileName );//"data/settings.ini");

	//ofxIniSettings *userIni = 0;
	string userSettingsFileName = sharedPaths.userSettingsIniFile();
	if ( ofFileExists( userSettingsFileName ) ) {
		//userIni = new ofxIniSettings();
		_userIni.load( userSettingsFileName );
		_userIni.outputFilename = userSettingsFileName;
	}

	_w				= readInt( "Screen.w", _w );
	_h				= readInt( "Screen.h", _h );
	_screenPhysW	= readFloat( "Screen.physW", _screenPhysW );
	_screenPhysH	= readFloat( "Screen.physH", _screenPhysH );

	_screenFormat	= screenFormatFromStr( _defaultIni.get("Screen.screenFormat", string("") ) );
	_fullScreen		= readInt( "Screen.fullScreen", _fullScreen );
	_rotate			= readInt("Screen.rotate", _rotate);
	_mirror			= readInt("Screen.mirror", _mirror);
	_fadeDuration	= _defaultIni.get("Screen.fadeDuration", _fadeDuration );
	_introDuration	= _defaultIni.get("Screen.introDuration", _introDuration );
	_introImageFile		= _defaultIni.get("Screen.introImage", string("") );

	string resizeMode = _defaultIni.get("Screen.resizeMode", string("") );
	if ( resizeMode == "crop" ) _resizeMode = ResizeModeCrop;
	if ( resizeMode == "fit" ) _resizeMode = ResizeModeFit;

	_mouseShow		= readInt( "Screen.mouseShow", _mouseShow );

	_soundVolume = _defaultIni.get( "Sound.volume", _soundVolume);

	_cameraDevice	= readInt( "Camera.device", _cameraDevice );
	_cameraDevice2	= readInt( "Camera.device2", _cameraDevice );
	_cameraPS3Eye   = readInt( "Camera.PS3Eye", _cameraPS3Eye );
	_cameraKinect   = readInt( "Camera.Kinect", _cameraKinect );

	_cameraFrameRate	= _defaultIni.get( "Camera.frameRate", _cameraFrameRate );
	_cameraMirror	= _defaultIni.get( "Camera.mirror", _cameraMirror );
	_cameraGrabW	= _defaultIni.get( "Camera.grabW", _cameraGrabW );
	_cameraGrabH	= _defaultIni.get( "Camera.grabH", _cameraGrabH );
	_cameraPhysW	= _defaultIni.get( "Camera.physW", _cameraGrabW );	
	_cameraPhysH	= _defaultIni.get( "Camera.physH", _cameraGrabH );
	_cameraProcessW = _defaultIni.get( "Camera.processW", _cameraProcessW );
	_cameraProcessH = _defaultIni.get( "Camera.processH", _cameraProcessH );
	_cameraOptFlowW = _defaultIni.get( "Camera.optFlowW", _cameraOptFlowW );
	_cameraOptFlowH = _defaultIni.get( "Camera.optFlowH", _cameraOptFlowH );


	if ( _cameraKinect ) {
		_cameraGrabW = 640;
		_cameraGrabH = 480;
	}


	if(_args.contains("-Screen.scale")){
		scale = _args.getFloat("-Screen.scale");
	}

	_runDurationSec = readInt( "Run.durationSec", _runDurationSec );
	if(_args.contains("-Run.durationSec")){
		_runDurationSec = _args.getFloat("-Run.durationSec");
	}

#ifdef PRODUCT_TYPE_TRIAL
	if ( PRODUCT_TRIAL_TIMELIMIT_ENABLED ) {
		_runDurationSec = PRODUCT_TRIAL_PLAYING_SEC;
	}
#endif

	_w = _w * scale;
	_h = _h * scale;

	_renderAngle	= _rotate * 90;
	_renderW		= _w;
	_renderH		= _h;
	if ( ((_renderAngle + 360) / 90) % 2 == 1 ) { 
		swap( _renderW, _renderH );
	}

	if ( window ) {
		ofSetupOpenGL(window, 
			screenWidth(), screenHeight(), 
			(fullScreen() == 1) ? OF_FULLSCREEN : OF_WINDOW
			);			// <-------- setup the GL context
		ofSetWindowTitle( "final project" );

		if ( fullScreen() == 2 ) {
			ofSetWindowPosition(0,0);
		}
	}
	_pWindow = window;

	if ( !mouseShow() ) {
		ofHideCursor();	
	}


	cout << "----------------------------------------------" << endl;
	//cout << PRODUCT_TITLE << " version " << PRODUCT_VERSION << endl;
#ifdef PRODUCT_TYPE_TRIAL
	cout << "Trial mode" << endl;
#endif
	cout << "Screen " << _w << " x " << _h << ", " 
		<< " camera " << _cameraDevice << ", " 
		<< " duration " << _runDurationSec 
		<< endl;
	cout << "----------------------------------------------" << endl;

}

//--------------------------------------------------------------
void RunInfo::setupInSetup()
{
	if ( _introDuration > 0 && _introImageFile != "" ) {
		_introImage.loadImage( _introImageFile );
	}
}
//--------------------------------------------------------------
int _storeFactW = 800;	
int _storeFactH = 600;

void RunInfo::drawBegin( int factW, int factH )	
{
	ofPushMatrix();   

	_storeFactW = factW;
	_storeFactH = factH;


	ofTranslate( screenWidth() / 2, screenHeight() / 2 );	
	ofRotate( renderAngle() );	
	ofTranslate( -renderWidth() / 2, -renderHeight() / 2 );	
	
	if ( factW > 0 && factH > 0 ) {
		float scaleOutX = 1.0 * renderWidth() / factW;
		float scaleOutY = 1.0 * renderHeight() / factH;
		

		ofTranslate( renderWidth() / 2, renderHeight() / 2 );	
		ofScale( scaleOutX * ((_mirror == 0) ? 1.0 : -1.0), scaleOutY );
		ofTranslate( -factW / 2, -factH / 2 );	
	}
	


}

//--------------------------------------------------------------
void RunInfo::drawEnd()	
{
	ofPopMatrix();


	const float now = ofGetElapsedTimef();


	const float introSec = introDuration();
	if ( introSec > 0.01 ) {
		if ( now < introSec ) {
			const float introFade = _fadeDuration;		
			int alpha = 255;
			if ( now > introSec - introFade ) {
				alpha = 255 * (1.0 * (introSec - now ) / introFade ); 
			}
			ofEnableAlphaBlending();
			ofSetColor( 255, 255, 255, alpha );
			_introImage.draw( 0, 0,  renderWidth(), renderHeight() );
			ofDisableAlphaBlending();
		}
		
	}

	const float fadeSec = _fadeDuration;
	if ( fadeSec > 0.01 ) {
		bool fade = false;
		float alpha = 0.0;
		if ( durationControl() && now >= _runDurationSec - fadeSec && _runDurationSec > 0.1 ) {
			fade = true;
			alpha = 1 - (_runDurationSec - now) / fadeSec;
		}
		if ( now < fadeSec ) {
			fade = true;
			alpha = 1 - now / fadeSec;
		}
		if ( fade ) {
			ofEnableAlphaBlending();
			ofFill();
			ofSetColor( 0, 0, 0, alpha * 255 );
			ofRect( 0, 0, renderWidth(), renderHeight() );

			ofNoFill();
			ofDisableAlphaBlending();
		}
	}

	if ( printFps() ) {
		doPrintFps();
	}


	
}

//--------------------------------------------------------------
void RunInfo::updateBegin()
{
	_updateStart = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void RunInfo::update()	
{
	float now = ofGetElapsedTimef();
	if ( durationControl() && now >= _runDurationSec && _runDurationSec > 0.1 ) {
		cout << "RunInfo: Exit by timer" << endl;
		OF_EXIT_APP(0);	
	}
	_updateEnd = now;
}

//--------------------------------------------------------------

void RunInfo::doPrintFps()	
{

	ofSetColor( 0, 0, 0 );
	ofFill();
	ofRect( 0, 0, 350, 30 );
	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "fps: %5d, update: %f ms", int(ofGetFrameRate()), _updateEnd - _updateStart );
	ofDrawBitmapString( reportStr, 3, 20 ); 
}


//-------------------------------------------------------------------
ofxVec2f RunInfo::windowToScreen( const ofxVec2f &p, float internalW, float internalH )	
{
	if ( _rotate == 0 ) {
		return ofxVec2f( p.x * internalW / _w, p.y * internalH / _h );
	}
	if ( _rotate == 2 ) {
		return ofxVec2f( (1.0 - p.x / _w) * internalW , ( 1.0 - p.y / _h ) * internalH  );
	}
	return ofxVec2f( 0, 0 );	
}

//-------------------------------------------------------------------
ofxVec2f RunInfo::windowToCamera( const ofxVec2f &p, float internalW, float internalH )	
{
	ofxVec2f p1 = windowToScreen( p, internalW, internalH );
	float camW = cameraProcessW();
	float camH = cameraProcessH();
	float scale = min( 1.0f * internalW / camW, 1.0f * internalH / camH );	
	p1.x /= ( scale );
	p1.y /= ( scale );
	return p1;
}

//-------------------------------------------------------------------
void RunInfo::exitApp() {
	if ( _pWindow != 0 ) {
		_pWindow->close();
	}
	else {
		cout << "WARNING: wish to exit app, but have no window pointer!" << endl;
	}
}

//-------------------------------------------------------------------
void RunInfo::consoleShow()	
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_SHOW );
	SetForegroundWindow( hWnd );
}

//-------------------------------------------------------------------
void RunInfo::consoleHide()				
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_HIDE );
}

//-------------------------------------------------------------------
void RunInfo::windowShow()					
{	
	glutShowWindow();
	string title = "final project";
	HWND hWnd = FindWindowA( 0, title.c_str() );
	if ( hWnd ) {
		SetForegroundWindow( hWnd );
	}
	else {
		cout << "WARINIG: can not Show Window - no window with title " << title << endl;
	}
}

//-------------------------------------------------------------------
void RunInfo::windowHide()				
{
	glutHideWindow();
}

//-------------------------------------------------------------------
