#include "pbBinaryDenoiser.h"
#include <highgui.h>

//#define BINARYDENOISER_SHOW_WINDOWS

//------------------------------------------------------------------------------
void pbBinaryDenoiser::setup(int w, int h)
{
    _w = w;
    _h = h;
    mem_storage = NULL;
    contours = NULL;

    CvSize sz = cvSize(w, h);
    _mask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
    cvZero(_mask);

    _drawImageReady = false;

}

//------------------------------------------------------------------------------
void pbBinaryDenoiser::morphology(int openClose, int closeOpen)
{
    if (openClose) {
        cvMorphologyEx(_mask, _mask, 0, 0, CV_MOP_OPEN, openClose);
        cvMorphologyEx(_mask, _mask, 0, 0, CV_MOP_CLOSE, openClose);
    }
    if (closeOpen) {
        cvMorphologyEx(_mask, _mask, 0, 0, CV_MOP_CLOSE, closeOpen);
        cvMorphologyEx(_mask, _mask, 0, 0, CV_MOP_OPEN, closeOpen);
    }

}

//------------------------------------------------------------------------------

void pbBinaryDenoiser::denoiseUsingMorphology(IplImage *inputMask, int openClose, int closeOpen)
{
    cvCopy(inputMask, _mask);
    morphology(openClose, closeOpen);
    _drawImageReady = false;
}


//------------------------------------------------------------------------------
void pbBinaryDenoiser::denoiseUsingFloodFill(IplImage *inputMask, int openClose, int closeOpen,
        int blackAreaThreshold, int whiteAreaThreshold)
{
    cvCopy(inputMask, _mask);
    morphology(openClose, closeOpen);

    IplImage *mask = _mask;
    const int black = 0;
    const int white = 255;
    const int blackTemp = 10;
    const int whiteTemp = 250;

    for (int y = 0; y < _h; y++) {
        for (int x = 0; x < _w; x++) {
            int value = CV_IMAGE_ELEM(mask, uchar, y, x);
            if (value == 0) {
                CvConnectedComp cc;
                cvFloodFill(mask, cvPoint(x, y), cvScalar(blackTemp), cvScalar(0), cvScalar(0), &cc, 8);
                if (cc.area < blackAreaThreshold) {
                    cvFloodFill(mask, cvPoint(x, y), cvScalar(white), cvScalar(0), cvScalar(0), &cc, 8);
                }

            }
        }
    }
    for (int y = 0; y < _h; y++) {
        for (int x = 0; x < _w; x++) {
            int value = CV_IMAGE_ELEM(mask, uchar, y, x);
            if (value == 255) {
                CvConnectedComp cc;
                cvFloodFill(mask, cvPoint(x, y), cvScalar(whiteTemp), cvScalar(0), cvScalar(0), &cc, 8);
                if (cc.area < whiteAreaThreshold) {
                    cvFloodFill(mask, cvPoint(x, y), cvScalar(black), cvScalar(0), cvScalar(0), &cc, 8);
                }

            }
        }
    }

    cvThreshold(mask, mask, 128, 255, CV_THRESH_BINARY);

    _drawImageReady = false;
}

//------------------------------------------------------------------------------
void pbBinaryDenoiser::denoiseUsingContours(IplImage *inputMask, int openClose, int closeOpen,
        float minPerimeter, bool approx, double approxLevel)
{
    const int poly1_hull0 = 1;

    cvCopy(inputMask, _mask);
    morphology(openClose, closeOpen);

    //if ( findContours )
    {
        //FIND CONTOURS AROUND ONLY BIGGER REGIONS
        //
        if (mem_storage == NULL) {
            mem_storage = cvCreateMemStorage(0);
        } else {
            cvClearMemStorage(mem_storage);
        }
        CvContourScanner scanner = cvStartFindContours(
                                       _mask,
                                       mem_storage,
                                       sizeof(CvContour),
                                       CV_RETR_EXTERNAL,
                                       (approx) ? CV_CHAIN_APPROX_NONE : CV_CHAIN_APPROX_SIMPLE
                                   );

        CvSeq *c;
        int numCont = 0;
        while ((c = cvFindNextContour(scanner)) != NULL) {
            double len = cvContourPerimeter(c);
            //Get rid of blob if its perimeter is too small:
            //
            if (len < minPerimeter) {
                cvSubstituteContour(scanner, NULL);
            } else {
                // Smooth its edges if its large enough
                //
                if (approx) {

                    CvSeq *c_new;
                    if (poly1_hull0) {
                        // Polygonal approximation
                        //
                        c_new = cvApproxPoly(
                                    c,
                                    sizeof(CvContour),
                                    mem_storage,
                                    CV_POLY_APPROX_DP,
                                    approxLevel,
                                    0
                                );
                    } else {
                        // Convex Hull of the segmentation
                        //
                        c_new = cvConvexHull2(
                                    c,
                                    mem_storage,
                                    CV_CLOCKWISE,
                                    1
                                );
                    }
                    cvSubstituteContour(scanner, c_new);
                }
                numCont++;
            }
        }
        contours = cvEndFindContours(&scanner);

        // PAINT THE FOUND REGIONS BACK INTO THE IMAGE
        //
        cvZero(_mask);
        for (c = contours; c != NULL; c = c->h_next) {
            cvDrawContours(
                _mask,
                c,
                CV_RGB(0xff, 0xff, 0xff),
                CV_RGB(0xff, 0xff, 0xff),
                -1,
                CV_FILLED,
                8
            );
        }
    }


    _drawImageReady = false;

#ifdef BINARYDENOISER_SHOW_WINDOWS
    namedWindow("pbBinaryDenoiser", CV_WINDOW_AUTOSIZE);
    imshow("pbBinaryDenoiser", mask);
#endif

}

//------------------------------------------------------------------------------
void pbBinaryDenoiser::draw(float x, float y, float w, float h)
{
    if (!_mask || _mask->width == 0) {
        return;
    }

    if (!_drawImageReady) {
        _drawImageReady = true;
        if (!_drawImage.bAllocated) {
            _drawImage.allocate(_w, _h);
        }
        if (_drawImage.width != _w || _drawImage.height != _h) {
            _drawImage.resize(_w, _h);
        }
        _drawImage.setFromPixels((unsigned char *) _mask->imageData, _w, _h);

    }
    if (w == 0 && h == 0) {
        _drawImage.draw(x, y);
    } else {
        _drawImage.draw(x, y, w, h);
    }

}

//------------------------------------------------------------------------------
