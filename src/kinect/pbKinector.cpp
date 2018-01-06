#include "pbKinector.h"
#include "pbKinectThread.h"

pbKinector shared_kinect;

XnBool g_bDrawCoM = false;
XnBool g_bPause = false;

XnBool g_bQuit = false;

#define SAMPLE_XML_PATH "data/openni/Sample-Scene.xml"



//------------------------------------------------------------------
void pbKinector::Start( const pbKinectorParam &param )
{
	if ( !param.threaded ) {
		cout << endl;
		cout << "!!! WARNINIG: Starting Kinect in main thread, so detection can work unstable" << endl;
		cout << endl;
		shared_kinect.setup( param );
	}
	else {		
		shared_kinectThread.start( param, true );	//! Сбивает пути!!!
	}
}


//------------------------------------------------------------------
void pbKinector::Close()
{
	if (!shared_kinect.param().threaded) {
		shared_kinect.close();
	}
	else {
		shared_kinectThread.stop();		
	}

}

//------------------------------------------------------------------
void pbKinector::Update()
{
	if ( !shared_kinect.param().threaded ) {
		shared_kinect.update();
		shared_kinect.updateDepthRGB( true );
	}
}


//------------------------------------------------------------------
#define START_CAPTURE_CHECK_RC(status, what)												\
	if (nRetVal != XN_STATUS_OK)														\
{																					\
	printf("Failed to %s: %s\n", what, xnGetStatusString(status));				\
	recordStop();	\
	return;			\
}

//return ;																	

//------------------------------------------------------------------
#define CHECK_ERRORS(status, errors, what)		\
	if (status == XN_STATUS_NO_NODE_PRESENT)	\
{										\
	XnChar strError[1024];				\
	errors.ToString(strError, 1024);	\
	printf("%s\n", strError);			\
}
//return (status);						\

//------------------------------------------------------------------
#define CHECK_RC(status, what)											\
	if (status != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(status));		\
	_starting = false;											\
	return;														\
}
//return status;													

//------------------------------------------------------------------
pbKinector::pbKinector(void)
{
	g_pRecorder = 0;
	_depthRGB = 0;

	_image = 0;
	_depth = 0;		
	_labels = 0;

	_started = false;

	_w = 640;
	_h = 480;


	_recording = false;
	_playing = false;
}

//------------------------------------------------------------------
pbKinector::~pbKinector(void)
{
	if ( _depthRGB ) {
		delete[] _depthRGB;
		_depthRGB = 0;
	}

	if ( _image ) {
		delete[] _image;
		_image = 0;
	};
	if ( _depth ) {
		delete[] _depth;
		_depth = 0;
	};
	if ( _labels ) {
		delete[] _labels;
		_labels = 0;
	};

}

//------------------------------------------------------------------
void pbKinector::setResolution(xn::MapGenerator* pGenerator, int w, int h )
{
	if (pGenerator == NULL)
	{
		cout << "pbKinector::setResolution   Stream does not exist!" << endl;
		return;
	}

	XnMapOutputMode Mode;
	pGenerator->GetMapOutputMode(Mode);
	Mode.nXRes = w; //Resolution((XnResolution)res).GetXResolution();
	Mode.nYRes = h; //Resolution((XnResolution)res).GetYResolution();
	XnStatus nRetVal = pGenerator->SetMapOutputMode(Mode);
	if (nRetVal != XN_STATUS_OK)
	{
		cout << "pbKinector::setResolution   Failed to set resolution: " << w << " x " << h << endl;
	}
}

//------------------------------------------------------------------
void pbKinector::setFPS(xn::MapGenerator* pGenerator, int fps)
{
	if (pGenerator == NULL)
	{
		cout << "pbKinector::setFPS   Stream does not exist!" << endl;
		return;
	}

	XnMapOutputMode Mode;
	pGenerator->GetMapOutputMode(Mode);
	Mode.nFPS = fps;
	XnStatus nRetVal = pGenerator->SetMapOutputMode(Mode);
	if (nRetVal != XN_STATUS_OK)
	{
		cout << "pbKinector::setFPS   Failed to set FPS: " << fps << endl;
	}
}

//------------------------------------------------------------------
void pbKinector::setup( const pbKinectorParam &param )
{
	_starting = true;

	_param = param;
	bool defaultParam = (_param.camera == -1 && _param.w == -1 && _param.h == -1);
	if ( !defaultParam ) {
		_w = _param.w;
		_h = _param.h;
	}
	

	if ( _param.rgbImage )		_image = new unsigned char[ getWidth() * getHeight() * 3 ];
	if ( _param.depthImage )	_depth = new unsigned short[ getWidth() * getHeight() ];
	if ( _param.labelImage )	_labels = new unsigned short[ getWidth() * getHeight() ];
	_depthRGB = new unsigned char[ getWidth() * getHeight()* 3 ];




	cout << endl;
	cout << "------------------------------------" << endl;
	cout << "Starting Kinect " << _w << " x " << _h << " ";
	if ( defaultParam ) {
		cout << "Load from xml" << endl;
	}
	else {
		cout << "Set device=" << _param.camera << " fps=" << _param.fps;
	}
	if ( _param.threaded ) cout << " Threaded"; else cout << " No threaded";
	cout << endl;
	cout << endl;

	XnStatus status = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	//Мотор
	_motor.setup();
	_motor.setLed( 4 );

	

	//Контекст
	if ( defaultParam ) {
		//Из xml
		status = context.InitFromXmlFile(SAMPLE_XML_PATH);
		CHECK_ERRORS(status, errors, "InitFromXmlFile");
		CHECK_RC(status, "InitFromXml");
	}
	else {
		//Выбор из настроек - реализовано только для Depth
		status = context.Init();
		xn::NodeInfoList list;
		printf("depth devices:\n");
		status = context.EnumerateProductionTrees(XN_NODE_TYPE_DEPTH, NULL, list, NULL);
		if (status != XN_STATUS_OK) {
			printf("enumerating depth generators failed. Reason: %s\n",	xnGetStatusString (status));
			_starting = false;											\
			return;
		} else {
			for (xn::NodeInfoList::Iterator nodeIt = list.Begin(); nodeIt != list.End(); ++nodeIt) {
				const xn::NodeInfo& info = *nodeIt;
				const XnProductionNodeDescription& description = info.GetDescription();
				printf("depth: vendor %s name %s, instance %s, creation %s\n", description.strVendor, description.strName, info.GetInstanceName(), info.GetCreationInfo());
			}
		} 

		//Создаем нужное дерево
		xn::NodeInfoList::Iterator nodeIt = list.Begin(); 
		for (int i=0; i<_param.camera && nodeIt != list.End(); i++) {
			nodeIt++;										//Устанавливаем нужное устройство
		}
		if ( nodeIt == list.End() ) {			
			cout << "ERROR: No kinect device " << _param.camera << "!!" << endl;
			_starting = false;											\
			return;
		}

		xn::NodeInfo& tree = *nodeIt;					
		status = context.CreateProductionTree( tree );		//Запускаем контекст
	}

	//depth
	if ( _param.depthImage ) {
		status = context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
		CHECK_RC(status, "Find depth generator");

		//Устанавливаем разрешение и fps
		if ( _param.w >= 0 && _param.h >= 0 ) {
			setResolution( &g_DepthGenerator, _param.w, _param.h );
		}		
		if ( _param.fps >= 0 ) {
			setFPS( &g_DepthGenerator, _param.fps );
		}		
	}

	
	//rgb
	if ( _param.rgbImage ) {
		status = context.FindExistingNode(XN_NODE_TYPE_IMAGE, image_generator);
		CHECK_RC(status, "Find image generator");
	}

	//синхронизация depth и rgb
	if ( _param.depthImage && _param.rgbImage ) {
		if (g_DepthGenerator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
			XnStatus result = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(image_generator);
			CHECK_RC(result, "Register viewport");
		}
	}

	if ( _param.labelImage ) {
		status = context.FindExistingNode(XN_NODE_TYPE_SCENE, g_SceneAnalyzer);
		CHECK_RC(status, "Find scene analyzer");
	}

	status = context.StartGeneratingAll();
	CHECK_RC(status, "StartGenerating");


	//xn::DepthMetaData depthMD;
	//g_DepthGenerator.GetMetaData(depthMD);

	//	_w = depthMD.XRes(); //320;
	//	_h = depthMD.YRes(); //240;


	_motor.setLed( 1 );

	_started = true;


	_recording = false;
	_playing = false;

	_starting = false;

}

//------------------------------------------------------------------
void pbKinector::update()
{

	if ( _started ) {

		if (!g_bPause)
		{
			// Read next available data
			context.WaitAndUpdateAll();

			//Сразу сохраняем изображения
			if ( _param.depthImage ) {
				xn::DepthMetaData depthMD;
				g_DepthGenerator.GetMetaData(depthMD);
				memcpy( _depth, depthMD.Data(), getWidth() * getHeight() * sizeof( _depth[0] ) );
			}

			if ( _param.rgbImage ) {
				xn::ImageMetaData imageMD;
				image_generator.GetMetaData(imageMD);
				memcpy( _image, imageMD.Data(), getWidth() * getHeight() * 3 );
			}

			if ( _param.labelImage ) {
				xn::SceneMetaData sceneMD;
				g_SceneAnalyzer.GetMetaData(sceneMD);
				memcpy( _labels, sceneMD.Data(), getWidth() * getHeight() * sizeof( _labels[0] ) );
			}

		}

		// Process the data
		//DRAW
		//g_DepthGenerator.GetMetaData(depthMD);
		//g_SceneAnalyzer.GetMetaData(sceneMD);
		//image_generator.GetMetaData(imageMD);

		//DrawDepthMap(depthMD, sceneMD, imageMD);
	}
}

//------------------------------------------------------------------
void pbKinector::close()
{	
	//Мотор
	_motor.setLed( 0 );
	_motor.close();

	if ( started() ) {
		recordStop();
		context.Shutdown();
	}

	_started = false;

}

//------------------------------------------------------------------
int pbKinector::getWidth()
{
	return _w;
}

//------------------------------------------------------------------
int pbKinector::getHeight()
{
	return _h;
}

//------------------------------------------------------------------
/*unsigned char *pbKinector::getImagePixelsConst()
{
	//xn::ImageMetaData imageMD;
	//image_generator.GetMetaData(imageMD);
	//return const_cast< unsigned char *>(imageMD.Data());
	return _image;

}*/

//------------------------------------------------------------------
XnFloat Colors[][3] =
{
	{1,1,1},	//{0,1,1},
	{0,0,1},
	{0,1,0},
	{1,1,0},
	{1,0,0},
	{1,.5,0},
	{.5,1,0},
	{0,.5,1},
	{.5,0,1},
	{1,1,.5},
	{1,1,1}
};

XnUInt32 nColors = 10;

#define MAX_DEPTH 10000
float g_pDepthHist[MAX_DEPTH];

//------------------------------------------------------------------
void pbKinector::updateDepthRGB( bool drawLabels )
{
	if (_started && _param.depthImage ) {
		drawLabels = drawLabels && _param.labelImage;


		xn::DepthMetaData depthMD;
		g_DepthGenerator.GetMetaData(depthMD);
		const XnDepthPixel* pDepth = depthMD.Data();

		int imageW = depthMD.XRes();
		int imageH = depthMD.YRes();

		unsigned int nValue = 0;
		unsigned int nHistValue = 0;
		unsigned int nIndex = 0;
		unsigned int nX = 0;
		unsigned int nY = 0;
		unsigned int nNumberOfPoints = 0;
		XnUInt16 g_nXRes = depthMD.XRes();
		XnUInt16 g_nYRes = depthMD.YRes();

		// Calculate the accumulative histogram
		memset(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));
		for (nY=0; nY<g_nYRes; nY++) {
			for (nX=0; nX<g_nXRes; nX++) {
				nValue = *pDepth;
				if (nValue != 0)	{
					g_pDepthHist[nValue]++;
					nNumberOfPoints++;
				}
				pDepth++;
			}
		}

		for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
		{
			g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
		}
		if (nNumberOfPoints)
		{
			for (nIndex=1; nIndex<MAX_DEPTH; nIndex++)
			{
				g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
			}
		}


		//Картинка

		xn::SceneMetaData sceneMD;
		const XnLabel* pLabels = 0;
		if ( _param.labelImage ) {
			g_SceneAnalyzer.GetMetaData(sceneMD);
			pLabels = sceneMD.Data();
		}

		// Prepare the texture map
		unsigned char* pDestImage = _depthRGB;
		pDepth = depthMD.Data();

		for (nY=0; nY<g_nYRes; nY++) {
			for (nX=0; nX < g_nXRes; nX++, nIndex++) {
				nValue = *pDepth;
				//XnLabel label = *pLabels;
				XnUInt32 nColorID = ( drawLabels ) ? (*pLabels % nColors) : nColors;

				//if (label == 0)	{
				//	nColorID = nColors;
				//}

				if (nValue != 0) {
					nHistValue = g_pDepthHist[nValue];

					pDestImage[0] = nHistValue * Colors[nColorID][0]; 
					pDestImage[1] = nHistValue * Colors[nColorID][1];
					pDestImage[2] = nHistValue * Colors[nColorID][2];

					//pDestImage[0] = 255 * Colors[nColorID][0]; 
					//pDestImage[1] = 255 * Colors[nColorID][1];
					//pDestImage[2] = 255 * Colors[nColorID][2];
				}
				else
				{
					pDestImage[0] = 0;
					pDestImage[1] = 0;
					pDestImage[2] = 0;
				}

				pDepth++;
				pLabels++;
				pDestImage+=3;
			}

			//pDestImage += (texWidth - g_nXRes) *3;
		}


	}
}


//------------------------------------------------------------------
unsigned char *pbKinector::getDepthPixelsRGBConst()
{
	return _depthRGB;
}

//------------------------------------------------------------------
unsigned short *pbKinector::getDepthPixelsConst()
{
	//xn::DepthMetaData depthMD;
	//g_DepthGenerator.GetMetaData(depthMD);	
	//return const_cast< unsigned short *>( depthMD.Data() );
	return _depth;
}

//------------------------------------------------------------------
unsigned short *pbKinector::getScenePixelsConst()
{
	if ( _labels == 0 ) {
		cout << "ERROR: Kinect - labels are disabled" << endl;
		ofSleepMillis( 5000 );
		OF_EXIT_APP( 0 );
	}

	//xn::SceneMetaData sceneMD;
	//g_SceneAnalyzer.GetMetaData(sceneMD);
	//return const_cast< unsigned short *>( sceneMD.Data() );
	return _labels;
}

//------------------------------------------------------------------
const Mat pbKinector::labelsMat16()
{
	return cv::Mat( cv::Size( _w, _h ), CV_16UC1, _labels );//.clone(); 
}

//------------------------------------------------------------------
const Mat pbKinector::depthMat16()
{
	return cv::Mat( cv::Size( _w, _h ), CV_16UC1, _depth );//.clone();
}

//------------------------------------------------------------------
const Mat pbKinector::colorMat8C3()
{
	return Mat( cv::Size( _w, _h ), CV_8UC3, _image );
}

//------------------------------------------------------------------
//Конвертация экранной точки с глубиной в мировую
ofPoint pbKinector::convertScreenToWorld( ofPoint screenPoint )
{
	if ( started() ) {
		XnPoint3D scr, world;
		scr.X = screenPoint.x;
		scr.Y = screenPoint.y;
		scr.Z = screenPoint.z;
		g_DepthGenerator.ConvertProjectiveToRealWorld( 1, &scr, &world );
		return ofPoint( world.X, world.Y, world.Z );
	}
	else {
		return ofPoint();
	}
}

//------------------------------------------------------------------
void pbKinector::convertScreenToWorld( int n, XnPoint3D *screenPoint, XnPoint3D *worldPoint )
{
	if ( n > 0 && started() ) {
		g_DepthGenerator.ConvertProjectiveToRealWorld( n, screenPoint, worldPoint );
	}
}

//------------------------------------------------------------------
//Конвертация мировых в экранные координаты

ofPoint pbKinector::convertWorldToScreen( ofPoint worldPoint )
{
	if ( started() ) {
		XnPoint3D scr, world;
		world.X = worldPoint.x;
		world.Y = worldPoint.y;
		world.Z = worldPoint.z;
		g_DepthGenerator.ConvertRealWorldToProjective( 1, &world, &scr );
		return ofPoint( scr.X, scr.Y, scr.Z );
	}
	else {
		return ofPoint();
	}
}

//------------------------------------------------------------------
void pbKinector::convertWorldToScreen( int n, XnPoint3D *worldPoint, XnPoint3D *screenPoint )
{	
	if ( n > 0 && started() ) {
		g_DepthGenerator.ConvertRealWorldToProjective( n, worldPoint, screenPoint );
	}
}


//------------------------------------------------------------------
void pbKinector::recordStart( const string &fileName)
{
	if ( !started() ) { return; }
	if ( playing() || recording() ) { return; }

	cout << "Start recording..." << endl;
	/*char recordFile[256] = {0};
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	XnUInt32 size;
	xnOSStrFormat(recordFile, sizeof(recordFile)-1, &size,
		"%d_%02d_%02d[%02d_%02d_%02d].oni",
		timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	*/

	_recording = true;

	if (g_pRecorder != NULL)
	{
		recordStop();
	}

	XnStatus nRetVal = XN_STATUS_OK;
	g_pRecorder = new xn::Recorder;

	context.CreateAnyProductionTree(XN_NODE_TYPE_RECORDER, NULL, *g_pRecorder);
	START_CAPTURE_CHECK_RC(nRetVal, "Create recorder");

	nRetVal = g_pRecorder->SetDestination(XN_RECORD_MEDIUM_FILE, fileName.c_str() ); //recordFile);
	START_CAPTURE_CHECK_RC(nRetVal, "set destination");

	if ( _param.rgbImage ) {
		nRetVal = g_pRecorder->AddNodeToRecording(image_generator, XN_CODEC_NULL);
		START_CAPTURE_CHECK_RC(nRetVal, "add node image");
	}

	if ( _param.depthImage ) {
		nRetVal = g_pRecorder->AddNodeToRecording(g_DepthGenerator, XN_CODEC_16Z ); //XN_CODEC_16Z_EMB_TABLES);
		START_CAPTURE_CHECK_RC(nRetVal, "add node depth");	
	}


}

//------------------------------------------------------------------
void pbKinector::recordStop()
{
	if ( _recording ) {
		cout << "Stop recording..." << endl;
		_recording = false;
		if (g_pRecorder != NULL)
		{
			if ( _param.depthImage ) { 
				g_pRecorder->RemoveNodeFromRecording(g_DepthGenerator);
			}
			if ( _param.rgbImage ) {
				g_pRecorder->RemoveNodeFromRecording(image_generator);
			}

			g_pRecorder->Release(); //g_pRecorder->Unref();
			delete g_pRecorder;
		}
		g_pRecorder = NULL;
	}
}

//------------------------------------------------------------------
void pbKinector::playerStart( const string &fileName)
{
	if ( playing() || recording() ) { return; }
	_playing = true;

}

//------------------------------------------------------------------
void pbKinector::playerStop()
{
	if ( _playing ) {
		_playing = false;
	}

}

//------------------------------------------------------------------
/*void DrawProjectivePoints(XnPoint3D& ptIn, int width, double r, double g, double b)
{
static XnFloat pt[3];

pt[0] = ptIn.X;
pt[1] = ptIn.Y;
pt[2] = 0;
glColor4f(r,
g,
b,
1.0f);
glPointSize(width);
glVertexPointer(3, GL_FLOAT, 0, pt);
glDrawArrays(GL_POINTS, 0, 1);

glFlush();

}*/










