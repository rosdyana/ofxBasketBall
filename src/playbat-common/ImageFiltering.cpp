#include "ImageFiltering.h"


//--------------------------------------------------------------------
void ImageFiltering::setup(int w, int h, const ImageFilteringSettings &settings)
{
    _settings = settings;
    _w = w;
    _h = h;
    _inited = false;

    filteredImage.allocate(w, h);
    filteredImage1.allocate(w, h);

    //����� ��������
    gray1.allocate(w, h);
    gray1.set(0);

}

//--------------------------------------------------------------------
void ImageFiltering::update(const ofxCvColorImage &colorImg)
{
    if (enabled()) {
        //if ( !_inited ) {
        //	filteredImage1 = colorImg;
        //	_inited = true;
        //}
        filteredImage = colorImg;

        //Sobel
        const int windowSize = 7;	//3;			//��������
        cvSobel(filteredImage.getCvImage(), filteredImage1.getCvImage(), 1, 1, windowSize);   //5); //1 );
        cvConvertScale(filteredImage1.getCvImage(), filteredImage.getCvImage(),
                       _settings.amplifyKoef,
                       0.0);

        filteredImage1.flagImageChanged();		//��� ����������� ��������



    }

}

//--------------------------------------------------------------------
ofxCvGrayscaleImage *ImageFiltering::canny(ofxCvGrayscaleImage &gray,
        const CannySettings &settings,
        bool updateTexture)
{
    if (enabled()) {
        //Canny
        //gray = colorImg;
        //IplImage* out = cvCreateImage( cvSize( cvGetSize( in ), IPL_DEPTH_8U, 1 );
        //double lowThresh	= 100;//100;				//��������
        //double highThresh	= 150; //150;			//��������
        //int aperture		= 3;					//��������
        cvCanny(gray.getCvImage(), gray1.getCvImage(), settings.lowThresh, settings.highThresh, settings.aperture);

        if (updateTexture) {
            gray1.flagImageChanged();				//��� ����������� ��������
        }

        return &gray1;
    }
    return 0;
}
//--------------------------------------------------------------------


/*
void Sobel(const Mat&  src, Mat&  dst, int ddepth, int xorder, int yorder, int ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT)�

    Calculates the first, second, third or mixed image derivatives using an extended Sobel operator
    Parameters:

        * src � The source image
        * dst � The destination image; will have the same size and the same number of channels as src
        * ddepth � The destination image depth
        * xorder � Order of the derivative x
        * yorder � Order of the derivative y
        * ksize � Size of the extended Sobel kernel, must be 1, 3, 5 or 7
        * scale � The optional scale factor for the computed derivative values (by default, no scaling is applied, see getDerivKernels() )
        * delta � The optional delta value, added to the results prior to storing them in dst
        * borderType � The pixel extrapolation method, see borderInterpolate()


void Scharr(const Mat&  src, Mat&  dst, int ddepth, int xorder, int yorder, double scale=1, double delta=0, int borderType=BORDER_DEFAULT)�

    Calculates the first x- or y- image derivative using Scharr operator
    Parameters:

        * src � The source image
        * dst � The destination image; will have the same size and the same number of channels as src
        * ddepth � The destination image depth
        * xorder � Order of the derivative x
        * yorder � Order of the derivative y
        * scale � The optional scale factor for the computed derivative values (by default, no scaling is applied, see getDerivKernels() )
        * delta � The optional delta value, added to the results prior to storing them in dst
        * borderType � The pixel extrapolation method, see borderInterpolate()

The function computes the first x- or y- spatial image derivative using Scharr operator. The call

\texttt{Scharr(src, dst, ddepth, xorder, yorder, scale, delta, borderType)}

is equivalent to

\texttt{Sobel(src, dst, ddepth, xorder, yorder, CV\_SCHARR, scale, delta, borderType)} .


*/