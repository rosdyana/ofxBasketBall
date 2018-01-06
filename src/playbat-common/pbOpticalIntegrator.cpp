#include "pbOpticalIntegrator.h"

#include "highgui.h"
#include "pcvConverter.h"

//----------------------------------------------------------------------
void pbOpticalIntegrator::setup( int historyLength )
{	
	_n = historyLength;						//длина истории
	_optPos = 0;
	_optFrame.resize( _n );
	_optFlow.resize( _n );
	fill( _optFrame.begin(), _optFrame.end(), -1 );

}

//----------------------------------------------------------------------
void pbOpticalIntegrator::update( const Mat &image, int frame, bool debugDraw )
{	
	//_videoSmooth.updateCamera( dt, image, debugDraw );

	Mat img = image; //_videoSmooth.image();
	Mat imgFlow = img;
	//imgFlow = img.clone();
	//resize( img, imgFlow, cv::Size(), 0.5, 0.5 );
	cvtColor( imgFlow, _currentGray, CV_BGR2GRAY );
	if ( _prevGray.empty() ) {
		_prevGray = _currentGray.clone();
	}

	{ 
		//Оптический поток
		double pyrScale		=	0.5;
		int levels			=	3; //5;
		int winsize			=	3; //5; //7; //The averaging window size; The larger values increase the algorithm robustness to image noise and give more chances for fast motion detection, but yield more blurred motion field
		int iterations		=	1; //3;
		int polyN			=	5; //7; //Size of the pixel neighborhood used to find polynomial expansion in each pixel.
		double polySigma	=	1.1; //Standard deviation of the Gaussian that is used to smooth derivatives that are used as a basis for the polynomial expansion. For polyN=5 you can set polySigma=1.1 , for polyN=7 a good value would be polySigma=1.5
		int flags = 0;
		//| OPTFLOW_FARNEBACK_GAUSSIAN;			

		calcOpticalFlowFarneback(_prevGray, _currentGray, _flow, pyrScale, levels, winsize, iterations, polyN, polySigma, flags);
		
	}

	_prevGray = _currentGray.clone();

	//обнуление края детектора движения, чтоб не шумел
	const float kFlowBorderZero=0.05;
	if ( kFlowBorderZero > 0 )	{
		int w = _flow.size().width;
		int h = _flow.size().height;
		int border = kFlowBorderZero * w;
		_flow(cv::Rect(0, 0, w, border)).setTo( cv::Scalar(0,0) );
		_flow(cv::Rect(0, h - border, w, border)).setTo( cv::Scalar(0,0) );
		_flow(cv::Rect(0, 0, border, h)).setTo( cv::Scalar(0,0) );
		_flow(cv::Rect(w-border, 0, border, h)).setTo( cv::Scalar(0,0) );
	}


	//Обновляем историю
	_optFrame[ _optPos % _n ] = frame;
	_optFlow[ _optPos % _n ] = _flow.clone();
	_optPos++;
	

	//Рисуем
	if ( debugDraw ) {
		Mat absFlow;
		pcvConverter::opticalFlowToAbs( _flow, absFlow, 4.0 );
		Mat absFlowBig;
		resize( absFlow, absFlowBig, cv::Size(), 2, 2 );
		imshow( "flow", absFlowBig );
	}

}

//----------------------------------------------------------------------
//вычисление движения прямоугольника от frame0 до frame1.
//оптический поток усредняется по всему прямоугольнику
//поэтому, возможно, в задачах стоит передавать уменьшенный прямоугольник
ofRectangle pbOpticalIntegrator::integrateRect( const ofRectangle &rect0, float w, float h, 
											   int frame0, int frame1 )
{
	//cout << "integrate" << endl;
	if ( !_optFlow[0].empty() ) {
		cv::Size size = _optFlow[0].size();
		cv::Rect rectFlow( 0, 0, size.width, size.height );
		float scaleX = 1.0 * size.width / w;
		float scaleY = 1.0 * size.height / h;
		
		ofRectangle rect( rect0.x * scaleX, rect0.y * scaleY, rect0.width * scaleX, rect0.height * scaleY ); 
			
		//двигаем точку, сколько она прошла за прошедшие кадры
		for (int i = max(frame0 + 1, 0); i <= frame1; i++ ) {			
			//ищем подходящий индекс. TODO можно ускорить)
			int k = -1;
			for (int j=0; j<_n; j++) {
				if ( _optFrame[j] == i ) { k = j; break; }
			}
			if ( k >= 0 ) {		
				//применяем k-й поток
				cv::Rect rect1 = cv::Rect(rect.x, rect.y, rect.width, rect.height) & rectFlow;
				if ( rect1.width > 0 && rect1.height > 0 ) {
					cv::Scalar m = cv::mean( _optFlow[k](rect1) );	
					rect.x += m[0];
					rect.y += m[1];

					//защита от выхода середины прямоугольника за границы экрана
					float cx = rect.x + rect.width / 2;
					cx = ofClamp( cx, 0, rectFlow.width );
					float cy = rect.y + rect.height / 2;
					cy = ofClamp( cy, 0, rectFlow.height );

					rect.x = cx - rect.width / 2;
					rect.y = cy - rect.height / 2;
				}
			}
			else {
				//cout << "WARNING Optical flow dropped frame " << i << endl;
			}
		}
		ofRectangle res( rect.x / scaleX, rect.y / scaleY, rect.width / scaleX, rect.height / scaleY );


		//cout << "integrate ok" << endl;

		return res;
	}
	else return rect0;

}

//----------------------------------------------------------------------

