#include "pbMaskStabilizer.h"
#include "highgui.h"

//-----------------------------------------------------------------
pbMaskStabilizer::pbMaskStabilizer(void)
{
    _inited = false;
}

//-----------------------------------------------------------------
pbMaskStabilizer::~pbMaskStabilizer(void)
{
}

//-----------------------------------------------------------------
void pbMaskStabilizer::setup(float historySec)	//сколько должна стоять точка, чтоб ее считать сработавшей
{
    kHistorySec = historySec;
    _inited = false;
}

//-----------------------------------------------------------------
void pbMaskStabilizer::init(const Mat &mask)
{
    _size = mask.size();
    _history = Mat(_size, CV_32FC1);
    _history.setTo(cv::Scalar(0));
    _maskResult = Mat(_size, CV_8UC1);
    _maskResult.setTo(cv::Scalar(0));
}

//-----------------------------------------------------------------
void pbMaskStabilizer::update(float dt, const Mat &mask)
{
    if (!_inited) {
        _inited = true;
        init(mask);
    }
    mask.convertTo(_tempMaskFloat, CV_32FC1, 1.0 / 255.0, 0);
    cv::multiply(_history, _tempMaskFloat, _history);				//обнуляем все, которые 0
    _history += dt * _tempMaskFloat;									//добавляем ненулевые


    //imshow( "mask", mask );
    //imshow( "history", _history );
    //imshow( "maskHist", this->mask() );
}

//-----------------------------------------------------------------
Mat &pbMaskStabilizer::mask()
{
    if (_inited) {
        threshold(_history, _tempMaskFloat, kHistorySec, 255, CV_THRESH_BINARY);
        _tempMaskFloat.convertTo(_maskResult, CV_8UC1, 1.0 * 255);
    } else {
        cout << "ERROR: pbMaskStabilizer - not inited" << endl;
    }
    return _maskResult;	//лишь бы что-то вернуть, всё равно фактически этого не произойдет
}

//-----------------------------------------------------------------
