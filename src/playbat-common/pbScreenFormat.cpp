#include "pbScreenFormat.h"

//--------------------------------------------------------------
string screenFormatToStr( pbScreenFormat format ) 
{
	string res = "";
	if ( format == pbScreenFormat_3x4 ) res = "3x4";
	if ( format == pbScreenFormat_16x9 ) res = "16x9";
	if ( format == pbScreenFormat_16x10 ) res = "16x10";
	return res;
}

pbScreenFormat screenFormatFromStr( const string &str ) 
{
	pbScreenFormat format = pbScreenFormat_3x4;
	if ( str == "3x4" ) format = pbScreenFormat_3x4;
	if ( str == "16x9" ) format = pbScreenFormat_16x9;
	if ( str == "16x10" ) format = pbScreenFormat_16x10;
	return format;
}

//--------------------------------------------------------------
