#pragma once

//������ � Kinect

//���������:
//1. ��������� _Kinect � ����� ���� openFrameworks, http://ajaxorg.posterous.com/kinect-driver-for-windows-prototype
//2. �������� Kinect ��� ������������ ������ � �������
//3. ��������� � ��� �/C++-Code Generation-Runtime library = 
//	��� Release - Multithreaded Dll (��� ��� ������ openFrameworks �����)
//  ��� Debug - Multithreaded Debug Dll
//4. � ���������� ������� ��������� ���������� ������ ������� �� ������� Kinect



#include "cv.h"
#include "ofMain.h"

using namespace cv;
using namespace std;

extern Mat _pbKinectNullMat;

class pbKinect

{
public:
	//��������� ����������
	static const int KinectDepthRangeLow = 1;         //[KinectDepthLow, KinectDepthHigh - 1] - �������� ���������� �������� �������
	static const int KinectDepthRangeHigh = 0x07ff;	

	static const int KinectFrameWidth	= 640;
	static const int KinectFrameHeight	= 480;


	//��������������� �������
	static void KinectDepthToWorld(float &x, float &y, float &z) {};
	static void KinectWorldToRGBSpace(float &x, float &y, float z) {};
	static float KinectDepthValueToZ(unsigned short Depth) { return 0; };
	static unsigned short KinectZToDepthValue( float z ) { return 0; };
	static bool KinectIsDepthValid(unsigned short Depth) { return 0; };

	//��������� ����� ���������� ��������
	static void KinectDepthToValidMask( const Mat &depth, Mat &validMask ) {};

	//�������������� � floating-point
	static void KinectDepthToFloatDepth( const Mat &depth, Mat &floatDepth ) {};
	static const double KinectFloatRangeHigh() { return 0.84; }		//�������� ��� ������������ ��������

public:

	void setup() {};
	void close() {};
	int cameraCount() { return 0; };
	bool startCamera(	int device,
						int w, int h,		
						int fps,														
						bool color,	//1- ��� 4-�������						
						bool mirrorX						
						) { return 0; };		

	bool stopCamera() { return 0; };

	//Grab
	bool isFrameNew() { return 0; };
	Mat &grabColorFrameCV() { return _pbKinectNullMat; };	//�������. �������� �� ������� � �� ������
	Mat &grabDepthFrameCVRaw() { return _pbKinectNullMat; };	//�������. �������� �� ������� � �� ������
	Mat &grabDepthFrameCVFloat() { return _pbKinectNullMat; };	//�������� �������, �������� 0..1
	Mat &grabDepthFrameCV8bit() { return _pbKinectNullMat; };	//�������� ������, ��� ������������

	

private:
	Mat _frame;	
	Mat _depthFrame;
	Mat _depthFrameFloat;
	Mat _depthFrame8bit;

	void *_cam;
	bool _mirrorX;




};
