#pragma once

#include "cv.h"
#include "CLEyeMulticam.h"
#include "pbVideoCallback.h"

using namespace cv;
using namespace std;

// Camera capture class
class CLEyeCapture
{
public:
	CLEyeCapture(CLEyeCameraResolution resolution, CLEyeCameraColorMode mode, int fps, bool mirrorX);
	void setCallback( pbVideoGrabberCallbackType callback, void *param );
	bool Start();
	void Stop();
	void Run();
	static DWORD WINAPI CaptureThread(LPVOID instance);


	IplImage *grabFrame();		//Быстрая операция. Просто сбрасывает frameIsNew. Картинка - хранится потоком, не удалять и не менять.
	bool isFrameNew();

	bool setParameter( CLEyeCameraParameter param, int value );
	int getParameter( CLEyeCameraParameter param );

private:
	GUID _cameraGUID;
	CLEyeCameraInstance _cam;
	CLEyeCameraColorMode _mode;
	CLEyeCameraResolution _resolution;
	int _fps;
	HANDLE _hThread;
	bool _running;
	double measuredCnt;
	bool _isColor;

	IplImage *pCapImage;
	bool _isFrameNew;

	bool _mirrorX;

	pbVideoGrabberCallbackType _callback;
	void *_callbackParam;

};
