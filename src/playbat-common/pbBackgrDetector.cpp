#include "pbBackgrDetector.h"
#include "highgui.h"

//-------------------------------------------------------------------------
pbBackgrDetector::pbBackgrDetector(void)
{
	bg_model = 0;
}

//-------------------------------------------------------------------------
pbBackgrDetector::~pbBackgrDetector(void)
{
	close();
}

//-------------------------------------------------------------------------
void pbBackgrDetector::setup()
{
	free();
}

//-------------------------------------------------------------------------
void pbBackgrDetector::close()
{
	free();
}

//-------------------------------------------------------------------------
void pbBackgrDetector::free()
{
	if ( bg_model ) {
		cvReleaseBGStatModel( &bg_model );	//�� �� �������
	}
}

//-------------------------------------------------------------------------
void pbBackgrDetector::updateCamera( float dt, const Mat &image, bool debugDraw )
{
	_image = image.clone();
	IplImage img = IplImage(_image);

	bool update_bg_model = false;

	if(!bg_model) {
        //create BG model
        //bg_model = cvCreateGaussianBGModel( &img );		//����� �������, �� ������� ������
		//CvFGDStatModelParams parameters	- ����� ������ ���������
        bg_model = cvCreateFGDStatModel( &img );			//������, �� ���������
		update_bg_model = true;
//      continue;
    }
        
//      double t = (double)cvGetTickCount();
        cvUpdateBGStatModel( &img, bg_model, update_bg_model ? -1 : 0 );
//      t = (double)cvGetTickCount() - t;
//      printf( "%d. %.1f\n", fr, t/(cvGetTickFrequency()*1000.) );

		if ( debugDraw ) {
			cvShowImage("BG", bg_model->background);
			cvShowImage("FG", bg_model->foreground);
		}
}

//-------------------------------------------------------------------------
void pbBackgrDetector::reinit()					//������������� �������� ������������� ���� (��������� �� ������� ������ �����
{
	free();
}

//-------------------------------------------------------------------------
void pbBackgrDetector::update( float dt )
{

}

//-------------------------------------------------------------------------
const Mat &pbBackgrDetector::foreground()	//��������� ������� ��������� �����
{
	if ( bg_model->foreground ) {
		_foreground = Mat(	bg_model->foreground );
	}
	return _foreground;
}

//-------------------------------------------------------------------------
const Mat &pbBackgrDetector::background()	//������� ���
{
	if ( bg_model->background ) {
		_background = Mat(	bg_model->background );
	}
	return _background;
}

//-------------------------------------------------------------------------

 