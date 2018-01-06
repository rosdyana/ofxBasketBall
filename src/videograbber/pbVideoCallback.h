#pragma once

#include "cv.h"

using namespace cv;
using namespace std;

typedef void (* pbVideoGrabberCallbackType)(const Mat &image, void *param) ;