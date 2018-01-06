#include "pbScreenGrab.h"

#include<iostream>
#include<windows.h>
using namespace std;

HDC hdcScr = 0;
HDC hdcMem = 0;
HBITMAP hbmScr = 0;


//--------------------------------------------------------------
int CaptureBMP( int grabX, int grabY, int grabW, int grabH, vector<unsigned char> &outData32 )
{
    // Source[1]

   
    BITMAP bmp;
    //int iXRes, iYRes;

    // Create a normal DC and a memory DC for the entire screen. The
    // normal DC provides a "snapshot" of the screen contents. The
    // memory DC keeps a copy of this "snapshot" in the associated
    // bitmap.
	if ( !hdcScr ) {
	    hdcScr = CreateDC(L"DISPLAY", NULL, NULL, NULL);	
		hdcMem = CreateCompatibleDC(hdcScr);
	}

    //iXRes = GetDeviceCaps(hdcScr, HORZRES);
    //iYRes = GetDeviceCaps(hdcScr, VERTRES);

    // Create a compatible bitmap for hdcScreen.
	if ( hbmScr == 0 ) {							
		hbmScr = CreateCompatibleBitmap(hdcScr, grabW, grabH);
	}
	if (hbmScr == 0) {
		cout << "Error 1" << endl;
		return 0;
	}

    // Select the bitmaps into the compatible DC.
	if (!SelectObject(hdcMem, hbmScr)) {
		cout << "Error 2" << endl;
		return 0;
	}
		
    // Copy color data for the entire display into a
    // bitmap that is selected into a compatible DC.
    if (!StretchBlt(hdcMem,
        0, 0, grabW, grabH,
        hdcScr,
        grabX, grabY, grabW, grabH,
        SRCCOPY)) {
		cout << "Error 3" << endl;
		return 0;
	}


    // Source[2]
    PBITMAPINFO pbmi;
    WORD cClrBits;

    // Retrieve the bitmap's color format, width, and height.
	if (!GetObjectW(hbmScr, sizeof(BITMAP), (LPSTR) &bmp)) {
		cout << "Error 4" << endl;
		return 0;
	}

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)
    if (cClrBits != 24)
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                sizeof(BITMAPINFOHEADER) +
                sizeof(RGBQUAD) * (1 << cClrBits));

    // There is no RGBQUAD array for the 24-bit-per-pixel format.
    else
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8
                                    * pbmi->bmiHeader.biHeight * cClrBits;

    // Set biClrImportant to 0, indicating that all of the
    // device colors are important.
    pbmi->bmiHeader.biClrImportant = 0;

    HANDLE hf;                  // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE *hp;                   // byte pointer
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER) pbmi;


	int dataSize = pbih->biSizeImage;
	if ( dataSize != grabW * grabH * 4 ) {
		cout << "ERROR Grab, bad data size " << dataSize << ", expect " <<  grabW * grabH * 4 << endl;
	}

	outData32.resize( dataSize );

    lpBits = &outData32[0]; //(LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    //lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) {		
		cout << "Error 5" << endl;
		return 0;
	}


    // Retrieve the color table (RGBQUAD array) and the bits
    // (array of palette indices) from the DIB.
	if (!GetDIBits(hdcMem, hbmScr, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
		cout << "Error 6" << endl;
		return 0;
	}


    return 1;
} 


//--------------------------------------------------------------
pbScreenGrab::pbScreenGrab(void)
{
}

//--------------------------------------------------------------
pbScreenGrab::~pbScreenGrab(void)
{
}

//--------------------------------------------------------------
void pbScreenGrab::setup( int w, int h )
{
	_w = w;
	_h = h;
}

//--------------------------------------------------------------
void pbScreenGrab::close()
{

}

//--------------------------------------------------------------
void pbScreenGrab::grab( int x, int y )
{
	CaptureBMP( x, y, _w, _h, _data );
}

//--------------------------------------------------------------
void pbScreenGrab::getMat( Mat &img )
{
	if ( _data.size() == 4 * _w * _h ) {
		_img = Mat( cv::Size( _w, _h ), CV_8UC4, &_data[0] );
		img = _img.clone();
		flip( img, img, 0 );
	}

}

//--------------------------------------------------------------
