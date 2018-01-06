#pragma once

#include <string>
using namespace std;

typedef int pbScreenFormat;
const pbScreenFormat pbScreenFormat_3x4		= 0;
const pbScreenFormat pbScreenFormat_16x9	= 1;
const pbScreenFormat pbScreenFormat_16x10	= 2;

string screenFormatToStr( pbScreenFormat format );
pbScreenFormat screenFormatFromStr( const string &str );