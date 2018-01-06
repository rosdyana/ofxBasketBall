#pragma once

#include "ofMain.h"
#include <cv.h>
#include "ofxVectorMath.h"

using namespace std;
using namespace cv;


class pcvConverter
{
public:
	static bool convertGray( const cv::Mat &source, ofImage &image, float koef = 255.0 );
	static bool matToOfImage3( const cv::Mat &sourceC134, ofImage &image );
	
	static bool convertRGB( const cv::Mat &source34UC, ofImage &image ); //Deprecated

	static bool ofImageToMat( ofImage &image, cv::Mat &mat );

	static void convertRGB2YUV( int r, int g, int b, int &y, int &u, int &v );

	static void opticalFlowToAbs( const Mat &flow, Mat &result, float scale );


	static ofxVec2f fromCv( const cv::Point &p );
	static cv::Point toCv( const ofxVec2f &p );

	template<typename Tp, typename Tp2 > 
	static cv::Rect_<float> absRectToRelRect( const cv::Rect_<Tp> &r, const cv::Rect_<Tp2> &world )
	{
		return cv::Rect_<float>( 1.0 * (r.x - world.x) / world.width,
							1.0 * (r.y - world.y) / world.height,
							1.0 * r.width / world.width,
							1.0 * r.height / world.height );
	}

	template<typename Tp, typename Tp2 >
	static cv::Rect_<Tp> relRectToAbsRect( const cv::Rect_<float> &r, const cv::Rect_<Tp2> &world )
	{
			return cv::Rect_<Tp>( world.x + r.x * world.width,
					world.y + r.y * world.height,
					r.width * world.width,
					r.height * world.height );
	}

	template<typename Tp, typename Tp2 >
	static cv::Point_<Tp> relPointToAbsPoint( const cv::Point_<float> &p, const cv::Rect_<Tp2> &world )
	{
		return cv::Rect_<Tp>( world.x + r.x * world.width,
					world.y + r.y * world.height );
	}

	template<typename Tp, typename Tp2>
	static cv::Point_<Tp> rectCenter( const cv::Rect_<Tp2> r ) 
	{
		return cv::Point_<Tp>( r.x + r.width * 0.5, r.y + r.height * 0.5 );
	}

};
