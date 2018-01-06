#include "FindContours.h"

//---------------------------------------------------------
void FindContours::setup( int storageSize )
{

	_storageSize = storageSize;
	_pnt.resize( _storageSize );		
	_beginIndex.resize( _storageSize );
	
	_tempImage = 0;
	_cv_storage = 0;
	reset();
}

//---------------------------------------------------------
void FindContours::reset()			
{
	_size = 0;					
	_pntCount = 0;				
}

//---------------------------------------------------------
void FindContours::update( IplImage *grayImage, const FindContoursData &data )	
{
	reset();

	if ( grayImage == 0 ) return;
	
	if( _cv_storage == NULL ){
		_cv_storage = cvCreateMemStorage(0);
	} else {
		cvClearMemStorage( _cv_storage );
	}

	/*if ( _tempImage == 0 ) {
		_tempImage = new ofxCvGrayscaleImage( grayImage );			
	}
	else *_tempImage = grayImage;
*/

	IplImage *image = grayImage; //_tempImage->getCvImage();
	CvSeq* contours = 0;
	//cvThreshold( g_gray, g_gray, g_thresh, 255, CV_THRESH_BINARY );
	cvFindContours( image, _cv_storage, &contours,
		sizeof(CvContour),
		CV_RETR_EXTERNAL, //CV_RETR_LIST,
		CV_CHAIN_APPROX_NONE //CV_CHAIN_APPROX_SIMPLE
	);

	convert( contours, data );
	
	if ( data.debugDraw ) {
		IplImage *image = grayImage; //grayImage.getCvImage();
		cvZero( image );
		if( contours ){
			cvDrawContours(
			image,
			contours,
			cvScalarAll(255),
			cvScalarAll(255),
			100 );
		}
	}


	// Free the storage memory.
	// Warning: do this inside this function otherwise a strange memory leak
	//if( contour_storage != NULL ) { cvReleaseMemStorage(&contour_storage); }
	//if( storage != NULL ) { cvReleaseMemStorage(&storage); }

}
//---------------------------------------------------------
void FindContours::convert( CvSeq* contours, const FindContoursData &data )
{
	CvSeq* contour_ptr = contours;

	// put the contours from the linked list, into an array for sorting
	while( (contour_ptr != NULL) ) {

		int length	= cvArcLength( contour_ptr );
		if ( length >= data.minLength && length <= data.maxLength ) {

			CvPoint           pt;
			CvSeqReader       reader;
			cvStartReadSeq( contour_ptr, &reader, 0 );

			
			if ( _size < _storageSize && _pntCount < _storageSize ) {	
				int beginIndex = _pntCount;

				int n = contour_ptr->total;
				int reducer = 0;
	    		for( int j=0; j < n; j++ ) {
					CV_READ_SEQ_ELEM( pt, reader );
					//прореживание
					if ( (reducer++) % data.reduce == 0 || j == n-1 ) {
						if ( _pntCount < _storageSize ) {		
							_pnt[ _pntCount++ ] = ofPoint((float)pt.x, (float)pt.y);
						}
					}
		            //blobs[i].pts.push_back( ofPoint((float)pt.x, (float)pt.y) );
				}
				//blobs[i].nPts = blobs[i].pts.size();
				_beginIndex[ _size++ ] = beginIndex;
			}
			

		}
		contour_ptr = contour_ptr->h_next;
	}
	//cout << "find countour points = " << _pntCount << endl;
}

//---------------------------------------------------------
int FindContours::size()				
{
	return _size;
}

//---------------------------------------------------------
FCContour FindContours::item( int i )
{
	FCContour c;
	c.size = ( i < _size - 1 ) ? ( _beginIndex[ i + 1 ] - _beginIndex[ i ] ) : ( _pntCount - _beginIndex[ i ] );
	c.pnt = &_pnt[ _beginIndex[ i ] ];
	return c;
}

//---------------------------------------------------------
