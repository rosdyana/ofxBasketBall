#include "pcvConverter.h"
#include <highgui.h>


//----------------------------------------------------------------
bool pcvConverter::convertGray( const cv::Mat &source, ofImage &image, float koef )
{
	int w = source.cols;
	int h = source.rows;

	Mat dest;
	source.convertTo(dest, CV_8UC1, koef);
	image.setFromPixels( (unsigned char*)IplImage(dest).imageData, w, h, OF_IMAGE_GRAYSCALE );
	//imshow("convert", dest);

	return true;
}

//----------------------------------------------------------------
bool pcvConverter::matToOfImage3( const cv::Mat &sourceC134, ofImage &image )
{
	if ( sourceC134.empty() ) {
		image.clear();
	}
	else {
		//если float - преобразуем в unsigned char
		bool isFloat = ( sourceC134.type() == CV_32F );
		Mat temp;
		if ( isFloat ) {
			sourceC134.convertTo( temp, CV_8U, 255.0 );
		}

	
		const Mat &source = (isFloat) ? temp : sourceC134;
		int w = source.size().width;
		int h = source.size().height;

		//Если одноканальная картинка
		if ( source.channels() == 1 ) {
			Mat rgb;
			cvtColor( source, rgb, CV_GRAY2BGR );
			image.setFromPixels( (unsigned char*)IplImage(rgb).imageData, w, h, OF_IMAGE_COLOR );
		}
		if ( source.channels() == 3 ) {
			Mat rgb = source.clone();	//TODO непотимально, но нужно для устранения step
			image.setFromPixels( (unsigned char*)IplImage(rgb).imageData, w, h, OF_IMAGE_COLOR );
		}
		if ( source.channels() == 4 ) {
			Mat rgb;
			//source.convertTo( rgb, CV_8UC3 );
			cvtColor( source, rgb, CV_RGBA2BGR );
			image.setFromPixels( (unsigned char*)IplImage(rgb).imageData, w, h, OF_IMAGE_COLOR );
		}	
	}

	/*if( cvImage->width == cvImage->widthStep ){
			memcpy( cvImage->imageData,  _pixels, w*h*3);
		}else{
			
 		 	for( int i=0; i < height; i++ ) {
            		memcpy( cvImage->imageData + (i*cvImage->widthStep),
 	                   _pixels + (i*w*3),
        	            width*3 );
        		}
			
		}
	*/

	return true;
}

//----------------------------------------------------------------
bool pcvConverter::ofImageToMat( ofImage &image, cv::Mat &mat )
{
	mat = Mat();
	if ( image.getImageType() == OF_IMAGE_COLOR ) {
		Mat img = Mat( cv::Size( image.getWidth(), image.getHeight() ), CV_8UC3, image.getPixels().getData() );
	}
	if ( image.getImageType() == OF_IMAGE_COLOR_ALPHA ) {
		Mat img = Mat( cv::Size( image.getWidth(), image.getHeight()), CV_8UC4, image.getPixels().getData());
	}
	if ( image.getImageType() == OF_IMAGE_GRAYSCALE ) {
		Mat img = Mat( cv::Size( image.getWidth(), image.getHeight()), CV_8UC1, image.getPixels().getData());
	}

	return true;
}


//----------------------------------------------------------------
bool pcvConverter::convertRGB( const cv::Mat &source34UC, ofImage &image )
{
	return matToOfImage3( source34UC, image );
}

//-----------------------------------------------------------------

void pcvConverter::convertRGB2YUV( int r, int g, int b, int &y, int &u, int &v )
{
	Mat rgbColor( 1, 1, CV_8UC3 );
	rgbColor.setTo( cv::Scalar( r, g, b ) );
	Mat yuvColor;
	cvtColor( rgbColor, yuvColor, CV_RGB2YCrCb );	
	vector<Mat> yuvSplitColor;
	split( yuvColor, yuvSplitColor );
	
	y = yuvSplitColor[0].at<unsigned char>(0,0);
	u = yuvSplitColor[1].at<unsigned char>(0,0);
	v = yuvSplitColor[2].at<unsigned char>(0,0);
	//cout << "color " << _param.colorR << " " << _param.colorG << " " << _param.colorB << endl;
	//cout << "templ " << templYUV[0] << " " << templYUV[1] << " " << templYUV[2] << endl;

}

//----------------------------------------------------------------
void pcvConverter::opticalFlowToAbs( const Mat &flow, Mat &result, float scale )
{
	vector<Mat> flow_planes;
	split( flow, flow_planes );
	pow(flow_planes[0], 2, flow_planes[0]);
	pow(flow_planes[1], 2, flow_planes[1]);
	
	result = flow_planes[0] + flow_planes[1];
	result = result * scale;

}


//----------------------------------------------------------------
ofxVec2f pcvConverter::fromCv( const cv::Point &p )
{
	return ofxVec2f( p.x, p.y );
}

cv::Point pcvConverter::toCv( const ofxVec2f &p )
{
	return cv::Point( p.x, p.y );
}


//----------------------------------------------------------------
