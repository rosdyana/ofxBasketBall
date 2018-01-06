#ifndef _RUN_INFO_H
#define _RUN_INFO_H

#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "ofxIniSettings.h"
#include "pbScreenFormat.h"
#include "ofxVectorMath.h"
#include "ofxArgs.h"


class RunInfo
{
public:
	RunInfo();

	void setup( int argc, char *argv[], ofAppGlutWindow *window );	
	void setupInSetup();			
	
	int readInt( const string &key, int defaultValue );
	float readFloat( const string &key, float defaultValue );
	void writeInt( const string &key, int value );
	void writeFloat( const string &key, int value );
	

	ofxArgs &args() { return _args; }	
	//ofAppGlutWindow &window() { return *_pWindow; }	
	void exitApp();						
	void consoleShow();					
	void consoleHide();					
	void windowShow();					
	void windowHide();					

	void updateBegin();
	void update();	

	void drawBegin( int factW = 0, int factH = 0 );	

	void drawEnd();	

	void setPrintFps( bool enable ) { _printFps = enable; }
	bool  printFps()			{ return _printFps; }

	int fullScreen() { return _fullScreen; }	
	int screenWidth()	{ return _w; }
	int screenHeight()	{ return _h; }

	float screenPhysW() { return _screenPhysW; }	
	float screenPhysH() { return _screenPhysH; }	

	int renderWidth()	{ return _renderW; }
	int renderHeight()	{ return _renderH; }
	int renderAngle()	{ return _renderAngle; }


	pbScreenFormat screenFormat() { return _screenFormat; }


	ofxVec2f windowToScreen(  const ofxVec2f &p, float internalW, float internalH );	
	ofxVec2f windowToCamera( const ofxVec2f &p, float internalW, float internalH );	


	int mouseShow()	{ return _mouseShow; }		

	float soundVolume() { return _soundVolume; }	

	int cameraDevice()	{ return _cameraDevice;	}	
	int cameraDevice2()	{ return _cameraDevice2;	}	
	bool cameraPS3Eye()	{ return _cameraPS3Eye;    } 
	bool cameraKinect()	{ return _cameraKinect;    }

	int cameraFrameRate()	{ return _cameraFrameRate; }
	int cameraMirror()	{ return _cameraMirror; }	
	int cameraPhysW()	{ return _cameraPhysW; }	
	int cameraPhysH()	{ return _cameraPhysH; }
	int cameraGrabW()	{ return _cameraGrabW; }	
	int cameraGrabH()	{ return _cameraGrabH; }
	int cameraProcessW()	{ return _cameraProcessW; }	
	int cameraProcessH()	{ return _cameraProcessH; }
	int cameraOptFlowW()	{ return _cameraOptFlowW; }	
	int cameraOptFlowH()	{ return _cameraOptFlowH; }

	float fadeDuration()	{ return _fadeDuration;	}	

	float introDuration()	{ return _introDuration; }	
	string introImageFile()		{ return _introImageFile; }		

	bool durationControl()	{ return _durationControl; }	
	void setDurationControl( bool durationControl )	{	_durationControl = durationControl; }

	static const int ResizeModeCrop = 0;
	static const int ResizeModeFit = 1;


private:
	ofxIniSettings _defaultIni, _userIni;

	ofxArgs _args;					
	ofAppGlutWindow *_pWindow;		

	int _w;
	int _h;
	float _screenPhysW;
	float _screenPhysH;

	int _rotate;		
	int _fullScreen;		
	int _mirror;		
	float	_fadeDuration;	

	float _introDuration; 	
	string _introImageFile;		
	ofImage _introImage;		


	int _resizeMode;	//0 - crop, 1 - fit

	int _renderW;
	int _renderH;
	int _renderAngle;
	pbScreenFormat _screenFormat;

	float _runDurationSec;		


	int _mouseShow;


	float _soundVolume;	


	int _cameraDevice;
	int _cameraDevice2;
	int _cameraPS3Eye;
	int _cameraKinect;
	int _cameraFrameRate;
	int _cameraMirror;
	int _cameraPhysW;
	int _cameraPhysH;
	int _cameraGrabW;
	int _cameraGrabH;
	int _cameraProcessW;
	int _cameraProcessH;
	int _cameraOptFlowW;
	int _cameraOptFlowH;

	//
	bool _durationControl;

	bool _printFps;
	void doPrintFps();	

	float _updateStart;
	float _updateEnd;

};

extern RunInfo runInfo;

#endif

